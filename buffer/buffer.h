//
// Created by Anfernee Jervis on 7/1/22.
//

#ifndef TED_BUFFER_H
#define TED_BUFFER_H

#include "gap.h"

/*
 * TextBuffer
 * This data structure represents the current buffer of the text editor.
 * It's essentially an array of gap buffers, each gap buffer representing a line in the file.
 *
 * [] -> [contents of line |       |  one]
 * [] -> [contents of line |       |  twp]
 * [] -> [contents of line |       |  three]
 * [] -> [contents of line |       |  four]
 * [] -> [contents of line |       |  five]
 * [] -> []
 * [] -> []
 *
 * Note that blank lines will have a buffer. This can be optimized later (with a performance penalty for inserts)
 *
 * Having the rows be an array allows constant lookup so file exploration is cheap.
 * Copying small strings is cheap so gap buffers for lines shouldn't be too expensive.
 *
 * Additionally, this structure will hold details about the current state of the text editor,
 * such as the cursor position (row, col).
 * */

typedef struct TextBuffer {
    GapBuffer** lines;
    int cursor[2];
} TextBuffer;


/*
 * CreateTextBuffer creates and initializes a new text buffer.
 * lines: The number of lines to use
 * line_size: Default initial line size to allocate.
 * Returns a pointer to an initialized TextBuffer or NULL
 * */
TextBuffer* CreateTextBuffer(TextBuffer* instance, int lines, int line_size);


/*
 * MoveCursor moves the cursor to row and column given. If the values are out of bounds, it's moved
 * to the closest valid position (e.g. if row is negative, it's moved to row 0)
 *
 * Memory is allocated as needed.
 *
 * Returns 0 on success or MEM_ERROR
 * */

int TextBufferMoveCursor(TextBuffer* instance, int row, int col);


/*
 * Insert inserts a character at the cursor location. Space is made for the inserted character; it doesn't overwrite
 * a character that might be at the cursor's current position.
 *
 * Memory is allocated as needed.
 *
 * Returns 0 on success, or MEM_ERROR
 * */

int TextBufferInsert(TextBuffer* instance, char ch);


/*
 * Backspace deletes the character that appears before the cursor location. Similar to hitting the backspace button.
 * */
void TextBufferBackspace(TextBuffer* instance);

#endif //TED_BUFFER_H
