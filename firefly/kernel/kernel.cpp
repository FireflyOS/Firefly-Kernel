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

[[noreturn]] void kernel_main() {
    mm::VirtualMemoryManager vmm{ true };

    // Test OOM
    // for(;;)
    // {
    //     auto ptr = mm::pmm::allocate();
    //     if (!ptr)
    //         break;
    //     info_logger << info_logger.hex(ptr) << '\n';
    // }

    panic("Reached the end of the kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel
