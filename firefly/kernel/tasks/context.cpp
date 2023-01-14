#include "firefly/tasks/context.hpp"

#include "firefly/logger.hpp"

namespace firefly::kernel::tasks {
void Context::save(const Registers *regs) {
    /*
      debugLine << "Saving RIP at " << fmt::hex
                << regs->rip
                << '\n'
                << fmt::endl;
                */
    this->regs = *regs;
}
void Context::load(Registers *regs) {
    /*
      debugLine << "loading RIP at " << fmt::hex
                << this->regs.rip
                << "\n From original RIP at " << fmt::hex
                << regs->rip
                << '\n'
                << fmt::endl;
                */
    *regs = this->regs;
}
}  // namespace firefly::kernel::tasks
