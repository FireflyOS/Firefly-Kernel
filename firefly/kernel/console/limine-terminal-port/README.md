# Port of the Limine bootloader terminal

Normally this terminal is provided by the Limine bootloader and can (and should) be used by the kernel during early boot.

It's an extremely fast with a complete "terminfo"/vt100 implementation. There really isn't a reason not to use this terminal.

The only issue here is that it's merely an *early boot console*, and it's located in conventional, lower-half memory.

Once you get to userspace you'll find it very annoying to try and map memory *around* the it considering that terminal shouldn't be in lower-half memory in the first place!

That's what this port is for.
You should be able to include it into your kernel and use it just fine.

Please let us know if any issues arise, thank you!

## Features
* Every feature that Limine terminal has is supported
* Multiple terminals support

## Usage

1. First off, choose a font from fonts/ folder or create your own and load it in your os (link it directly to the kernel, load it from filesystem, as a module, etc)

2. To initialize the terminal, include `term.h` and provide some basic functions declared in the header file (Example shown below)

3. Create new term_t and run `term_init(term, arguments);` (If you set bios to false, you will not be able to use text mode)

4. To use vbe mode with framebuffer, run `term_vbe(term, arguments);` (Example shown below)

5. To use text mode, run `term_textmode(term);`

Note: There also are C++ wrappers for term_t and image_t structures (cppterm_t and cppimage_t) in `source/cpp/` directory

## Example
```c
#include "term.h"

void *alloc_mem(size_t size)
{
   // Allocate memory
   // memset() memory to zero
}
void free_mem(void *ptr, size_t size)
{
   // Free memory
}
void *memcpy(void *dest, const void *src, size_t len)
{
   // Memcpy
}
void *memset(void *dest, int ch, size_t n)
{
   // Memset
}

void callback(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)
{
   handleCallback();
}

struct framebuffer_t frm = {
   address, // Framebuffer address
   width, // Framebuffer width
   height, // Framebuffer height
   pitch // Framebuffer pitch
};

struct font_t font = {
   font_address, // Address of font file
   8, // Font width
   16, // Font height
   1, // Character spacing
   0, // Font scaling x
   0 // Font scaling y
};

struct style_t style = {
   DEFAULT_ANSI_COLOURS, // Default terminal palette
   DEFAULT_ANSI_BRIGHT_COLOURS, // Default terminal bright palette
   0xA0000000, // Background colour
   0xFFFFFF, // Foreground colour
   64, // Terminal margin
   0 // Terminal margin gradient
};

struct image_t image;
image_open(&image, bmpBackgroundAddress, size);
struct background_t back = {
   &image, // Background. Set to NULL to disable background
   STRETCHED, // STRETCHED, CENTERED or TILED
   0x00000000 // Terminal backdrop colour
};

struct term_t term;
term_init(&term, callback, bootedInBiosMode);

// VBE mode
// In VBE mode you can create more terminals for different framebuffers
term_vbe(&term, frm, font, style, back);
term_print(&term, "Hello, World!");

// Text mode
term_textmode(&term);
term_print(&term, "Hello, World!");
```

Based on: https://github.com/limine-bootloader/limine
