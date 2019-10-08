#pragma once
#include <stl/algorithm.h>
#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/cstring.h>

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
     *                      Background color to print
     */
    color bg;

    /**
     *                      Foreground color to print
     */
    color fg;

    /**
     *                      X-coordinate to print at
     */
    size_t x;

    /**
     *                      Y-coordinate to print at
     */
    size_t y;

    /**
     *                      Creates a VGA character for printing
     * @param c             The ASCII character to print
     * @return              a VGA character to print
     */
    vga_char character(char c);
};

/**
 *                          Character width of default mode
 */
constexpr static size_t width = 80;
/**
 *                          Character height of default mode
 */
constexpr static size_t height = 15;
/**
 *                          Address of VGA character buffer
 */
constexpr static size_t display_buff_addr = 0xB8000;
vga_char* const display_buffer = reinterpret_cast<vga_char*>(display_buff_addr);

/**
 *                          A driver for displaying text
 */
struct Display {
    bool _visual_cursor = false;

    /**
     * Constructor, enables cursor
     */
    Display();

    /**
     *                      Default cursor for the display
     */
    cursor crs{ color::black, color::white, 0, 0 };

    // exists only for testing
    // ~Display() {
    //     clear();
    // }

    /**
     *                      Prints an ASCII string
     * @param arr           The null-terminated string to print
     * @return              This Display
     */
    Display& operator<<(const char* arr);

    /**
     *                      Prints an ASCII character
     * @param c             The character to print
     * @return              This Display
     */
    Display& operator<<(char c);

    // Display& operator<<(int);

    /**
     *                      Clears the entire screen
     */
    void clear();

    /**
     *                      Handles backspace characters
     */
    void backspace();

    /**
     *                      Gets the length of a VGA character string
     * @param str           pointer to null-terminated VGA string
     * @return              length of the string
     */
    [[nodiscard]] static size_t vgalen(const vga_char* str);

    /**
     *                      Keeps the cursor coordinates in bounds
     *                      and scrolls the screen
     */
    void handle_write_pos();


    void update_cursor(size_t x, size_t y);

    void toggle_cursor(bool on);
    /**
     *                      Deals with printing escape characters
     * @param c             The character to handle
     * @return true  :      c was an escape character
     *         false :      c was not an escape character 
     */
    bool handle_special_characters(const char c);

    /**
     *                      Prints a VGA character to the screen
     *                      at the default cursor
     * @param c             The character to print
     * @param _update       Whether or not to update cursor
     */
    void write(const vga_char c, bool _update=true);

    /**
     *                      Prints a VGA string to the screen
     *                      at the defautl cursor
     * @param str           Pointer to the null-terminated VGA string
     */
    void write(const vga_char* str);

    /**
     *                      Prints an ASCII character
     * @param c             The character to print
     * @param _update       Whether or not to update cursor
     */
    void write(const char c, bool _update=true);

    /**
     *                      Prints an ASCII string
     * @param arr           The null-terminated string to print
     */
    void write(const char* str);

    /**
     *                      Sets the default background color
     * @param c             The color to set
     */
    void set_background_color(color c);

    /**
     *                      Sets the default foreground color
     * @param c             The color to set
     */
    void set_foreground_color(color c);
};