//
// Created by Anfernee Jervis on 8/27/22.
//

typedef struct Cursor {
    int row;
    int col;
} Cursor;

struct VirtualScreen {
    char* buffer;           // Internal representation of the screen
    int buf_pos;
    int len;
    Cursor cursor;
};