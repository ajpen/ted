//
// Created by Anfernee Jervis on 9/30/22.
//

#include <stdlib.h>
#include <string.h>

#include "../error/error.h"
#include "visual.h"

/*
 * Initializes a new virtual screen with the window sizes set and the buffer allocated
 * */
struct VirtualScreen* new_virtualscreen(void){

    struct VirtualScreen* screen = malloc(sizeof(struct VirtualScreen));
    set_window_size(screen);

    // double the buffer for the screen to allow escape codes to be sent without overflowing
    screen->len = screen->height * screen->width * sizeof(char) * 2;
    screen->buffer = malloc(screen->len);
    screen->buf_pos = 0;

    // the line the screen starts printing from
    screen->render_start_line = 0;

    return screen;
}

/*
 * Append the string to the vritual screen's buffer. Resizes buffer if there's
 * not enough space.
 * panics if there's any issue with allocation or when attempting to append to a
 * null buffer.
 * */
void screen_append(struct VirtualScreen *screen, const char *str, int size) {

    char * newbuf = NULL;

    if (screen->buffer == NULL){
        panic("Attempt to append to uninitialized screen\n");
    }

    // Reallocate if buffer is full
    if ((screen->len - screen->buf_pos) < size) {
        newbuf = realloc(screen->buffer, screen->len * 2);
        if (newbuf == NULL){
            panic("Can't resize screen buffer");
        }

        screen->buffer = newbuf;
    }

    memcpy(screen->buffer + screen->buf_pos, str, size);
    screen->buf_pos += size;
}


/*
 * Returns the number of screen rows required to print a line of the given length.
 * undefined for line_length < 0
 * */
int required_screen_rows(int line_length, int screen_width) {

    // lines required is the number of times the screen width is filled len/width + 1 if there's a remainder
    if (line_length == 0) {
        return 1;

    } else {
        return (line_length / screen_width) + ((line_length % screen_width) > 0);
    }
}