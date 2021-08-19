#ifndef __SERIAL_KERNEL_HPP__
#define __SERIAL_KERNEL_HPP__

#include <cstdlib/cstdint.h>

namespace firefly::kernel::io {
class SerialPort {
public:
    constexpr static inline size_t BAUD_BASE = 115200;
    constexpr static inline uint16_t COM1 = 0x3F8;
    constexpr static inline uint16_t COM2 = 0x2F8;
    constexpr static inline uint16_t COM3 = 0x3E8;
    constexpr static inline uint16_t COM4 = 0x2E8;

private:
    uint16_t port;
    uint16_t baud_divisor;

    bool initialized = false;

public:
    SerialPort(uint16_t port, uint32_t baud_rate);

    bool initialize() noexcept;
    bool is_initialized() const noexcept;


    bool received() const noexcept;
    char read_char() const noexcept;

    void send_char(char c) noexcept;
    bool is_transmit_empty() const noexcept;

    void send_chars(const char* c, int len = -1) noexcept;
    void read_string(char* buffer, int len) const noexcept;
};

}  // namespace firefly::kernel::io

#endif