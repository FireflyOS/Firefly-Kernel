#pragma once

#include <cstdint>

#include "firefly/intel64/page_permissions.hpp"
#include "firefly/limine.hpp"
#include "firefly/memory-manager/mm.hpp"

namespace firefly::kernel::core::paging {
void invalidatePage(const VirtualAddress page);
void invalidatePage(const uint64_t page);
void map(const uint64_t virtual_addr, const uint64_t physical_addr, AccessFlags access_flags, const uint64_t *pml_ptr, const PageSize page_size = SIZE_4KB);
void bootMapExtraRegion(limine_memmap_response *mmap);
}  // namespace firefly::kernel::core::paging
