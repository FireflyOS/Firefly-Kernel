#include "firefly/kernel.hpp"

#include <frg/array.hpp>
#include <frg/vector.hpp>

#include "firefly/drivers/serial.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/secondary/heap.hpp"
#include "firefly/panic.hpp"

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel {
void log_core_firefly_contributors() {
    ConsoleLogger::log() << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    frg::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            ConsoleLogger::log() << ConsoleLogger::log().newline() << ConsoleLogger::log().tab();
        }
        ConsoleLogger::log() << " " << arr[i];
    }
    ConsoleLogger::log() << ConsoleLogger::log().newline();
}

[[noreturn]] void kernel_main() {
    log_core_firefly_contributors();
    core::acpi::Acpi::accessor().dumpTables();

    mm::kernelHeap::init();

    // Testing the heap with a vector
    frg::vector<int, Allocator> vec;
    vec.push(1);
    vec.push(2);
    vec.push(3);
    ConsoleLogger() << "Vec.size: " << vec.size() << ", vec.front: " << vec.front() << "\n";

    // Testing the heap with allocations
    // Allocate 4 bytes
    auto ptr = mm::heap->allocate(4);
    ConsoleLogger() << "ptr=" << ConsoleLogger::log().hex(reinterpret_cast<uintptr_t>(ptr)) << '\n';

    // Deallocate and allocate 4 bytes again (should print the same address)
    ConsoleLogger() << "Deallocating ptr and allocating memory...\n";
    mm::heap->deallocate(ptr);
    ptr = mm::heap->allocate(4);
    ConsoleLogger() << "ptr=" << ConsoleLogger::log().hex(reinterpret_cast<uintptr_t>(ptr)) << '\n';

    panic("Reached the end of the kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel
