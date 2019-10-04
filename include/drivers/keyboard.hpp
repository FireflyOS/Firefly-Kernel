#pragma once
#include "drivers/ports.hpp"

struct Keyboard {
    enum status : short {
        out_buffer_status = 0b10000000,
        in_buffer_status = 0b01000000,
        system_flag = 0b0010000,
        command_data = 0b00010000,
        unknown_lock = 0b00001000,
        unknown_timeout = 0b00000100,
        timeout_error = 0b00000010,
        parity_error = 0b00000001
    };

    unsigned char get_scancode() {
        if (firefly::read_port(0x64) & status::out_buffer_status) {
            return firefly::read_port(0x60);
        }
        return '\0';
    }

    void handle_input(char scancode) {
        (void) scancode;
    }
    
};