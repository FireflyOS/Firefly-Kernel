#pragma once

#include <stddef.h>
#include <stdint.h>

namespace firefly::kernel::device::fb {
void init();
void scroll();
void set_font(uint8_t* fnt, int size, int fnt_w, int fnt_h, int color);
void putc(char c, int x, int y);
void putc(char c, int x, int y, int color);
void putc(char c);
void puts(const char* str);
void boot_splash();
}  // namespace firefly::kernel::device::fb
