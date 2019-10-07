#include "drivers/keyboard.hpp"

Keyboard::Keyboard(Display& disp)
    : disp{ disp } {
    start_load(disp, "Loading keyboard driver");
    end_load(disp, "Keyboard driver");
}

void Keyboard::append_cin(char c) {
    stdin[stdin_index++] = c;
}

void Keyboard::wait_for_status() {
    while (!(firefly::read_port(status_register) & status::in_buffer_status))
        ;
}

firefly::std::optional<unsigned char> Keyboard::get_scancode() {
    if (firefly::read_port(status_register) & status::out_buffer_status) {
        return firefly::read_port(data_port);
    }
    return nullptr;
}

void Keyboard::handle_input(unsigned char scancode, Display& disp) {
    if (scancode == keys::caps_lock) {
        shift_pressed = !shift_pressed;
        return;
    }

    if (scancode & 0x80) {
        if ((scancode & ~(1UL << 7)) == keys::lshift) {
            shift_pressed = !shift_pressed;
        }
    } else {
        if (scancode == keys::lshift) {
            shift_pressed = !shift_pressed;
        } else if (scancode == keys::enter) {
            disp << "\n";
            append_cin('\n');
        } else if (scancode == keys::backspace) {
            disp << '\b';
            append_cin('\b');
        } else {
            char ascii = sc_ascii[scancode];
            if (shift_pressed)
                ascii -= 32;
            append_cin(ascii);
            char str[2] = { ascii, '\0' };
            disp << str;
        }
    }
}