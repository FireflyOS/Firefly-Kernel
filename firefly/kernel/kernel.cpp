#include "firefly/kernel.hpp"

#include <frg/array.hpp>

#include "firefly/drivers/ps2.hpp"
#include "firefly/drivers/serial.hpp"
#include "firefly/init/init.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/primary/buddy.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/panic.hpp"


[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel {
void log_core_firefly_contributors() {
    info_logger << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    frg::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            info_logger << info_logger.newline() << info_logger.tab();
        }
        info_logger << " " << arr[i];
    }
    info_logger << info_logger.newline();
}

#include "firefly/memory-manager/zone-specifier.hpp"

[[noreturn]] void kernel_main() {
    // mm::VirtualMemoryManager vmm{ true };

    for (int i = 0; i < 6; i++)
    {
        auto ptr = mm::pmm::allocate();
        info_logger << info_logger.hex(ptr) << '\n';
    }

    // A little buddy allocator test.
    // Note: This may not work for you because it manages 8192 bytes,
    // but only 4096 have been allocated. This is because our buddy
    // allocator can only allocate 4096 as the smallest allocation which wouldn't make for a good test.
    // info_logger << "\nBuddy test: Allocating, freeing and allocating a page..\n";
    // mm::BuddyAllocator instance((uint64_t *)mm::pmm::allocate(), 13);

    // auto ptr = instance.alloc(8192);
    // info_logger << "ptr: " << info_logger.hex(ptr) << '\n';
    // instance.free(ptr, 12);

    // ptr = instance.alloc(4096);
    // info_logger << "ptr: " << info_logger.hex(ptr) << '\n';

    panic("Reached the end of the kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel
