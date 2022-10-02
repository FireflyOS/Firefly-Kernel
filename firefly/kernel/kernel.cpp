#include "firefly/kernel.hpp"

#include <frg/array.hpp>

#include "firefly/drivers/serial.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/logger.hpp"
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


#include "firefly/memory-manager/primary/primary_phys.hpp"

// Dummy VmBackingAllocator
class Foo {
public:
    void *allocate(int s) {
        return firefly::kernel::mm::Physical::allocate(s);
    }
};


// Dummy locking mechanism
class Bar {
public:
    void lock() {
    }
    void unlock() {
    }
};

struct dummyCache {
    int foo;
    char bar;
    long baz;
};

[[noreturn]] void kernel_main() {
    log_core_firefly_contributors();
    core::acpi::Acpi::accessor().dumpTables();

	// Test heap
    mm::kernelHeap::init();

	// Allocate 4 bytes
	auto ptr = mm::heap->allocate(4);
	ConsoleLogger() << "ptr=" << ConsoleLogger::log().hex(reinterpret_cast<uintptr_t>(ptr)) << '\n';

	// Deallocate and allocate 4 bytes again (should print the same address)
	mm::heap->deallocate(ptr);
	ptr = mm::heap->allocate(4);
	ConsoleLogger() << "ptr=" << ConsoleLogger::log().hex(reinterpret_cast<uintptr_t>(ptr)) << '\n';

    panic("Reached the end of the kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel
