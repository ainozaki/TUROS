#include "print.h"

#include "font.h"
#include "pixel.h"

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;

void putc(char c) {
    int x, y;
    if (c == '\r') {
        cursor_x = 0;
        return;
    } else if (c == '\n') {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
        if (cursor_y >= fb->vr) {
            cursor_y = 0;
            set_default_screen();
        }
        return;
    }

    // Print character
    for (y = 0; y < FONT_HEIGHT; y++) {
        for (x = 0; x < FONT_WIDTH; x++) {
            if (font_bitmap[(unsigned int) c][y][x]) {
                draw_px(cursor_x + x, cursor_y + y, /*red=*/0, /*green=*/0,
                        /*blue=*/0);
            }
        }
    }
    // Update cursor.
    cursor_x += FONT_WIDTH;
    if (cursor_x >= fb->hr) {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
        if (cursor_y >= fb->vr) {
            cursor_y = 0;
            set_default_screen();
        }
    }
}

void puts(char *s) {
    while (*s != '\0') {
        putc(*s);
        s++;
    }
}