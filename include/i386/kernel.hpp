#pragma once

#include <i386/drivers/vga.hpp>

namespace firefly::kernel::main {
class Singleton {
public:
    static Singleton &Get() {
        if (instance.dead) {
            //Error out
        }
        return instance;
    }

    firefly::drivers::vga::cursor &internal_cursor_handle() {
        return cout;
    }

    void internal_set_cursor_handle(firefly::drivers::vga::cursor &object) {
        cout = object;
    }

private:
    Singleton() {
    }
    static Singleton instance;
    bool dead = false;
    firefly::drivers::vga::cursor cout;
};

// firefly::drivers::vga::cursor &internal_cursor_handle();
}  // namespace firefly::kernel::main