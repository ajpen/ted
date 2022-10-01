//
// Created by Anfernee Jervis on 9/30/22.
//

#ifndef TED_VISUAL_H
#define TED_VISUAL_H

#include "buffer.h"

typedef struct Cursor {
    int x;
    int y;
} Cursor;


struct VirtualScreen {
    char* buffer;           // Contents on the screen (text & escape codes)
    int buf_pos;
    int len;
    Cursor cursor;
    int width;
    int height;
    int render_start_line;
};



struct VirtualScreen* new_virtualscreen(void);
void screen_append(struct VirtualScreen *screen, const char *str, int size);
int required_screen_rows(int line_length, int screen_width);
void move_cursor_in_view(TextBuffer* buffer, struct VirtualScreen* screen);
void draw_editor_window(TextBuffer* buffer, struct VirtualScreen* screen);
void set_virtual_cursor_position(TextBuffer* buffer, struct VirtualScreen* screen);
void set_window_size(struct VirtualScreen *screen);

#endif //TED_VISUAL_H
