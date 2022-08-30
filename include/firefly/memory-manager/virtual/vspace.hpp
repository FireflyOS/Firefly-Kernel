#pragma once

#include "firefly/intel64/paging.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"

// #pragma region Macro wrappers
/* Use these macros when no custom logic is needed. It's shorter and cleaner. */
#define VIRTUAL_SPACE_FUNC_UNMAP        \
    void unmap(T virt) const override { \
        VirtualSpace::unmap(virt);      \
    }

#define VIRTUAL_SPACE_FUNC_MAP                                   \
    void map(T virt, T phys, AccessFlags flags) const override { \
        VirtualSpace::map(virt, phys, flags);                    \
    }

#define VIRTUAL_SPACE_FUNC_MAP_RANGE                                                                         \
    void mapRange(T base, T len, AccessFlags flags, AddressLayout off = AddressLayout::Low) const override { \
        VirtualSpace::mapRange(base, len, flags, off);                                                       \
    }
// #pragma endregion

namespace firefly::kernel::mm {

using core::paging::AccessFlags;
using core::paging::CacheMode;

class VirtualSpace {
public:
    VirtualSpace() = default;
    virtual ~VirtualSpace() {
        Physical::deallocate(PhysicalAddress(pml4));
    }

    void operator delete([[maybe_unused]] void *ptr) {
    }

protected:
    using T = uint64_t;

    inline void initSpace(PhysicalAddress root) {
        pml4 = static_cast<T *>(root);
    }

    virtual void map(T virtual_addr, T physical_addr, AccessFlags flags) const {
        core::paging::map(virtual_addr, physical_addr, flags, pml4);
    }

    virtual void mapRange(uint64_t base, uint64_t len, AccessFlags flags, AddressLayout offset = AddressLayout::Low) const {
        for (uint64_t i = base; i < (base + len); i += PAGE_SIZE)
            map(i + offset, i, flags);
    }

    virtual void unmap([[maybe_unused]] T virtual_addr) const {
        firefly::panic("unmap() has not been implemented");
    }

    inline void invalidate(VirtualAddress page) const {
        core::paging::invalidatePage(page);
    }

    inline void invalidate(T page) const {
        core::paging::invalidatePage(page);
    }

    inline T root() const {
        return reinterpret_cast<T>(pml4);
    }

    inline void loadAddressSpace() const {
        asm volatile("mov %0, %%cr3" ::"r"(reinterpret_cast<uintptr_t>(pml4)));
    }

private:
    T *pml4;
};
}  // namespace firefly::kernel::mm