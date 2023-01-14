#include "firefly/timer/timer.hpp"

#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/hpet/hpet.hpp"
#include "firefly/intel64/int/interrupt.hpp"
#include "firefly/intel64/pit/pit.hpp"
#include "firefly/logger.hpp"
#include "firefly/memory-manager/allocator.hpp"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/panic.hpp"
#include "frg/vector.hpp"
#include "libk++/memory.hpp"

// clang-format disable
#include "firefly/tasks/task.hpp"
// clang-format enable

namespace firefly::kernel {
// TODO: rework this, probably a better solution
namespace timer {

namespace {
// This will just get increased
static volatile uint64_t ticks = 0;
static volatile uint32_t ticks_20ms = 0;
static frg::vector<tasks::Task, Allocator> tasks;
static volatile uint8_t cur = 0xFF;
}  // namespace

void timer_irq(Registers* stack) {
    // debugLine << "timer\n"
    //              << fmt::endl;
    switch (cur) {
        case 0xFF:
            tasks[0].load(stack);
            cur = 0;
            break;
        case 0:
            tasks[0].save(stack);
            tasks[1].load(stack);
            cur = 1;
            break;
        case 1:
            tasks[1].save(stack);
            tasks[0].load(stack);
            cur = 0;
            break;
        default:
            break;
    }
    apic::ApicTimer::accessor().oneShotTimer(ticks_20ms);
}

void loop1() {
    debugLine << "loop 1 \n"
              << fmt::endl;

    for (;;) {
    }
}

void loop2() {
    debugLine << "loop 2\n"
              << fmt::endl;

    for (;;) {
    }
}

void start() {
    apic::ApicTimer::accessor().oneShotTimer(ticks_20ms);
}

void init() {
    resetTicks();
    HPET::init();
    core::interrupt::registerIRQHandler(timer_irq, 0);
    if (apic::ApicTimer::isAvailable()) {
        apic::ApicTimer::init();
        ticks_20ms = 200 * apic::ApicTimer::accessor().calibrate(100);
    } else {
        pit::init();
    }

    auto sp1 = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(8192));
    auto sp2 = reinterpret_cast<uintptr_t>(mm::Physical::must_allocate(8192));
    auto task1 = tasks::Task(reinterpret_cast<std::uintptr_t>(&loop1), sp1);
    auto task2 = tasks::Task(reinterpret_cast<std::uintptr_t>(&loop2), sp2);
    tasks.push(task1);
    tasks.push(task2);
    debugLine << "TASK #0 RIP " << fmt::hex << tasks[0].getRip() << '\n'
              << fmt::endl;
    return;
    // panic("No usable timer found");
}


// Probably call this function in the IRQ handler, or whatever the timer fires
void tick() {
    ticks = ticks + 1;
}

void resetTicks() {
    ticks = 0;
}

void msleep(uint64_t ms) {
    usleep(ms * 1000);
}

void usleep(uint64_t us) {
    HPET::accessor().usleep(us);
}


}  // namespace timer
}  // namespace firefly::kernel
