#include <x86_64/drivers/ps2.hpp>
#include <stl/cstdlib/stdio.h>
#include <x86_64/applications/application_manager.hpp>
#include <x86_64/drivers/ps2mouse.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::drivers::ps2 {
    using namespace firefly::kernel::io;

    constexpr short data_port = 0x60;
    constexpr short status_register = 0x64;

    void (*key_handle)(void);
    uint8_t *current_key;
    // constexpr short command_register = 0x64;

    enum keys : unsigned char {
        backspace = 0x0E,
        enter = 0x1C,
        lshift = 0x2A,
        caps_lock = 0x3A,
        max_stdin_length = 255,
    };

    /**
 *                          Buffer for CIN characters, limited at 255
 */
    char stdin[keys::max_stdin_length + 1] = {};

    /**
 *                          Current stdin write index
 */
    static size_t stdin_index = 0;

    /**
 *                          True if shift or caps lock is active, false if not
 */
    bool shift_pressed = false;

    /**
 *                          Array for key names
 */
    const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                                                            "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                                                            "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                                                            "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                                                            "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                                                            "/", "RShift", "Keypad *", "LAlt", "Spacebar" };

    /**
 *                          Array for ascii key values
 */
    static char sc_ascii[59] = { '?', '?', '1', '2', '3', '4', '5', '6',
                                                      '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                                                      'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                                                      'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                                                      'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ' };

    /**
 *                          Enum used for io port statuses
 */
    enum status : short {
        // 1 : data to be read
        out_buffer_status = 0b00000001,
        // 0 : data may be written
        in_buffer_status = 0b00000010,
        // 1 : passes POST
        system_flag = 0b00000100,
        // 0 : data written is data
        // 1 : data written is command
        command_data = 0b00001000,
        // may be keyboard lock,
        // most likely unused
        unknown_lock = 0b00010000,
        // may be timeout,
        // or second ps/2 port output buffer full
        unknown_timeout = 0b00100000,
        timeout_error = 0b01000000,
        parity_error = 0b10000000
    };

    bool init() {
        return (inb(status_register) & status::system_flag) ? true : false;
    }

    /**
 *                          Appends a character to cin and
 *                          increments stdin_index
 * @param c                 char to be appended
 */
    void append_cin(char c) {
        stdin[stdin_index++] = c;
    }

    firefly::std::optional<unsigned char> get_scancode() {
        // ensure there is data to be read
        if (inb(status_register) & status::out_buffer_status)
            return inb(data_port);
        return nullptr;
    }

    void redirect_to_app(void(*keyboard_handle)(void), uint8_t *currentkey){
        key_handle = keyboard_handle;
        current_key = currentkey;
    }
    void app_input() {
        //firefly::kernel::io::legacy::writeTextSerial(": app_input()\n");
        if (!(inb(status_register) & status::out_buffer_status)) return;

        uint8_t scancode = inb(0x60);

        if (scancode == keys::caps_lock) {
            shift_pressed = !shift_pressed;
            return;
        }

        if (scancode & 0x80) {
            if ((scancode & ~(1UL << 7)) == keys::lshift)
                shift_pressed = !shift_pressed;
        } else {
            //if(scancode != 0) printf("0x%X (0x%X) ", scancode, sc_ascii[scancode]);
            switch (scancode) {
                case keys::lshift: {
                    shift_pressed = !shift_pressed;
                    break;
                }
                case keys::enter: {
                    *current_key = '\n';
                    printf("\n");

                    break;
                }
                case keys::backspace: {
                    *current_key = '\b';
                    puts("\b");
                    break;
                }
                default: {
                    char ascii = sc_ascii[scancode];
                    if (shift_pressed) ascii -= 32;
                    *current_key = ascii;
                    char str[2] = { ascii, '\0' };
                    puts(str);
                    
                    break;
                }
            }
        }
        key_handle();
        return;
    }    

    void handle_input() {
        if (!(inb(status_register) & status::out_buffer_status)) return;

        uint8_t scancode = inb(0x60);

        firefly::kernel::io::legacy::writeTextSerial("scancode: 0x%X\n", scancode);
        

        if (scancode == keys::caps_lock) {
            shift_pressed = !shift_pressed;
            return;
        }

        if (scancode & 0x80) {
            if ((scancode & ~(1UL << 7)) == keys::lshift)
                shift_pressed = !shift_pressed;
        } else {
            //if(scancode != 0) printf("0x%X (0x%X) ", scancode, sc_ascii[scancode]);
            switch (scancode) {
                case keys::lshift: {
                    shift_pressed = !shift_pressed;
                    break;
                }
                case keys::enter: {
                    puts("\n");

                    stdin[stdin_index + 1] = '\0';

                    const char *arguments[2] = {"test", "123"}; 

                    int result = firefly::applications::run(stdin, 0, (char **)arguments);

                    stdin_index = 0;

                    int size = sizeof(stdin);
                    int i = 0;
        
                    while(i < size) stdin[++i] = '\0'; 

                    if(result == 0x44f9ad) printf("[ERROR] Command not found!\n");
                    
                    printf("\n> ");

                    break;
                }
                case keys::backspace: {
                    puts("\b");
                    append_cin('\b');
                    break;
                }
                default: {
                    char ascii = sc_ascii[scancode];
                    if (shift_pressed) ascii -= 32;
                    append_cin(ascii);
                    char str[2] = { ascii, '\0' };
                    puts(str);
                    
                    break;
                }
            }
        }
    }

    char *get_buffer(){ return stdin; }
}  // namespace firefly::drivers::ps2