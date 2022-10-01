#pragma once

#include <cstddef>
#include <cstdint>
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
static constexpr bool debugSlab{ true };
};

/* vm = virtual memory */
template <class VmBackingAllocator, typename Lock>
class slabCache {
    struct slab;
    enum SlabType {
        Small,
        Large
    };

public:
    void initialize(int sz, const frg::string_view& descriptor = "anonymous") {
        // Todo: Ensure sz is a power of 2 and round it to the nearest power of two if it isn't.
        size = sz;
        slab_type = slabTypeOf(sz);
        createSlab(descriptor);

        ConsoleLogger() << "base: " << ConsoleLogger::log().hex(free_slabs.get_root()->base_address) << "\n";
    }

    VirtualAddress allocate() {
        lock.lock();

        slab* _slab = partial_slabs.first();
        VirtualAddress object;

        auto do_dequeue = [&]() {
            return (object = reinterpret_cast<VirtualAddress>(_slab->object_queue.dequeue()));
        };

        // Check if we can allocate an object from the partial slab
        if (!_slab) {
            _slab = free_slabs.first();

            if (unlikely(!_slab)) {
                panic("TODO: Slab cache is OOM. Implement grow()");
                // grow();
                // allocate();
            }

            // Move a free slab into the partial slab
            free_slabs.remove(_slab);
            partial_slabs.insert(_slab);
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
            partial_slabs.remove(_slab);
            full_slabs.insert(_slab);

            // Retry allocation
            lock.unlock();
            allocate();
        }

    end:
        lock.unlock();
        return object;
    }

    void deallocate(VirtualAddress ptr) {
        (void)ptr;
    }

public:
    // Caches are connected to each other via an intrusive doubly linked list
    frg::default_list_hook<slabCache> node;

private:
    void grow();
    void shrink();

    void createSlab(const frg::string_view& descriptor) {
        // Todo: This should probably be handled by the VmBackingAllocator class
        size_t alloc_sz = slab_type == SlabType::Large ? PageSize::Size2M : PageSize::Size4K;

        if constexpr (debugSlab) {
            ConsoleLogger() << "Start of slab creation\n";
            ConsoleLogger() << "slab descriptor='" << descriptor.data() << "'\n";
            ConsoleLogger() << "Creating new slab with an allocation of size '" << alloc_sz << "', object size is: " << size << '\n';
            ConsoleLogger() << "is large slab: " << (alloc_sz == PageSize::Size2M ? "true" : "false") << '\n';
        }

        auto base = reinterpret_cast<uintptr_t>(vm_backing.allocate(alloc_sz));
        slab* _slab = new (reinterpret_cast<void*>(base)) struct slab(descriptor, base, alloc_sz, size);
        free_slabs.insert(_slab);

        if constexpr (debugSlab)
            ConsoleLogger() << "slab can hold " << _slab->max_objects << " objects\nEnd of slab creation\n";
    }

    SlabType slabTypeOf(int size) const {
        return size > static_cast<int>(PageSize::Size4K / 8) ? SlabType::Large : SlabType::Small;
    }

private:
    // Todo: Go through this struct after finishing the slab and weed out unused members to make the struct smaller
    struct slab {
        struct object : frg::default_queue_hook<object> {};

        slab(const frg::string_view& _descriptor, uintptr_t address, size_t len, size_t sz)
            : descriptor{ _descriptor }, base_address{ address } {
            // Reserve some memory for the slab structure & align it to sizeof(slab) bytes.
            // If we don't do this we will end up overwriting the object we just created.
            address += sizeof(slab);
            address = (address + sizeof(slab) - 1) & ~(sizeof(slab) - 1);

            for (auto i = address; i <= (address + len); i += sz) {
                // TODO: VmBackingAllocator should do this.
                // Marks each 4kib page as a slab page.
                if (i % PageSize::Size4K == 0)
                    pagelist.phys_to_page(i)->flags = RawPageFlags::Slab;

                frg::queue_result res = object_queue.enqueue(reinterpret_cast<object*>(i));
                assert_truth(res == frg::queue_result::Okay);
            }

            max_objects = object_queue.size() - 1;
        }

        frg::string_view descriptor;
        uintptr_t base_address;
        int max_objects;

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

    alignas(CACHE_LINE_SIZE)
        frg::rbtree<slab, &slab::tree_hook, comparator> full_slabs,
        free_slabs, partial_slabs;
};
}  // namespace firefly::kernel::mm