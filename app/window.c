//
// Created by Anfernee Jervis on 11/5/22.
//

#include "window.h"


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



Window* CreateWindow(TextBuffer* source, RenderConfig* renderConf, int rows, int cols) {
    Window* instance = malloc(sizeof(Window));

    if (instance == NULL){
        return NULL;
    }

    instance->status_line = malloc(sizeof(char) * cols);

    if (instance->status_line == NULL){
        return NULL;
    }
    instance->source_buffer = source;
    instance->render_settings = renderConf;
    instance->cols = cols;
    instance->rows = rows;
    instance->cursor_y = 0;
    instance->cursor_x = 0;
    instance->start_line = 0;
    instance->render_state = FullRender;

    return instance;
}

int WindowRenderTextBuffer(Window* window) {
    if (window == NULL){
        return -1;
    }

    int err;

    if (window->render_state == Rendered){
        return 0;
    }

    for (int i=0; i<=window->source_buffer->last_line_loc; i++){

        GapBuffer* line = window->source_buffer->lines[i];

        if (window->render_state == CascadingRender && line->modified == true){

            // Cascade renders the line and the lines below it
            window->render_state = FullRender;
        }

        // FullRender means all lines are rendered. If not set, only render if modified (partial)
        if (window->render_state == FullRender || line->modified == true){
            err = RenderGapBuffer(line, *window->render_settings);

            if (err != 0){
                return err;
            }
        }
    }

    return 0;
}

int WindowMoveCursorInView(Window* window) {
    if (window == NULL){
        return -1;
    }

    int cursor_line = window->source_buffer->cursorRow;

    if (cursor_line < window->start_line){
        window->start_line = cursor_line;
    }

    else if (cursor_line > window->end_line){
        int lines_req = 0;
        int end_line = window->end_line + 1;

        // shift end of frame down
        while (end_line <= cursor_line){
            lines_req += required_screen_rows(
                    window->source_buffer->lines[end_line]->rendered_len,
                    window->cols);

            end_line++;
        }

        window->end_line = cursor_line;

        // shift start of frame down
        while(lines_req > 0){
            lines_req -= required_screen_rows(
                    window->source_buffer->lines[window->start_line]->rendered_len, window->cols);

            window->start_line++;
        }
    }

    return 0;
}

void WindowReset(Window* window) {

    if (window != NULL) {

        // Reset cursor
        TextBufferMoveCursor(window->source_buffer, 0, 0);
        window->cursor_x = 1;
        window->cursor_y = 1;


        // re-render
        window->render_state = FullRender;
        WindowRenderTextBuffer(window);

        // Reset window display
        window->start_line = 0;

        int cur_row = 0;
        for (int i=0; i<=window->source_buffer->last_line_loc; i++){

            cur_row += required_screen_rows(window->source_buffer->lines[i]->rendered_len, window->cols);

            if (cur_row >= window->rows){
                break;
            }

            window->end_line = i;
        }
    }
}

