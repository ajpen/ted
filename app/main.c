#include <ctype.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include "../buffer/buffer.h"

/*** definitions ***/
#define DEFAULT_LINE_SIZE 200
#define DEFAULT_NUM_LINES 1000

// CTRL_KEY macro returns the value of the k as a control key combination; basically CTRL + k
#define CTRL_KEY(k) ((k) & 0x1f)

// ANSI Escape Codes
#define ESC '\x1b'
#define INVERT_COLOUR "\x1b[7m"
#define INVERT_COLOUR_SIZE 4
#define RESET_INVERT_COLOUR "x1b[27m"
#define RESET_INVERT_COLOUR_SIZE 5

/* Constants */
enum specialKeys {
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT
};

/* structs */

struct VirtualScreen {
    char* buffer;           // Internal representation of the screen
    int buf_pos;
    int len;
};

// Main state & buffers
struct EditorState {

    // configuration states
    struct termios orig_termios;

    // File states
    char* file_name;
    char* file_path;         // path to file.
    FILE* fp;                // Opened file buffer. NULL if editor opened with a new file
    int flushed;             // was the most recent changes flushed to disk

    // buffer states
    TextBuffer* current_buffer;

    // Window states
    struct VirtualScreen screen;
    int screen_rows;
    int screen_cols;
    int vcursor_row;
    int vcursor_col;
};

/* global editor state */
struct EditorState editor_state;


/* prototypes */

/* System and Configuration */
void panic(const char* message);
void initialize(int argc, char* argv[]);
void cleanup();
void set_window_size();
void disableRawMode();
void enableRawMode();

/* Screen Manipulation */
void render_screen();
void draw_screen();
void draw_status_line(int line_size);

void screen_append(const char *str, int size);

/* Cursor Movement */
void move_cursor();
void up_arrow();
void down_arrow();
void right_arrow();
void left_arrow();

/* Input */
int read_char();
void process_keypress();

/* File Manipulation*/
int flush_buffer_to_file();
int load_file_and_initialize_buffer();


/* Main */
int main(int argc, char* argv[]) {
    initialize(argc, argv);

    while (1) {
        draw_screen();
        render_screen();
        process_keypress();
    }
}


/******************************* Implementations *********************************/

// TODO: Moving the cursor makes the cursor get a 'h' in the cursor space. Why?
/* Cursor Movement */
void move_cursor() {

    // Whether the cursor can be moved depends on where we currently are, the number of lines between the current cursor
    // row and the last line in the textbuffer,
    //


}

void up_arrow() {

    if (editor_state.vcursor_row > 0){
        editor_state.vcursor_row--;
    }
}

void down_arrow() {

    // "-2" because we reserved a row to the status bar.
    if (editor_state.vcursor_row < editor_state.screen_rows - 2){
        editor_state.vcursor_row++;
    }
}

void left_arrow() {
    if (editor_state.vcursor_col > 0){
        editor_state.vcursor_col--;
    }
}

void right_arrow() {
    if (editor_state.vcursor_col < editor_state.screen_cols - 1){
        editor_state.vcursor_col++;
    }
}


/* Input */
int read_char(){
    char c;
    ssize_t err;

    while((err = read(STDIN_FILENO, &c, 1)) != 1) {
        if (err == EAGAIN) {
            panic("read_char: read() returned EAGAIN");
        }
    }

    // Handle escape sequences
    if (c == ESC){
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESC;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESC;

        if (seq[0] == '['){
            switch (seq[1]) {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
            }
        }

        return ESC;

    } else {
        return c;
    }
}


void process_keypress(){

    int c = read_char();

    switch (c) {

        case ARROW_UP: up_arrow(); break;
        case ARROW_DOWN: down_arrow(); break;
        case ARROW_LEFT: left_arrow(); break;
        case ARROW_RIGHT: right_arrow(); break;


        case CTRL_KEY('q'):

            // Clear screen
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);

            cleanup();
            exit(0);
            break;
    }
}

