#include <x86_64/drivers/ps2mouse.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

namespace firefly::kernel::io::mouse {
    namespace variables {
        uint8_t cycle = 0;
        int8_t buffer[3];
        int8_t x = 0;
        int8_t y = 0;
    }
    void handle([[maybe_unused]] iframe *intframe){
        firefly::kernel::io::legacy::writeTextSerial("catcha 1\n");
        switch(variables::cycle){
            case 0: {
                variables::buffer[0] = inb(PS2MOUSE_PORT);
                variables::cycle++;
                break;
            }
            case 1: {
                variables::buffer[1] = inb(PS2MOUSE_PORT);
                variables::cycle++;
                break;
            }
            case 2: {
                variables::buffer[2] = inb(PS2MOUSE_PORT);
                variables::x = variables::buffer[1];
                variables::x = variables::buffer[2];
                variables::cycle = 0;
                break;
            }
        }
    }
    void wait(uint8_t is_write){
        int timeout = 100000;
        switch(is_write) {
            case 0: {
                while(timeout--) if((inb(PS2MOUSE_PORT + 4) & 1) == 1) return;
            }
            case 1: {
                while(timeout--) if((inb(PS2MOUSE_PORT + 4) & 2) == 0) return;
            }
        }
    }
    void write(uint8_t byte){
        wait(1);
        outb(PS2MOUSE_PORT + 4, 0xD4);
        wait(1);
        outb(PS2MOUSE_PORT, byte);
    }
    uint8_t read(){
        wait(0);
        return inb(PS2MOUSE_PORT);
    }
    void init(){
        firefly::kernel::io::legacy::writeTextSerial("catcha 2\n");
        uint8_t status;

        wait(1);
        outb(PS2MOUSE_PORT + 4, 0xA8);
        wait(1);
        outb(PS2MOUSE_PORT + 4, 0x20);
        wait(0);
        status = inb(PS2MOUSE_PORT) | 2;
        wait(1);
        outb(PS2MOUSE_PORT, status);

        write(0xF4);
        read();

        firefly::kernel::core::interrupt::change::update(&handle, 0x08, 0x8E, 13);
    }
}  // namespace firefly::kernel::io::legacy