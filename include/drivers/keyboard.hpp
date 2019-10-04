#pragma once
#include "drivers/ports.hpp"
#include "optional.h"

constexpr short data_port = 0x60;
constexpr short status_register = 0x64;
constexpr short command_register = 0x64;

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

    firefly::std::optional<unsigned char> get_scancode() {
        if (firefly::read_port(status_register) & status::out_buffer_status) {
            return firefly::read_port(data_port);
        }
        return nullptr;
    }

    void handle_input(char scancode) {
        (void) scancode;
    }
    
};