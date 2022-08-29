#ifndef TERM_HPP
#define TERM_HPP

#include "../term.h"
#include "image.hpp"

struct cppterm_t : term_t
{
    void init(callback_t callback, bool bios)
    {
        term_init(this, callback, bios);
    }

    void reinit()
    {
        term_reinit(this);
    }
    void deinit()
    {
        term_deinit(this);
    }
    void vbe(framebuffer_t frm, font_t font, style_t style = style_t { DEFAULT_ANSI_COLOURS, DEFAULT_ANSI_BRIGHT_COLOURS, DEFAULT_BACKGROUND, DEFAULT_FOREGROUND, DEFAULT_MARGIN, DEFAULT_MARGIN_GRADIENT }, background_t back = background_t { nullptr, TILED, 0x000000 })
    {
        term_vbe(this, frm, font, style, back);
    }
#if defined(__i386__) || defined(__x86_64__)
    void textmode()
    {
        term_textmode(this);
    }
#endif
    void notready()
    {
        term_notready(this);
    }
    void putchar(uint8_t c)
    {
        term_putchar(this, c);
    }
    void write(uint64_t buf, uint64_t count)
    {
        term_write(this, buf, count);
    }
    void print(const char *str)
    {
        term_print(this, str);
    }

    void raw_putchar(uint8_t c)
    {
        term_raw_putchar(this, c);
    }
    void clear(bool move)
    {
        term_clear(this, move);
    }
    void enable_cursor()
    {
        term_enable_cursor(this);
    }
    bool disable_cursor()
    {
        return term_disable_cursor(this);
    }
    void set_cursor_pos(size_t x, size_t y)
    {
        term_set_cursor_pos(this, x, y);
    }
    void get_cursor_pos(size_t &x, size_t &y)
    {
        term_get_cursor_pos(this, &x, &y);
    }
    void set_text_fg(size_t fg)
    {
        term_set_text_fg(this, fg);
    }
    void set_text_bg(size_t bg)
    {
        term_set_text_bg(this, bg);
    }
    void set_text_fg_bright(size_t fg)
    {
        term_set_text_fg_bright(this, fg);
    }
    void set_text_bg_bright(size_t bg)
    {
        term_set_text_bg_bright(this, bg);
    }
    void set_text_fg_default()
    {
        term_set_text_fg_default(this);
    }
    void set_text_bg_default()
    {
        term_set_text_bg_default(this);
    }
    bool scroll_disable()
    {
        return term_scroll_disable(this);
    }
    void scroll_enable()
    {
        term_scroll_enable(this);
    }
    void move_character(size_t new_x, size_t new_y, size_t old_x, size_t old_y)
    {
        term_move_character(this, new_x, new_y, old_x, old_y);
    }
    void scroll()
    {
        term_scroll(this);
    }
    void revscroll()
    {
        term_revscroll(this);
    }
    void swap_palette()
    {
        term_swap_palette(this);
    }
    void save_state()
    {
        term_save_state(this);
    }
    void restore_state()
    {
        term_restore_state(this);
    }

    void double_buffer_flush()
    {
        term_double_buffer_flush(this);
    }

    uint64_t context_size()
    {
        return term_context_size(this);
    }
    void context_save(uint64_t ptr)
    {
        term_context_save(this, ptr);
    }
    void context_restore(uint64_t ptr)
    {
        term_context_restore(this, ptr);
    }
    void full_refresh()
    {
        term_full_refresh(this);
    }
};

#endif // TERM_HPP