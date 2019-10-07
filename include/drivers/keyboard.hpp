#pragma once
#include "array.h"
#include "drivers/ports.hpp"
#include "drivers/vga.hpp"
#include "optional.h"
#include "utils.hpp"

constexpr short data_port = 0x60;
constexpr short status_register = 0x64;
constexpr short command_register = 0x64;

enum keys : short {
    backspace = 0x0E,
    enter = 0x1C,
    lshift = 0x2A,
    caps_lock = 0x3A,
    max_stdin_length = 255,
};

struct Keyboard {
    Display& disp;

    firefly::std::array<char, keys::max_stdin_length> stdin = {};
    size_t stdin_index = 0;

    bool shift_pressed = false;

    firefly::std::array<const char*, 59> sc_name = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                                                     "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                                                     "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                                                     "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                                                     "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                                                     "/", "RShift", "Keypad *", "LAlt", "Spacebar" };
    firefly::std::array<char, 59> sc_ascii = { '?', '?', '1', '2', '3', '4', '5', '6',
                                               '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                                               'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                                               'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                                               'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ' };

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

    Keyboard(Display& disp);

    void append_cin(char c);

    void wait_for_status();

    firefly::std::optional<unsigned char> get_scancode();

    void handle_input(unsigned char scancode, Display& disp);
};