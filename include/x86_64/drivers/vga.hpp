#pragma once
#include <stl/algorithm.h>
#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/cstring.h>

namespace firefly::drivers::vga {
/**
 *                          A VGA color
 */
enum class color : uint8_t {
    black = 0x00,
    blue = 0x01,
    green = 0x02,
    cyan = 0x03,
    red = 0x04,
    magenta = 0x05,
    brown = 0x06,
    lightgrey = 0x07,
    darkgrey = 0x08,
    lightblue = 0x09,
    lightgreen = 0x0A,
    lightcyan = 0x0B,
    lightred = 0x0C,
    lightmagenta = 0x0D,
    lightbrown = 0x0E,
    white = 0x0F,
};

/**
 *                          A VGA character
 */
struct __attribute__((packed)) vga_char {
    /**
     *                      The ASCII codepoint
     */
    char codepoint;
    /**
     *                      The character's color
     */
    color fg : 4;
    /**
     *                      The character's background color
     */
    color bg : 4;
};

static_assert(2 == sizeof(vga_char), "vga_char size incorrect");

/**
 *                          A cursor for printing to the screen
 */
struct cursor {
    /**
    *                      Foreground color to print
    */
    color fg;

    /**
     *                      Background color to print
     */
    color bg;

    /**
      *   X-coordinate to print at
      */
    size_t x;

    /**
      *   Y-coordinate to print at
      */
    size_t y;

    // I think the window manager should keep the min_ and max_ bounds
    // because that sounds more like the window's problem
    // and it should take care of its own cursor

    /**
     *                      Creates a VGA character for printing
     * @param c             The ASCII character to print
     * @return              a VGA character to print
     */
    vga_char character(char c);

    /**
     *                      Prints an ASCII string
     * @param arr           The null-terminated string to print
     * @return              This cursor
     */
    cursor& operator<<(const char* arr);

    /**
     *                      Prints an ASCII character
     * @param c             The character to print
     * @return              This cursor
     */
    cursor& operator<<(char c);


    /**
     *                      Prints an integer
     * @param n             The integer to print
     * @return cursor&      This cursor 
     */
    cursor& operator<<(int n);

    /**
     *                      Sets the default foreground color
     * @param c             The color to set
     */
    void set_foreground_color(color c);

    /**
     *                      Sets the default background color
     * @param c             The color to set
     */
    void set_background_color(color c);

    /**
     *                      Keeps the cursor in bounds
     *                      and scrolls the screen
     */
    void handle_bounds();
};


/**
 *                          Character width of default mode
 */
constexpr static size_t width = 80;
/**
 *                          Character height of default mode
 */
constexpr static size_t height = 25;
/**
 *                          Address of VGA character buffer
 */
constexpr static size_t display_buff_addr = 0xFFFFFFFF800B8000;
vga_char* const display_buffer = reinterpret_cast<vga_char*>(display_buff_addr);

static bool _visual_cursor = false;

/**
 *                          The VGA driver
 */
/**
     *                      Initializes the driver
     * @return true  :       Driver was successfully initialized
     */
bool init();

/**
     *                      Clears the entire screen
     */
// maybe not expose this and make it exclusive to window managers
// so they can clear only their own window?
// not sure how that'd work, either move the function to the wm
// class or make this take a wm reference
void clear();

/**
     *                      Gets the length of a VGA character string
     * @param str           pointer to null-terminated VGA string
     * @return              length of the string
     */
// I don't see this being necessary cause it exposes internals
// but idk, maybe we'll want to preserve color info along with the string
//[[nodiscard]] size_t vgalen(const vga_char* str);

/**
     *                      Deals with printing escape characters
     * @param c             The character to handle
     * @return true  :      c was an escape character
     *         false :      c was not an escape character 
     */
bool handle_special_characters(const char c, cursor& crs);

/**
     *                      Prints a VGA character to the screen
     *                      at the default cursor
     * @param c             The character to print
     * @param _update       Whether or not to update cursor
     */
void write(const vga_char c, cursor& crs, bool _update = true);

/**
     *                      Prints a VGA string to the screen
     *                      at the defautl cursor
     * @param str           Pointer to the null-terminated VGA string
     */
void write(const vga_char* str, cursor& crs);

/** 
     *                      Prints an ASCII character
     * @param c             The character to print
     * @param _update       Whether or not to update cursor
     */
void write(const char c, cursor& crs, bool _update = true);

/**
     *                      Prints an ASCII string
     * @param arr           The null-terminated string to print
     */
void write(const char* str, cursor& crs);

/**
     *                      Enables the hardware cursor
     */
void enable_hw_cursor();

/**
     *                      Disables the hardware cursor
     */
void disable_hw_cursor();

/**
     *                      Update the position of the hardware cursor
     * @param x             The new x-coordinate
     * @param y             The new y-coordinate
     */
void update_hw_cursor(size_t x, size_t y);
}  // namespace firefly::drivers::vga