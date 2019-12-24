#include <stl/array.h>
#include <drivers/ps2.hpp>
#include <drivers/vga.hpp>
#include <interrupt.hpp>

extern "C" void interrupt_wrapper();

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char* VERSION_STRING = "0.0";

void write_ff_info(cursor& crs) {
    vga::clear();
    crs << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char*, 7> arr = {
        "Lime\t  ", "JohnkaS", "EyeDevelop", "4lpha", "Burokkoru ", "extation", "RedEye2D"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            crs << "\n\t";
        }
        crs << arr[i] << "  ";
    }
    crs << "\n";
}

extern "C" [[noreturn]] void kmain() {
    // passed through rcx cause it tries to access parameter by [rbx-8]
    // when it's really at [rbx+8]
    uint64_t bin_size;
    asm volatile(
        "mov %%rcx, %0"
        :
        : "m"(bin_size));

    vga::init();
    interrupt::init();
    //interrupt::test_int();
    vga::clear();

    (void)bin_size;

    cursor crs{ color::white, color::black, 0, 0 };
    write_ff_info(crs);

    for (uint64_t i = 0; i < 10; i++) {
        crs << i << " ";
    }


    while (true)
        ;
}