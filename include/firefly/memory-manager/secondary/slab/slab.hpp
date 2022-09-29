#pragma once

#include <cstddef>
#include <cstdint>
#include <frg/list.hpp>
#include <frg/rbtree.hpp>
#include <frg/string.hpp>

#include "firefly/logger.hpp"
#include "firefly/memory-manager/mm.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "libk++/bits.h"

namespace firefly::kernel::mm::secondary {

/* vm = virtual memory */
template <class VmBackingAllocator, typename Lock>
class slabCache {
    struct slab;
    using BitmapType = uint8_t*;  // TODO: Implement frg::bitset
    enum SlabType {
        Small,
        Large
    };

public:
    slabCache() = default;

    void initialize(int sz, const frg::string_view& descriptor = "anonymous") {
        createSlab(descriptor);
        ConsoleLogger() << "root slab descriptor: " << slabs.get_root()->descriptor.data() << '\n';
        size = sz;
    }

    VirtualAddress allocate() {
        slab* _slab = slabs.first();
        SerialLogger() << "Found slab at: " << SerialLogger::log().hex(_slab->address) << '\n';

        for (int i = 0; i < _slab->available_object_count; i++) {
            if (!_slab->check(i)) {
                _slab->set(i);
                --_slab->available_object_count;
                return VirtualAddress(_slab->address + (i * size));
            }
        }

        return nullptr;
    }

    void deallocate(VirtualAddress ptr) {
        (void)ptr;
    }

    // Allow the user to allocate a range of memory ahead of time to avoid
    // frequent creation of objects which negatively impacts performance.
    void reserveRange(int range) const {
		(void)range;
        // Here we would basically vm_backing.allocate(range)
        // See comment at the bottom of this class for more.
    }

private:
    void grow(SlabType type);
    void shrink(SlabType type);

    void createSlab(frg::string_view descriptor) {
        auto base = reinterpret_cast<uintptr_t>(vm_backing.allocate(4096));
        struct slab* _slab = (struct slab*)base;

        _slab->objects = (BitmapType)(base + sizeof(slab));
        memset(_slab->objects, 0, size);

        _slab->available_object_count = 512;
        _slab->descriptor = std::move(descriptor);
        _slab->address = base;
        _slab->objects = reinterpret_cast<uint8_t*>(base);
        slabs.insert(_slab);
    }

    SlabType slabTypeOf(int size) {
        return size > static_cast<int>(PageSize::Size4K / 8) ? SlabType::Large : SlabType::Small;
    }

private:
    struct slab {
        frg::string_view descriptor;
        int available_object_count;
        BitmapType objects;
        uintptr_t address;

        slab() = default;
        slab(const frg::string_view& _descriptor, BitmapType bitmap_address, uintptr_t _address)
            : descriptor{ _descriptor }, objects{ bitmap_address }, address{ _address } {
        }

        // Bitmap operations
        inline void set(const int bit) {
            objects[bit / blk_size] |= BIT((bit % blk_size));
        }

        inline void clear(const int bit) {
            objects[bit / blk_size] &= ~BIT((bit % blk_size));
        }

        inline bool check(const int bit) const {
            return objects[bit / blk_size] & BIT((bit % blk_size));
        }

        static constexpr int blk_size{ sizeof(BitmapType) * 8 };
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
};
}  // namespace firefly::kernel::mm::secondary