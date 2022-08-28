//
// Created by Anfernee Jervis on 8/27/22.
//

// ANSI Escape Codes
#define ESC '\x1b'
#define INVERT_COLOUR "\x1b[7m"
#define INVERT_COLOUR_SIZE 4
#define RESET_STYLE_COLOUR "\x1b[0m"


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

/*
 * Returns the number of screen rows required to print a line of the given length.
 * */
int required_screen_rows(int line_length, int screen_width){

    // lines required is the number of times the screen width is filled len/width + 1 if there's a remainder
    return (line_length / screen_width) + ((line_length % screen_width) > 0);
}


/*
 * TODO: I can retroactively calculate this in the logic in the function that moves the cursor.
 * TODO: If i want to jump to file lines, I can then just call the function that moves the cursor x times
 *
 * TODO: After the cursor is moved to a new line, I can calculate what position the render start line needs
 * to be to keep the cursor visible. If cursor is above it, shift upwards. If below, shift downwards, ensuring enough
 * space for the cursor to be visible.
 *
 * Determines whether the cursor is off the screen, if so, shifts the display part of the buffer until
 * the cursor is back in view.
 * */
void move_cursor_in_view(TextBuffer* buffer, struct VirtualScreen* screen){
    Cursor cursor = {buffer->cursorRow, buffer->cursorCol};

    int cumul_req_rows = 0;
    int cur_line_required_rows;
    int cur_line = screen->render_start_line;


    // The entire render_start_line is guaranteed to be rendered (except the corner case where
    // its larger than the screen dimensions; a case we'll ignore for now)
    if (cursor.x < screen->render_start_line){
        screen->render_start_line = cursor.x;
    }

    // We'll calculate the current range in the buffer that is visible
    // TODO: This can probably be cached
    else {
        while (cur_line <= buffer->last_line_loc) {

            cur_line_required_rows = required_screen_rows(buffer->lines[cur_line]->str_len, screen->width);

            if ((cur_line_required_rows + cumul_req_rows) > screen->height - 1) {
                break;
            }

            cumul_req_rows += required_screen_rows(buffer->lines[cur_line]->str_len, screen->width);
            cur_line++;
        }

        // If the cursor is not in view, shift the text displayed until it is
        if (cursor.x > cur_line) {

            int rows_required = 0;

            // Calculate required space
            while(cur_line <= cursor.x){
                rows_required += required_screen_rows(buffer->lines[cur_line]->str_len, screen->width);
                cur_line++;
            }


            // shift the render start line down until we've made enough room
            while (rows_required > 0){
                rows_required -= required_screen_rows(buffer->lines[screen->render_start_line]->str_len, screen->width);
                screen->render_start_line++;
            }
        }
    }
}