void initialize(int argc, char* argv[]){

    // Get file path information
    if (argc >= 2){
        editor_state.file_path = argv[1];
    }
    else {
        editor_state.file_path = "Empty Buffer";
    }
    editor_state.file_name = basename(editor_state.file_path);

    // Loads the file and initialize the textbuffer
    load_file_and_initialize_buffer();

    // initialize screen
    enableRawMode();
    set_window_size();

    // double the buffer for the screen to allow escape codes to be sent without overflowing
    editor_state.screen.len = editor_state.screen_rows * editor_state.screen_cols * sizeof(char) * 2;
    editor_state.screen.buffer = malloc(editor_state.screen.len);
    editor_state.screen.buf_pos = 0;

    // Cursor home position
    editor_state.vcursor_col = 0;
    editor_state.vcursor_row = 0;

    editor_state.flushed = 1;
}

void cleanup(){
    // free memory for screen
    free(editor_state.screen.buffer);

    // Free the text buffer
    DestroyTextBuffer(editor_state.current_buffer);

    // clean up file pointers
    if (editor_state.fp != NULL){
        fclose(editor_state.fp);
    }
}

void panic(const char* message){
    // Clear screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    // Enable cursor
    write(STDOUT_FILENO,"\x1b[?25h", 6);
    perror(message);
    exit(1);
}

void set_window_size() {
    struct winsize ws;
    int rows, cols;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // panic("Failed to get window size");

        // manually get window size
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12 != 12)) {
            panic("Failed to get window size");
        }

        // read the response
        char buf[32];
        for (int i = 0; i < sizeof(buf); i++) {
            if (read(STDIN_FILENO, &buf[i], 1) != 1) {
                panic("Failed to get window size");
            }

            if (buf[i] == 'R') {
                buf[i + 1] = '\0';
                break;
            }
        }

        // parse the response and set the windows size
        if (sscanf(buf, "%d;%d", &editor_state.screen_rows, &editor_state.screen_cols) != 2) {
            panic("Failed to get windows size");
        }

    } else {
        editor_state.screen_rows = ws.ws_row;
        editor_state.screen_cols = ws.ws_col;
    }
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editor_state.orig_termios) == -1) {
        panic("tcsetattr");
    }
}

// Test raw mode
void enableRawMode(){

    if(tcgetattr(STDIN_FILENO, &editor_state.orig_termios) == -1) {
        panic("tcgetattr");
    }

    atexit(disableRawMode);

    struct termios raw = editor_state.orig_termios;

    /*
     * Local modes:
     * Here we are disabling:
     * ECHO: don't echo input characters back to the screen.
     * ICANON: canonical mode. Now each character is read as soon as it is typed.
     * ISIG: SIGINT/SIGSTOP signal interpretation. Now Ctrl-C/Ctrl-Z won't send signals to the process.
     * IEXTEN: extended input processing. Now Ctrl-V/Ctrl-C/Ctrl-D won't be interpreted as special characters.
     *
     * | - logic OR
     * & - logic AND
     * ~ - logic XOR
     */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    /*
     * Input modes:
     * Here we're disabling:
     * IXON: disable software flow control.
     * ICRNL: maps CR to NL (otherwise, it will be CR+NL). Now Ctrl-M will be ignored.
     * BRKINT: disable break processing. Now break condition will be ignored.
     * INPCK: disable parity checking. Now parity errors will be ignored. (This doesn't matter anymore)
     * ISTRIP: disable stripping of eighth bit. Now eighth bit will be ignored. (This probably doesn't matter)
     **/
    raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK);

    /*
     * Output modes:
     * Here we're disabling:
     * OPOST: output processing. Now output processing is disabled.
     *
     **/
    raw.c_oflag &= ~(OPOST);

    /*
     * Control modes:
     * Here we're disabling:
     * CS8: 8-bit characters. Now 8-bit characters will be used.
     **/
    raw.c_cflag |= (CS8);

    /*
     * c_cc - control characters.
     * VMIN: minimum number of characters to read. Set to 0 so that we can read whatever is available.
     * VTIME: time to wait for characters to be available. Set to 1 (100ms).
     **/
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        panic("tcsetattr");
    }
}


/* Display */
void render_screen() {

    // flush internal screen to display
    write(STDOUT_FILENO, editor_state.screen.buffer, editor_state.screen.len);
}


