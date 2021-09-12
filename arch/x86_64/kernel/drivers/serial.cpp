#include <cstdlib/cstring.h>

#include <x86_64/drivers/ports.hpp>
#include <x86_64/drivers/serial.hpp>
 
namespace firefly::kernel::io {
SerialPort::SerialPort(uint16_t port, uint32_t baud_rate)
    : port(port), baud_divisor{ static_cast<uint16_t>(BAUD_BASE / baud_rate) } {
}

bool SerialPort::is_initialized() const noexcept {
    return initialized;
}
bool SerialPort::received() const noexcept {
    return inb(port + 5) & 1;
}
char SerialPort::read_char() const noexcept {
    return inb(port);
}

bool SerialPort::is_transmit_empty() const noexcept {
    return inb(port + 5) & 0x20;
}

void SerialPort::send_char(char c) noexcept {
    outb(port, c);
}

bool SerialPort::initialize() noexcept {
    if (initialized) {
        return false;
    }

    outb(port + 1, 0);  // Disable interrupts
    outb(port + 3, 0x80);
    // divisor
    auto low_byte = (baud_divisor & 0xFF);
    auto high_byte = (baud_divisor >> 8) & 0xFF;
    outb(port, low_byte);
    outb(port + 1, high_byte);
    // Set 8 bits, no parity, one stop bit
    outb(port + 3, 0x03);
    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 2, 0xC7);
    // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x0B);
    outb(port + 4, 0x1E);
    // Let's test a byte
    outb(port, 0xAE);

    if (inb(port) != 0xAE) {
        return true;
    }
    initialized = true;
    return false;
}

void SerialPort::send_chars(const char* c, int len) noexcept {
    if (len == -1) {
        len = strlen(c);
    }

    for (int i = 0; i < len; ++i) {
        send_char(c[i]);
    }
    send_char('\0');
}

void SerialPort::read_string(char* buffer, int len) const noexcept {
    char current_char = '1'; // anything that's not '\0'
    int read_count = 0;
    while (read_count < len && ((!current_char) == '\0')) {
        current_char = read_char();
        buffer[read_count++] = current_char;
        len--;
    }
    buffer[read_count] = '\0';
}

}  // namespace firefly::kernel::io