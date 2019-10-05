#pragma once
#include "array.h"
#include "drivers/ports.hpp"
#include "drivers/vga.hpp"
#include "optional.h"
#include "utils.hpp"

constexpr short data_port = 0x60;
constexpr short status_register = 0x64;
constexpr short command_register = 0x64;

const firefly::std::array<uint8_t, 0xFF> _keys = {
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    // up to 0x0d
    '`', '\0', '\0', '\0', '\0', '\0', '\0', 'G'
};


struct Keyboard {
    enum status : short {
        out_buffer_status = 0b00000001,
        in_buffer_status = 0b00000010,
        system_flag = 0b00000100,
        command_data = 0b00001000,
        unknown_lock = 0b00010000,
        unknown_timeout = 0b00100000,
        timeout_error = 0b01000000,
        parity_error = 0b10000000
    };

    Keyboard(Display& disp) {
        start_load(disp, "Loading keyboard driver");
        end_load(disp, "Keyboard driver");
    }

    void wait_for_status() {
        while (!(firefly::read_port(status_register) & status::in_buffer_status))
            ;
    }

    const firefly::std::array<unsigned char, 255> _letters = {

    };

    firefly::std::optional<unsigned char> get_scancode() {
        if (firefly::read_port(status_register) & status::out_buffer_status) {
            return firefly::read_port(data_port);
        }
        return nullptr;
    }

    void handle_input(unsigned char scancode, Display& disp) {
        disp << _keys[scancode] << "\n";
    }
};