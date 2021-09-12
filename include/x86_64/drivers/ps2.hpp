#pragma once
#include <stl/array.h>
#include <x86_64/drivers/ports.hpp>
#include <stl/optional.h>
#include <x86_64/utils.hpp>

/**
 *                          The PS/2 port driver
 */
namespace firefly::drivers::ps2 {
    /**
     *                      Initializes the driver
     * @return true :       Driver was successfully initialized
     *         false :      Driver failed to initialize
     */
    bool init();

    /**
     *                      Tries to read a scancode from the keybaord
     * 
     * @return              scancode if input was readable
     */
    firefly::std::optional<unsigned char> get_scancode();

    /**
     *                      Handles input of a scancode and forwards it to the display buffer
     * 
     * @param scancode      Scancode that was receieved from the keyboard
     * @param crs           Where to print the input
     */
    void handle_input(unsigned char scancode);
}  // namespace firefly::drivers::ps2