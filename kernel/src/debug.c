/**
 * Carbon Operating System
 * Copyright (C) 2011 Lukas Heidemann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <api/types.h>
#include <api/compiler.h>
#include <io.h>
#include <debug.h>
#include <memory.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

static uint8_t console_color_bg = COLOR_BLACK;
static uint8_t console_color_fg = COLOR_WHITE;

static size_t console_cursor_x = 0;
static size_t console_cursor_y = 0;

uint16_t *console_videomem = (uint16_t *) MEMORY_VIDEO_PADDR;

//----------------------------------------------------------------------------//
// Internal macros
//----------------------------------------------------------------------------//

#define CONSOLE_WIDTH           80
#define CONSOLE_HEIGHT          25

#define CONSOLE_ATTR(fg, bg)        ((bg << 4) | (fg & 0x0F))
#define CONSOLE_CELL(c, fg, bg)     (c | (CONSOLE_ATTR(fg, bg) << 8))

#define CONSOLE_OFFSET(x, y)        (y * CONSOLE_WIDTH + x)

//----------------------------------------------------------------------------//
// Internals
//----------------------------------------------------------------------------//

/**
 * Scrolls the console for one line, if neccessary.
 */
static void _console_scroll()
{
    // No scrolling required?
    if (LIKELY(console_cursor_y < CONSOLE_HEIGHT))
        return;
    
    // Move up the screen's contents one line
    size_t i;
    
    for (i = CONSOLE_WIDTH; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; ++i)
        console_videomem[i - CONSOLE_WIDTH] = console_videomem[i];
        
    // Clear last line
    uint16_t cell = CONSOLE_CELL(' ', console_color_fg, console_color_bg);
        
    for (i = CONSOLE_WIDTH * (CONSOLE_HEIGHT - 1); i < CONSOLE_WIDTH * CONSOLE_HEIGHT; ++i)
        console_videomem[i] = cell;
    
    // Decrease cursor's Y position
    --console_cursor_y;
}

/**
 * Updates the console's cursor position.
 */
static void _console_update_cursor()
{
    // Get cursor location
    uint16_t loc = CONSOLE_OFFSET(console_cursor_x, console_cursor_y);
    
    // Set high byte
    io_outb(0x3D4, 14);
    io_outb(0x3D5, loc >> 8);
    
    // Set low byte
    io_outb(0x3D4, 15);
    io_outb(0x3D5, loc);
}

//----------------------------------------------------------------------------//
// Printing
//----------------------------------------------------------------------------//

void console_put(char c)
{
    // Backspace
    if (UNLIKELY(c == 0x08 && console_cursor_x > 0))
    {
        --console_cursor_x;
        console_put(' ');
        --console_cursor_x;
        
        return;
    }
    
    // Tabs
    else if (UNLIKELY(c == '\t'))
    {
        size_t chars = console_cursor_x % 8;
        size_t i;
        
        for (i = 0; i < chars; ++i)
            console_put(' ');
            
        return;
    }
    
    // Newlines
    else if (UNLIKELY(c == '\n'))
    {
        console_cursor_x = 0;
        ++console_cursor_y;
    }
    
    // Everything else
    else
    {
        // Set character
        console_videomem[CONSOLE_OFFSET(console_cursor_x, console_cursor_y)] = 
            CONSOLE_CELL(c, console_color_fg, console_color_bg);
            
        ++console_cursor_x;
        
        // Newline required?
        if (console_cursor_x >= CONSOLE_WIDTH)
        {
            console_cursor_x = 0;
            ++console_cursor_y;
        }
    }
    
    // Scrolling required?
    _console_scroll();
    
    // Update hardware cursor
    _console_update_cursor();
}

void console_clear()
{
    // Clear screen
    uint16_t cell = CONSOLE_CELL(' ', console_color_fg, console_color_bg);
    size_t i;
    
    for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; ++i)
        console_videomem[i] = cell;

    // Set cursor
    console_cursor_x = console_cursor_y = 0;
    _console_update_cursor();
}

//----------------------------------------------------------------------------//
// Color
//----------------------------------------------------------------------------//

void console_set_foreground(uint8_t color)
{
    console_color_fg = color;
}

void console_set_background(uint8_t color)
{
    console_color_bg = color;
}

uint8_t console_get_foreground()
{
    return console_color_fg;
}

uint8_t console_get_background()
{
    return console_color_bg;
}

//----------------------------------------------------------------------------//
// Cursor
//----------------------------------------------------------------------------//

size_t console_get_x()
{
    return console_cursor_x;
}

size_t console_get_y()
{
    return console_cursor_y;
}

void console_set_x(size_t x)
{
    console_cursor_x = x;
}

void console_set_y(size_t y)
{
    console_cursor_y = y;
}

//----------------------------------------------------------------------------//
// Size
//----------------------------------------------------------------------------//

size_t console_get_width()
{
    return CONSOLE_WIDTH;
}

size_t console_get_height()
{
    return CONSOLE_HEIGHT;
}

//----------------------------------------------------------------------------//
// Printing
//----------------------------------------------------------------------------//

void console_print(const char *str)
{
    char *ptr;
    for (ptr = (char *) str; *ptr; ++ptr)
        console_put(*ptr);
}

void console_print_hex(uintptr_t number)
{
    // Put prefix
    console_put('0');
    console_put('x');

    // Is zero?
    if (UNLIKELY(0 == number)) {
        console_put('0');
        return;
    }
    
    // Get characters in reverse order
    int32_t i = 0;
    int8_t out[16];
    
    for (i = 15; number != 0; --i) {
        uint32_t digit = number & 0xF;
        number = number >> 4;
        
        if (digit > 9) {
            out[i] = (int8_t) ('A' + digit - 0xA);
        } else
            out[i] = (int8_t) ('0' + digit);
    }

    // Print in correct order
    for (++i; i < 16; ++i)
        console_put(out[i]);
}

void console_print_dec(intptr_t number)
{
    // Is zero?
    if (UNLIKELY(0 == number)) {
        console_put('0');
        return;
    }
    
    // Is negative?
    if (UNLIKELY(number < 0)) {
        console_put('-');
        number = -number;
    }
    
    // Get characters in reverse order
    uint32_t i = 0;
    uint8_t out[16];
    
    for (i = 15; number != 0; --i) {
        uint32_t digit = number / 10;
        number = number % 10;

        out[i] = (int8_t) ('0' + digit);
    }

    // Print in correct order
    for (++i; i < 16; ++i)
        console_put(out[i]);
}
