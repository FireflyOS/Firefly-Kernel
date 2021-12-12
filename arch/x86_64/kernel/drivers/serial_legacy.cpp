#include <stdarg.h>
#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/ports.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

#include <x86_64/settings.hpp>

//I made a legacy serial port driver because original serial.cpp
//requires a support of "new" operator and it requires a malloc function

#define PORT 0x3f8

namespace firefly::kernel::io::legacy {
bool serial_port_init() {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return false;

    outb(PORT + 1, 0x00);  // Disable all interrupts
    outb(PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);  //                  (hi byte)
    outb(PORT + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xc7);  // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0b);  // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1e);  // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xae);  // Test serial chip (send byte 0xAE and check if serial returns same byte)

    if (inb(PORT + 0) != 0xae) return false;

    outb(PORT + 4, 0x0f);
    return true;
}
int isGotSignal() {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return -1;

    return inb(PORT + 5) & 1;
}
char readSerial() {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return 0xff;

    while (isGotSignal() == 0)
        ;
    return inb(PORT);
}
int isTransmitEmpty() {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return -1;

    return inb(PORT + 5) & 0x20;
}
void writeCharSerial(char a) {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return;

    while (isTransmitEmpty() == 0)
        ;
    outb(PORT, a);

    return;
}
void writeSerial(const char* data, size_t size, bool istoupper) {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return;

    for (size_t i = 0; i < size; i++) writeCharSerial((!istoupper) ? data[i] : toupper(data[i]));

    return;
}
int writeTextSerial(const char* fmt, ...) {
    if(firefly::kernel::settings::kernel_settings[1] == 0x00) return -1;

    va_list ap;
    va_start(ap, fmt);
    int i = 0;
    int len = strlen(fmt);
    int res = 0;

    for (; i < len; i++) {
        switch (fmt[i]) {
            case '%': {
                switch (fmt[i + 1]) {
                    case 'c': {
                        char arg = va_arg(ap, int);
                        writeCharSerial(arg);
                        i += 2, ++res;
                        break;
                    }

                    case 's': {
                        char* arg = va_arg(ap, char*);
                        writeSerial(arg, strlen(arg), false);
                        i += 2, (res += 2 + strlen(arg));
                        break;
                    }

                    case 'i':
                    case 'd': {
                        size_t arg = va_arg(ap, size_t);
                        if (arg == 0)
                            writeCharSerial('0');

                        else {
                            char buff[20];
                            writeSerial(itoa(arg, buff, 10), strlen(itoa(arg, buff, 10)), false);
                            res += digitcount(arg);
                        }
                        i += 2;
                        break;
                    }

                    case 'x': {
                        size_t arg = va_arg(ap, size_t);
                        char buff[20];
                        writeSerial(itoa(arg, buff, 16), strlen(itoa(arg, buff, 16)), false);
                        res += strlen(buff);
                        i += 2;
                        break;
                    }
                    case 'X': {
                        size_t arg = va_arg(ap, size_t);
                        char buff[20];
                        writeSerial(itoa(arg, buff, 16), strlen(itoa(arg, buff, 16)), true);
                        res += strlen(buff);
                        i += 2;
                        break;
                    }

                    case 'o': {
                        size_t arg = va_arg(ap, size_t);
                        char buff[20];
                        writeSerial(itoa(arg, buff, 8), strlen(itoa(arg, buff, 8)), false);
                        i += 2;
                        break;
                    }
                    default:
                        va_end(ap);
                        break;
                }
            }
            default:
                writeCharSerial(fmt[i]);
                va_end(ap);
                res++;
                break;
        }
    }
    return 0;
}
}  // namespace firefly::kernel::io::legacy