#include <i386/int/pic.hpp>
#include <i386/drivers/ports.hpp>

namespace firefly::kernel::core::pic {
    void PIC::initialize(int offset1, int offset2) const noexcept {
        unsigned char master_mask;
        unsigned char slave_mask;

        master_mask = io::inb(MASTER_PIC_DATA);
        slave_mask = io::inb(SLAVE_PIC_DATA);

        io::outb(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
        io::io_wait();
        io::outb(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
        io::io_wait();
        io::outb(MASTER_PIC_DATA, offset1);
        io::io_wait();
        io::outb(SLAVE_PIC_DATA, offset2);
        io::io_wait();
        io::outb(MASTER_PIC_DATA, 0x4); // there's a slave at IRQ2 (0000 0100)
        io::io_wait();
        io::outb(SLAVE_PIC_DATA, 0x2); // Tell slave PIC its cascade identity
        io::io_wait();

        io::outb(MASTER_PIC_DATA, ICW4_8086);
        io::io_wait();
        io::outb(SLAVE_PIC_DATA, ICW4_8086);
        io::io_wait();

        io::outb(MASTER_PIC_DATA, master_mask);
        io::outb(SLAVE_PIC_DATA, slave_mask);
    }

}  // namespace firefly::kernel::core::pic