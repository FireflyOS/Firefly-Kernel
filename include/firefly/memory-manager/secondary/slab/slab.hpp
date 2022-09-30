#pragma once

#include <cstddef>
#include <cstdint>
#include <frg/list.hpp>
#include <frg/queue.hpp>
#include <frg/rbtree.hpp>
#include <frg/string.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/page.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"


namespace firefly::kernel::mm::secondary {

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
    slabCache() = default;

    void initialize(int sz, const frg::string_view& descriptor = "anonymous") {
        // Todo: Ensure sz is a power of 2 and round it to the nearest power of two if it isn't.
        size = sz;
        createSlab(descriptor);
    }

    VirtualAddress allocate() {
        // Todo:
        // - Check if _slab is null and expand if true (maybe add some flags to enabled/disable auto resizing?)
        // - Check the available_object_count and move slabs into used/free/partial slabs, this will speed up allocation by a lot.
        auto _slab = slabs.first();
        auto object = _slab->avail_page.dequeue();

        if constexpr (debugSlab)
            ConsoleLogger() << "Allocating from a cache with the following descriptor: " << _slab->descriptor.data() << '\n';

        // Just a quick todo note for me - V01D
        if (_slab->avail_page.empty())
            panic("Detected fully allocated slab, need to implement: full, partial and free slabs.");

        return VirtualAddress(object);
    }

    void deallocate(VirtualAddress ptr) {
        (void)ptr;
    }

private:
    void grow(SlabType type);
    void shrink(SlabType type);

    void createSlab(frg::string_view descriptor) {
        // Todo: This should probably be handled by the VmBackingAllocator class
        size_t alloc_sz = slabTypeOf(size) == SlabType::Large ? PageSize::Size2M : PageSize::Size4K;
        ConsoleLogger() << "alloc_sz: " << alloc_sz << ", object size: " << size << '\n';
        ConsoleLogger() << "is large slab: " << (alloc_sz == PageSize::Size2M ? "true" : "false") << '\n';

        auto base = reinterpret_cast<uintptr_t>(vm_backing.allocate(alloc_sz));
        struct slab* _slab = new (reinterpret_cast<void*>(base)) struct slab(descriptor, base, alloc_sz, size);
        slabs.insert(_slab);
    }

    SlabType slabTypeOf(int size) const {
        return size > static_cast<int>(PageSize::Size4K / 8) ? SlabType::Large : SlabType::Small;
    }

private:
    struct slab {
        struct object : frg::default_queue_hook<object> {};

        slab(const frg::string_view& _descriptor, uintptr_t _address, size_t len, size_t sz)
            : descriptor{ _descriptor }, address{ _address } {
            // Reserve some memory for the slab structure & align it to sizeof(slab) bytes.
            // If we don't do this we will end up overwriting the object we just created.
            _address += sizeof(slab);
            _address = (_address + sizeof(slab) - 1) & ~(sizeof(slab) - 1);

            for (auto i = _address; i <= (_address + len); i += sz) {
                // TODO: VmBackingAllocator should do this.
                // Marks each 4kib page as a slab page.
                if (i % PageSize::Size4K == 0)
                    pagelist.phys_to_page(i)->flags = RawPageFlags::Slab;

                frg::queue_result res = avail_page.enqueue(reinterpret_cast<object*>(i));
                assert_truth(res == frg::queue_result::Okay);
            }
        }

        frg::string_view descriptor;
        int available_object_count;
        uintptr_t address;

        frg::intrusive_queue<object> avail_page;
        frg::rbtree_hook tree_hook;
    };

    struct comparator {
        bool operator()(const slab& a, const slab& b) {
            return a.address < b.address;
        }
    };

    int size;
    Lock lock;
    VmBackingAllocator vm_backing;
    frg::rbtree<slab, &slab::tree_hook, comparator> slabs;  // TODO: Add {used, free, partial} slabs.

    // Add a queue/freelist of Physical::allocate()'d addresses.
    // These will be used to quickly preallocate and assign memory
    // when creating a new slab.
    //
    // Edit: ... ok maybe this isn't useful.
    // We already check for large or small slabs and allocate memory accordingly.
    // I don't see a meaningful performance gain in caching some addresses, the buddy
    // just needs some optimization and we'll be fine. - V01D

    // Todo:
    //  Link slabCaches together via a linked list
};
}  // namespace firefly::kernel::mm::secondary