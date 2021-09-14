#include <stl/array.h>
#include <stl/cstdlib/stdio.h>
#include <x86_64/libk++/iostream.h>

#include <x86_64/drivers/ps2.hpp>
#include <x86_64/drivers/serial.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

#include <x86_64/drivers/vbe.hpp>
#include <x86_64/kernel.hpp>
#include <x86_64/trace/strace.hpp>

#include <x86_64/memory-manager/virtual_memory.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0-x86_64-fork";

namespace firefly::kernel::main {

void write_ff_info() {
    printf("Firefly\nVersion: %s\nContributors:", VERSION_STRING);

    firefly::std::array<const char *, 4> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL\t  ", "SergeyMC9730"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % (arr.max_size() - 1) == 0) {
            puts("\n\t");
        }
        printf("%s  ", arr[i]);
    }
    puts("\n");
}

void init_keyboard(){
    printf("Initialization a Keyboard...\n");

    bool isKeyboard = firefly::drivers::ps2::init();
    io::legacy::writeTextSerial("Keyboard Driver returned ");
    io::legacy::writeTextSerial((isKeyboard) ? "true\n" : "false\n");
    
    if(!isKeyboard) trace::panic(trace::PM_DRIVERERROR_PK, trace::PC_DRIVERERROR_PK);
}

void init_serial(){
    printf("Initialization a Serial Port...\n");
    
    bool isSerial = io::legacy::serial_port_init();

    if(!isSerial) trace::panic(trace::PM_DRIVERERROR_S, trace::PC_DRIVERERROR_S);
}

void kernel_main() {
    write_ff_info();
    
    init_serial();
    init_keyboard();

    static uint8_t *virtual_mem0 = virtual_memory::create_virtual_memory();

    printf("Virtual Memory Address: 0x%X\n", virtual_mem0);

    uint8_t temp_mem0[8] = {0xe0, 0x0e, 0xff, 0xfd, 0x01, 0x02, 0x03, 0x04};
    virtual_memory::multiple_set(virtual_mem0, temp_mem0, 64);

    printf("uint8_t  virtual_mem0[64] (0x%X) = 0x%X\n", virtual_memory::get_memory_location(virtual_mem0, 64), virtual_mem0[64]);
    printf("uint8_t  virtual_mem0[65] (0x%X) = 0x%X\n", virtual_memory::get_memory_location(virtual_mem0, 65), virtual_mem0[65]);

    printf("uint16_t virtual_mem0[64]        = 0x%X\n\n", virtual_memory::get_16bit_number(virtual_mem0, 64));
    printf("uint32_t virtual_mem0[64]        = 0x%X\n\n", virtual_memory::get_32bit_number(virtual_mem0, 64));

    trace::panic(trace::PM_MANUALLYCRASHED, trace::PC_MANUALLYCRASHED);
}
}  // namespace firefly::kernel::main