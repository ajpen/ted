#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "../buffer/buffer.h"

struct termios orig_termios;


void panic(const char* message){
    perror(message);
    exit(1);
}


void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        panic("tcsetattr");
    }
}

// Test raw mode
void enableRawMode(){

    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        panic("tcgetattr");
    }

    atexit(disableRawMode);

    struct termios raw = orig_termios;

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
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }
    return 0;
}
