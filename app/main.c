#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include "../buffer/buffer.h"

/*** definitions ***/
#define MAX_LINE_SIZE 200
#define MAX_NUM_LINES 1000

// CTRL_KEY macro returns the value of the k as a control key combination; basically CTRL + k
#define CTRL_KEY(k) ((k) & 0x1f)

/* prototypes */
void panic(const char* message);



/* structs */

// Main state & buffers
struct EditorState {

    // configuration states
    struct termios orig_termios;
    int screen_rows;
    int screen_cols;


    // buffer states
    TextBuffer* current_buffer;
};

/* global editor state */
struct EditorState editor_state;

/* Functions */
/******************************* Display *****************************************/
void clear_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}


/* Cursor Movement */
void move_cursor(int row, int col) {
    char buf[32];
    sprintf(buf, "\x1b[%d;%dH", row, col);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void up_arrow() {
    write(STDOUT_FILENO, "\x1b[A", 3);
}

void down_arrow() {
    write(STDOUT_FILENO, "\x1b[B", 3);
}

void right_arrow() {
    write(STDOUT_FILENO, "\x1b[C", 3);
}

void left_arrow(){
    write(STDOUT_FILENO, "\x1b[D", 3);
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

/******************************* Input *****************************************/
char read_char(){
    char c;

    if (read(STDIN_FILENO, &c, 1) == EAGAIN) {
        panic("read_char: read() returned EAGAIN");
    }
    return c;
}


void process_keypress(){
    char c = read_char();
    switch (c) {
        case CTRL_KEY('q'):
            clear_screen();
            exit(0);
            break;
    }
}


void panic(const char* message){
    clear_screen();
    perror(message);
    exit(1);
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



int main() {
    enableRawMode();


    while (1) {
        process_keypress();
    }
}
