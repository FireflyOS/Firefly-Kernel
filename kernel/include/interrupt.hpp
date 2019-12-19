#pragma once

namespace interrupt {
    /**
     *                      initializes the idt
     */
    void init();

    // test interrupt handler
    void test_int();
}