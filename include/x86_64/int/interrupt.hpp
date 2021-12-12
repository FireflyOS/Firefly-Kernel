#pragma once

struct __attribute__((packed)) iframe {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    uint64_t err;
    uint64_t int_no;
};
namespace firefly::kernel::core::interrupt {
    
    void init();

    // test interrupt handler
    void test_int();

    namespace change{ 
        void update(void (*handler)(iframe *), uint16_t cs, uint8_t type, uint8_t index);
    }

}  // namespace firefly::kernel::interrupt