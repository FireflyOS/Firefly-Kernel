#pragma once

namespace firefly::kernel::core::interrupt {
    void init();

    // test interrupt handler
    void test_int();

    namespace change{ 
        void update(uint32_t handler, uint16_t cs, uint8_t type, uint8_t index);
    }

}  // namespace firefly::kernel::interrupt