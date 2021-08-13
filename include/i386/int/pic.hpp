#ifndef __KERNEL_PIC_HPP__
#define __KERNEL_PIC_HPP__

#include <cstdlib/cstdint.h>

namespace firefly::kernel::core::pic {

class PIC {
public:
    constexpr static inline size_t MASTER_PIC_COMMAND = 0x20;
    constexpr static inline size_t SLAVE_PIC_COMMAND = 0xA0;

    constexpr static inline size_t MASTER_PIC_DATA = 0x21;
    constexpr static inline size_t SLAVE_PIC_DATA = 0xA1;
    constexpr static inline size_t ICW1_ICW4 = 0x01;      /* ICW4 (not) needed */
    constexpr static inline size_t ICW1_SINGLE = 0x02;    /* Single (cascade) mode */
    constexpr static inline size_t ICW1_INTERVAL4 = 0x04; /* Call address interval 4 (8) */
    constexpr static inline size_t ICW1_LEVEL = 0x08;     /* Level triggered (edge) mode */
    constexpr static inline size_t ICW1_INIT = 0x10;      /* Initialization - required! */

    constexpr static inline size_t ICW4_8086 = 0x01;       /* 8086/88 (MCS-80/85) mode */
    constexpr static inline size_t ICW4_AUTO = 0x02;       /* Auto (normal) EOI */
    constexpr static inline size_t ICW4_BUF_SLAVE = 0x08;  /* Buffered mode/slave */
    constexpr static inline size_t ICW4_BUF_MASTER = 0x0C; /* Buffered mode/master */
    constexpr static inline size_t ICW4_SFNM = 0x10;       /* Special fully nested (not) */

    PIC() = default;

    // Remaps the PIC to not conflict with software interrupt codes
    void initialize(int offset1, int offset2) const noexcept;
};

}  // namespace firefly::kernel::core::pic
#endif