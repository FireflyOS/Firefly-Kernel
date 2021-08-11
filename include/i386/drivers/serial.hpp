#ifndef __SERIAL_KERNEL_HPP__
#define __SERIAL_KERNEL_HPP

#include <cstdint.h>

namespace firefly::kernel::io {
    class SerialPort {
    constexpr static inline size_t BAUD_BASE = 
    private:
        uint16_t port;
        uint32_t baud_divisor;
    
    public:
        SerialPort(uint16_t port) : port(port) {
        }
    };

}

#endif