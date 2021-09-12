#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/cstring.h>

#include <x86_64/drivers/ports.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

//I made a legacy serial port driver because original serial.cpp 
//requires a support of "new" operator and it requires a malloc function

#define PORT 0x3f8

namespace firefly::kernel::io::legacy {
    bool serial_port_init() {
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
        return inb(PORT + 5) & 1;
    }
    char readSerial() {
        while (isGotSignal() == 0);
        return inb(PORT);
}
    int isTransmitEmpty() {
        return inb(PORT + 5) & 0x20;
    }
    void writeCharSerial(char a) {
        while (isTransmitEmpty() == 0);
        outb(PORT, a);
    }
    void writeSerial(const char* data, size_t size) {
        for (size_t i = 0; i < size; i++) writeCharSerial(data[i]);
    }
    void writeTextSerial(const char* data) {
        writeSerial(data, strlen(data));
    }
}  // namespace firefly::kernel::io