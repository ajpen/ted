//
// Created by Anfernee Jervis on 11/4/22.
//



#include <stdio.h>
#include "render.h"

int RenderGapBuffer(GapBuffer* buffer, RenderConfig* config) {
    if (buffer->rendered == NULL){
        buffer->rendered = malloc(sizeof(char) * buffer->str_len * 2);

        if (buffer->rendered == NULL){
            return MEM_ERROR;
        }

        buffer->rendered_len = buffer->str_len * 2;
    }

    int pos = 0;

    for (int i=0; i < buffer->str_len; i++){

        // resize if necessary
        if (pos == buffer->rendered_len){
            char* new_buf = realloc(buffer->rendered, buffer->rendered_len * 2);

            if (new_buf == NULL){
                return MEM_ERROR;
            }

        }

        if (GapBufferCharAt(buffer, i) == '\t'){
            sprintf(&(buffer->rendered[pos]), "%*s", config->tabsize, "");
            pos += config->tabsize;
        }

        else {
            buffer->rendered[pos] = GapBufferCharAt(buffer, i);
            pos++;
        }
    }

    return 0;
}

