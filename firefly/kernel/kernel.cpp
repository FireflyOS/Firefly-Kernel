#include "firefly/kernel.hpp"

#include <frg/array.hpp>
#include <frg/vector.hpp>

#include "firefly/drivers/ps2.hpp"
#include "firefly/drivers/serial.hpp"
#include "firefly/intel64/acpi/acpi.hpp"
#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/secondary/heap.hpp"
#include "firefly/panic.hpp"
#include "firefly/tasks/scheduler.hpp"
#include "firefly/timer/timer.hpp"

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel {
void log_core_firefly_contributors() {
    logLine << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    frg::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            logLine << '\n'
                    << '\t';
        }
        logLine << " " << arr[i];
    }
    logLine << '\n'
            << fmt::endl;
}

void loop1() {
    for (;;)
        debugLine << "loop 1 \n"
                  << fmt::endl;
}

void loop2() {
    for (;;)
        debugLine << "loop 2\n"
                  << fmt::endl;
}

void loop3() {
    for (;;)
        debugLine << "loop 3\n"
                  << fmt::endl;
}

[[noreturn]] void kernel_main() {
    log_core_firefly_contributors();
    core::acpi::Acpi::accessor().dumpTables();

    firefly::drivers::ps2::init();
    tasks::Scheduler::init();

    auto sp1 = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(8192));
    auto sp2 = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(8192));
    auto sp3 = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(8192));

    tasks::Scheduler::accessor().addTask(tasks::Task(reinterpret_cast<std::uintptr_t>(&loop1), sp1));
    tasks::Scheduler::accessor().addTask(tasks::Task(reinterpret_cast<std::uintptr_t>(&loop2), sp2));
    tasks::Scheduler::accessor().addTask(tasks::Task(reinterpret_cast<std::uintptr_t>(&loop3), sp3));

    timer::start();

    for (;;) {
        // wait for scheduler to start
    }
    panic("Reached the end of the kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel
