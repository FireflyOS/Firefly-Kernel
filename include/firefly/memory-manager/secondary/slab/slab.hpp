#pragma once

#include <cstddef>
#include <cstdint>
#include <frg/array.hpp>
#include <frg/list.hpp>
#include <frg/queue.hpp>
#include <frg/rbtree.hpp>
#include <frg/string.hpp>

#include "firefly/intel64/cache.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::mm {

namespace {
static constexpr bool debugSlab{ false };
static constexpr bool sanityCheckSlab{ true };
};  // namespace

/* vm = virtual memory */
template <class VmBackingAllocator, typename Lock>
class slabCache {
    static constexpr int object_size{ 8 };  // sizeof(slab::object) aka sizeof(frg::intrusive_queue<T>::hook) aka sizeof(pointer)
    struct slab;
    enum SlabType {
        Small,
        Large
    };

    enum SlabState {
        full,
        partial,
        free
    };

    constexpr bool powerOfTwo(int n) const {
        return (n > 0) && ((n & (n - 1)) == 0);
    }

    // Compute the next highest power of 2 of 32-bit 'n'
    constexpr int alignToSecondPower(int n) {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return ++n;
    }

public:
    slabCache() = default;
    slabCache(int sz, const frg::string_view& descriptor = "anonymous") {
        initialize(sz, std::move(descriptor));
    }

    void initialize(int sz, const frg::string_view& descriptor = "anonymous") {
        // Minimum allocation size has to be the size of a 'struct object', otherwise the
        // intrusive queue will end up in trouble since it's given too little memory.
        if (sz < object_size)
            sz = object_size;

        if (!powerOfTwo(size))
            size = alignToSecondPower(size);

        size = sz;
        slab_type = slabTypeOf(sz);
        createSlab(descriptor);
    }

    VirtualAddress allocate() {
        lock.lock();

        slab* _slab = slabs[SlabState::partial].first();
        VirtualAddress object;

        auto do_dequeue = [&]() {
            return (object = reinterpret_cast<VirtualAddress>(_slab->object_queue.dequeue()));
        };

        // Check if we can allocate an object from the partial slab
        if (!_slab) {
            _slab = slabs[SlabState::free].first();

            if (unlikely(!_slab)) {
                panic("TODO: Slab cache is OOM. Implement grow()");
                // grow();
                // allocate();
            }

            // Move a free slab into the partial slab
            slabs[SlabState::free].remove(_slab);
            slabs[SlabState::partial].insert(_slab);

            _slab->slab_state = SlabState::partial;
            do_dequeue();

            if constexpr (debugSlab)
                assert_truth(object != nullptr && "This should NOT happen, something went really wrong");

            goto end;
        }

        // Perform the allocation
        do_dequeue();

        // Check if this slab is full and move it
        // into the full slab tree if applicable
        if (!object || _slab->object_queue.size() == 0) {
            // Fix slab state
            slabs[SlabState::partial].remove(_slab);
            slabs[SlabState::full].insert(_slab);
            _slab->slab_state = SlabState::full;

            // Retry allocation
            lock.unlock();
            allocate();
        }

    end:
        lock.unlock();
        return object;
    }

    void deallocate(VirtualAddress ptr) {
        // Slabs are always aligned on 4kib boundaries.
        // A 4kib aligned address has it's lowest 12 bits cleared, that's what we're doing here.
        constexpr const int shift = PAGE_SHIFT;
        slab* _slab = reinterpret_cast<slab*>((reinterpret_cast<uintptr_t>(ptr) >> shift) << shift);

        if constexpr (sanityCheckSlab) {
            assert_truth(_slab->object_queue.size() - 1 < static_cast<size_t>(_slab->max_objects) && "Tried to free non-allocated address");
            assert_truth(_slab->slab_state != SlabState::free && "Tried to free non-allocated address");
        }

        if constexpr (debugSlab) {
            ConsoleLogger() << "Slab is located at: " << ConsoleLogger::log().hex(_slab) << '\n';
            ConsoleLogger() << "Descriptor: " << _slab->descriptor.data() << ", state: " << (int)_slab->slab_state << '\n';
        }

        // This object is most likely still in the cache, so we enqueue it at the head.
        // This will make sure we hand out "cache warm" objects whenever possible.
        _slab->object_queue.enqueue_head(reinterpret_cast<slab::object*>(ptr));

        // If this object was full, move it the the partial slab
        if (_slab->slab_state == SlabState::full) {
            auto previous_state = _slab->slab_state;
            _slab->slab_state = SlabState::partial;

            slabs[previous_state].remove(_slab);
            slabs[SlabState::partial].insert(_slab);
        }
    }

private:
    void grow();
    void shrink();

    void createSlab(const frg::string_view& descriptor) {
        size_t alloc_sz = slab_type == SlabType::Large ? PageSize::Size2M : PageSize::Size4K;

        if constexpr (debugSlab) {
            ConsoleLogger() << "Start of slab creation\n";
            ConsoleLogger() << "slab descriptor='" << descriptor.data() << "'\n";
            ConsoleLogger() << "Creating new slab with an allocation of size '" << alloc_sz << "', object size is: " << size << '\n';
            ConsoleLogger() << "is large slab: " << (alloc_sz == PageSize::Size2M ? "true" : "false") << '\n';
        }

        auto base = reinterpret_cast<uintptr_t>(vm_backing.allocate(alloc_sz));
        slab* _slab = new (reinterpret_cast<void*>(base)) struct slab(descriptor, base, alloc_sz, size);
        slabs[SlabState::free].insert(_slab);

        if constexpr (debugSlab)
            ConsoleLogger() << "slab can hold " << _slab->max_objects << " objects\nEnd of slab creation\n";
    }

    SlabType slabTypeOf(int size) const {
        return size > static_cast<int>(PageSize::Size4K / 8) ? SlabType::Large : SlabType::Small;
    }

private:
    struct slab {
        struct object : frg::default_queue_hook<object> {};

        slab(const frg::string_view& _descriptor, uintptr_t address, size_t len, size_t sz)
            : descriptor{ _descriptor } {
            // Note:
            // Technically this is breaking the spec which mandates the slab be placed at the _end_, but
            // I prefer it this way and don't see any major issues as a result of doing it like this. - V01D
            //
            // Reserve some memory for the slab structure & align it to sizeof(slab) bytes.
            // If we don't do this we will end up overwriting the object we just created.
            ConsoleLogger() << "base addr of slab=" << ConsoleLogger::log().hex(address) << '\n';
            address += sizeof(slab);
            address = (address + sizeof(slab) - 1) & ~(sizeof(slab) - 1);
            base_address = address;

            for (auto i = address; i <= (address + len); i += sz) {
                // TODO: VmBackingAllocator should do this.
                // Marks each 4kib page as a slab page.
                if (i % PageSize::Size4K == 0) {
                    auto const& page = pagelist.phys_to_page(i - AddressLayout::SlabHeap);
                    page->flags = RawPageFlags::Slab;
                    page->slab_size = sz;
                }

                frg::queue_result res = object_queue.enqueue(reinterpret_cast<object*>(i));
                assert_truth(res == frg::queue_result::Okay);
            }

            slab_state = SlabState::free;
            max_objects = object_queue.size() - 1;
        }

        frg::string_view descriptor;
        uintptr_t base_address;
        size_t max_objects;

        SlabState slab_state;
        frg::intrusive_queue<object> object_queue;
        frg::rbtree_hook tree_hook;
    };

    struct comparator {
        bool operator()(const slab& a, const slab& b) {
            return a.base_address < b.base_address;
        }
    };

    int size;
    Lock lock;
    SlabType slab_type;
    VmBackingAllocator vm_backing;

    // Full, partial and free slabs
    alignas(CACHE_LINE_SIZE) frg::array<
        frg::rbtree<
            slab,
            &slab::tree_hook,
            comparator>,
        3> slabs = {};
};
}  // namespace firefly::kernel::mm