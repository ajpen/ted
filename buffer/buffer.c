//
// Created by Anfernee Jervis on 7/1/22.
//

#include "buffer.h"
#include "gap.h"
#include <stdlib.h>


TextBuffer* CreateTextBuffer(int num_lines, int line_size){
    TextBuffer* textBuffer = malloc(sizeof(TextBuffer));

    if (textBuffer == NULL){
        return NULL;
    }

    // Allocate gap buffer array
    textBuffer->lines = malloc(sizeof(GapBuffer*) * num_lines);

    if (textBuffer->lines == NULL){
        return NULL;
    }

    // allocate the first line
    textBuffer->lines[0] = CreateGapBuffer(line_size);

    if (textBuffer->lines[0] == NULL){
        return NULL;
    }

    // NULL the rest of the lines
    for (int i=1; i<num_lines; i++){
        textBuffer->lines[i] = NULL;
    }

    textBuffer->num_lines = num_lines;
    textBuffer->cursorRow = 0;
    textBuffer->cursorCol = 0;
    textBuffer->cursorColMoved = 0;
    textBuffer->last_line_loc = 0;

    return textBuffer;
}


void DestroyTextBuffer(TextBuffer* instance){

    // Deallocate each GapBuffer
    for(int i=0; i<instance->num_lines; i++){

        // Break if we're at the last line
        if (instance->lines[i] == NULL){
            break;
        }

        DestroyGapBuffer(instance->lines[i]);
    }

    // Deallocate the gapbuffer array and the TextBuffer itself
    free(instance->lines);
    free(instance);
}


void TextBufferMoveCursor(TextBuffer* instance, int row, int col){
    if (row > instance->last_line_loc){
        row = instance->last_line_loc;
    }
    if (row < 0){
        row = 0;
    }

    instance->cursorRow = row;

    // We dont need to worry about checking out of bounds for col. GapBuffer's MoveGap handles that
    // We do however, need to indicate that the column moved so the gap can be moved before inserting
    if (instance->cursorCol != col){
        instance->cursorColMoved = 1;
    }

    instance->cursorCol = col;
}


int TextBufferInsert(TextBuffer* instance, char ch){

    int errno;

    // If the cursor column changed, we need to move the gap buffer before inserting
    if (instance->cursorColMoved) {
        errno = GapBufferMoveGap(instance->lines[instance->cursorRow], instance->cursorCol);

        if (errno != 0){
            return errno;
        }

        instance->cursorColMoved = 0;
    }
    return GapBufferInsertChar(instance->lines[instance->cursorRow], ch);
}


void TextBufferBackspace(TextBuffer* instance){
    GapBufferBackSpace(instance->lines[instance->cursorRow]);
}