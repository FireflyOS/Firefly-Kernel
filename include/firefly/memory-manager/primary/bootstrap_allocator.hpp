#pragma once

#include <cstdlib/cassert.h>

#include <cstdint>

namespace firefly::kernel::mm {

// This allocator is meant to initialize other allocators and should be discarded after booting.
// (TODO: Put it in a discard_after_boot section or something)
class BootstrapAllocator {
private:
    static constexpr auto block_size = sizeof(int) * 8;
    static constexpr auto largest_size = 128;
    static constexpr uint8_t reserved{ 1 }, free{ 0 };
    static constexpr bool sanity_checks{};

    uint8_t map[largest_size]{};  // Allows up to 128 allocations of one size
    uint64_t *base{};
    uint64_t size, length;

public:
    void init_buffer(uint64_t base, uint64_t size, uint64_t length) {
        this->base = reinterpret_cast<uint64_t *>(base);
        this->size = size;
        this->length = length;
    }

    void *allocate_buffer() {
        for (uint64_t i = 0; i < length; i++) {
            if (map[i] == free) {
                map[i] = reserved;
                return reinterpret_cast<void *>(((uintptr_t)base * i) + size);
            }
        }

        if constexpr (sanity_checks) {
            panic("Failed to allocate a memory buffer. Try increasing `BootstrapAllocator.largest_size`");
            __builtin_unreachable();
        }

        return nullptr;
    }
};
}  // namespace firefly::kernel::mm
