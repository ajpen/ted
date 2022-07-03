//
// Created by Anfernee Jervis on 7/1/22.
//

#include "buffer.h"
#include "gap.h"
#include <stdlib.h>
#include <strings.h>


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

    textBuffer->lines_capacity = num_lines;
    textBuffer->cursorRow = 0;
    textBuffer->cursorCol = 0;
    textBuffer->cursorColMoved = 0;
    textBuffer->last_line_loc = 0;

    return textBuffer;
}


void DestroyTextBuffer(TextBuffer* instance){

    // Deallocate each GapBuffer
    for(int i=0; i<instance->lines_capacity; i++){

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


int TextBufferNewLine(TextBuffer* instance){
    // split the current GapBuffer where the gap is.
    // Create a new GapBuffer and copy the second half of the string to the new GapBuffer
    // Check if there's space to add a new line. If not, reallocate `lines` and copy the old to the new
    // Finally, check if there's an allocated line below the current line. If there is, shift the lines array
    // down one place with memmove.

    int errno;

    // First ensure the gap location reflects the cursor position
    if (instance->cursorColMoved){
        errno = GapBufferMoveGap(instance->lines[instance->cursorRow], instance->cursorCol);

        if (errno != 0){
            return errno;
        }
    }

    // Split the current GapBuffer at the gap location
    // New gap will have the gap at the start of its string.
    GapBuffer* newline = GapBufferSplit(instance->lines[instance->cursorRow]);

    if (newline == NULL){
        return MEM_ERROR;
    }

    // Check if there's space to add a new line. If not, reallocate `lines` and copy the old to the new
    if (instance->last_line_loc == instance->lines_capacity - 1){
        GapBuffer** new_lines = realloc(instance->lines, sizeof(GapBuffer*) * (instance->lines_capacity * 2));

        if (new_lines == NULL){
            return MEM_ERROR;
        }

        instance->lines = new_lines;
        instance->lines_capacity *= 2;
    }

    // Shift the lines array down one place with memmove
    memmove(instance->lines + instance->cursorRow + 2,
            instance->lines + instance->cursorRow + 1,
            sizeof(GapBuffer*) * (instance->last_line_loc - instance->cursorRow));


    // Set the new line to the next available slot
    instance->lines[instance->cursorRow + 1] = newline;

    // Update the last line location
    instance->last_line_loc++;

    // Update the cursor position
    instance->cursorRow++;

    return 0;
}

char *TextBufferGetLine(TextBuffer *instance, int row) {
    if (row > instance->last_line_loc){
        return NULL;
    }
    if (row < 0){
        return NULL;
    }

    return GapBufferGetString(instance->lines[row]);
}