void draw_screen(){

    editor_state.screen.buf_pos = 0; // reset the screen
    int screen_str_pos = 0;  // position on the screen

    // Disable cursor
    screen_append("\x1b[?25l", 6);

    // Clear screen
    screen_append("\x1b[2J", 4);

    // Move cursor to the top
    screen_append("\x1b[H", 3);

    // TESTING STATUS BAR
    for (; screen_str_pos<editor_state.screen_rows; screen_str_pos++){

        if (screen_str_pos == editor_state.screen_rows-1){
            draw_status_line(editor_state.screen_cols);
        }

        if (screen_str_pos < editor_state.screen_rows-1){
            screen_append("\r\n", 2);
        }
    }

    // Move cursor to the cursor position
    char buf[32];
    sprintf(buf, "\x1b[%d;%dH", editor_state.vcursor_row + 1, editor_state.vcursor_col + 1);
    screen_append(buf, strlen(buf));

    // Enable cursor
    screen_append("\x1b[?25h", 6);
}


void draw_status_line(int line_size) {

    const char commands[] = "Ctrl+Q-quit Ctrl+S-Save";
    unsigned int commands_len = sizeof commands-1;

    const char modified[] = "changed";
    int modified_len = sizeof modified-1;

    /*
     * {|file cursor space| |modified len|    |       controls       |
     * [filename.c | 5,50   changed           Ctrl+Q-quit Ctrl+S-Save]
     * */

    // Calculate space for each part of the status line
    int file_cursor_space = line_size - (commands_len + modified_len);
    int cur_col_digits = snprintf(NULL, 0, "%d", editor_state.current_buffer->cursorCol);
    int cur_row_digits = snprintf(NULL, 0, "%d", editor_state.current_buffer->cursorRow);

    int f_name_space = file_cursor_space - (cur_col_digits + cur_row_digits + 5);
    int file_name_size = strlen(editor_state.file_name);

    int cursor_info_buffer_size = file_cursor_space - f_name_space;
    char cursor_info_buffer[cursor_info_buffer_size];


    // invert the colours
    screen_append(INVERT_COLOUR, INVERT_COLOUR_SIZE);

    // Write file name. If its longer than available space, we'll cut it short with ellipsis
    if (file_name_size > f_name_space){
        screen_append(editor_state.file_name, f_name_space - 4);

        screen_append("... ", 4);

    } else {
        screen_append(editor_state.file_name, file_name_size);
    }

    // Write col and row info
    sprintf(cursor_info_buffer,
            " | %d,%d ",
            editor_state.current_buffer->cursorRow, editor_state.current_buffer->cursorCol);

    screen_append(cursor_info_buffer, strlen(cursor_info_buffer));

    // Indicate if buffer was modified since last write.
    if (!editor_state.flushed) {
        screen_append(modified, modified_len);
    }
    else {
        memset(editor_state.screen.buffer + editor_state.screen.buf_pos, ' ', modified_len);
        editor_state.screen.buf_pos += modified_len;
    }

    // Fill with whitespace
    memset(editor_state.screen.buffer + editor_state.screen.buf_pos, ' ', f_name_space - file_name_size);
    editor_state.screen.buf_pos += f_name_space - file_name_size;

    // Finally, print help
    screen_append(commands, commands_len);
}


/*
 * Returns:
 * -1: If there was an issue with opening the file; the buffer is still initialized as a blank buffer
 * MEM_ERROR if it wasn't successfully loaded, or there was an issue resizing/manipulating/initializing the TextBuffer.
 * */
int load_file_and_initialize_buffer() {

    // We'll open all files in read mode. If there's no file, we'll just have a blank buffer.
    // Only when writing to file, will we rewrite or create + write to the file.
    editor_state.fp = fopen(editor_state.file_path, "r");

    // CreateTextBufferFromFile handles NULL values so we can just pass editor_state.fp and check the return
    editor_state.current_buffer = CreateTextBufferFromFile(editor_state.fp);

    if (editor_state.current_buffer == NULL){
        return MEM_ERROR;
    }

    if (editor_state.fp == NULL){
        return -1;
    }

    return 0;
}


void screen_append(const char *str, int size) {

    if (editor_state.screen.buffer != NULL && (editor_state.screen.len - editor_state.screen.buf_pos) > size) {
        memcpy(editor_state.screen.buffer + editor_state.screen.buf_pos, str, size);
        editor_state.screen.buf_pos += size;
    }
}


