#pragma once

#include <cstdlib/cassert.h>

#include <cstdint>

namespace firefly::kernel::mm {

// This allocator is meant to initialize other allocators and should be discarded after booting.
// (TODO: Put it in a discard_after_boot section or something)
class BootstrapAllocator {
private:
    static constexpr bool sanity_checks{ true };
    uint8_t *base{};
    uint64_t size, length;

public:
    void init_buffer(uint64_t base, uint64_t size, uint64_t length) {
        info_logger << "BootstrapAllocator-init: base: " << info_logger.hex(base) << " size: " << size << " len: " << length << '\n';
        this->base = reinterpret_cast<uint8_t *>(base);
        this->size = size;
        this->length = length;
    }

    void *allocate_buffer() {
        auto _base = base;
        base += size;

        if (base >= base + length) {
            if constexpr (sanity_checks) {
                panic("Failed to allocate a memory buffer. Try increasing the buffers size during initialization.");
            }
            return nullptr;
        }

        return _base;
    }
};
}  // namespace firefly::kernel::mm
