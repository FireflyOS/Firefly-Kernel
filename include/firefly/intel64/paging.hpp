#pragma once

#include <cstdint>

#include "firefly/intel64/page_flags.hpp"
#include "firefly/intel64/page_permissions.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::core::paging {
void invalidatePage(const VirtualAddress page);
void invalidatePage(const uint64_t page);
void map(uint64_t virtual_addr, uint64_t physical_addr, AccessFlags access_flags, Pml *root, const PageSize page_size);
}  // namespace firefly::kernel::core::paging
