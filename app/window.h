//
// Created by Anfernee Jervis on 11/5/22.
//

#ifndef TED_WINDOW_H
#define TED_WINDOW_H

#include "../buffer/buffer.h"
#include "../buffer/render.h"

typedef struct FlushableBuffer{
    char* buffer;
    int len;
} FlushableBuffer;

/*
 * Window Render states for the render function
 * --------------
 * Rendered: Window properly rendered
 * Partial: Only modified lines need re-rendering
 * Cascade: Lines below the first modified line needs re-rendering
 * Full: All lines need re-rendering.
 * */
typedef enum WinRenderState {
    Rendered = 0,
    PartialRender,
    CascadingRender,
    FullRender
} WinRenderState;


/*
 * Stores information about a window displayed
 * Each window has a reference for the text buffer it displays.
 * there's also information about window dimensions and whether
 * a full rendering is needed.
 * -----------------------------------------
 * source_buffer: text buffer assigned to the window
 * render_state: determines whether rendering is required
 * render_settings: controls how rendering is done
 * status_line: Buffer representing the status line
 * start_line: index of the topmost line displayed in the window
 * end_line: index of the last line displayed
 * rows: number of rows for that window
 * cols: length of each row of that window
 * cursor_x: x position of the window cursor (1 indexed)
 * cursor_y: y position of the window cursor (1 indexed)
 * */
typedef struct Window {
    TextBuffer* source_buffer;
    WinRenderState render_state;
    RenderConfig* render_settings;
    char* status_line;
    int start_line;
    int end_line;
    int rows;
    int cols;
    int cursor_x;
    int cursor_y;
} Window;


/*
 * Creates a new window with view set to the start of the text
 * buffer and the cursor location at 1, 1
 * Returns NULL if there's an issue
 * */
Window* CreateWindow(TextBuffer* source, RenderConfig* renderConf, int rows, int cols);


/*
 * Depending on the state of the window, re-render some/all the lines
 * in the text buffer such that they're updated.
 *
 * Returns 0 on success
 * */
int WindowRenderTextBuffer(Window* window);


/*
 * Returns a flushable buffer containing the entire window rendered and ready to
 * flush to the screen.
 *
 * Returns NULL on error.
 * */
FlushableBuffer* WindowRenderWindow(Window* window);


/*
 * Moves the cursor in view. Allows the option of making the cursor centered in the window, rather
 * than at the top/bottom (useful when search is done)
 * */
int WindowMoveCursorInView(Window* window, bool center_cursor);

#endif //TED_WINDOW_H
