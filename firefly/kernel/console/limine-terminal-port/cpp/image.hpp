#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "../image.h"

struct cppimage_t : image_t
{
    void make_centered(int frame_x_size, int frame_y_size, uint32_t back_colour)
    {
        image_make_centered(this, frame_x_size, frame_y_size, back_colour);
    }
    void make_stretched(int new_x_size, int new_y_size)
    {
        image_make_stretched(this, new_x_size, new_y_size);
    }
    bool open(uint64_t file, uint64_t size)
    {
        return image_open(this, file, size);
    }
    void close()
    {
        image_close(this);
    }
};

#endif // IMAGE_HPP