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
     */
    void handle_input();

    /**
     * Returns keyboard buffer
     * 
     * @return Keyboard buffer (char *)
     */
    char *get_buffer();

    /**
     * Enables keyboard redirecting
     * 
     * @param keyboard_handle function pointer to keyboard_handle trigger
     * @param current_key pointer to current_key variable
     * 
     * @return void
     */
    void redirect_to_app(void(*keyboard_handle)(void), uint8_t *currentkey);

    /**
     * Get keyboard input with redirecting
     * 
     * @return void
     */
    void app_input();

}  // namespace firefly::drivers::ps2