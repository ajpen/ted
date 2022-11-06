//
// Created by Anfernee Jervis on 11/5/22.
//

#include "window.h"

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


