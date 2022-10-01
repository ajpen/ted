//
// Created by Anfernee Jervis on 9/30/22.
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include "error.h"


void panic(const char* message){
    // Clear screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    // Enable cursor
    write(STDOUT_FILENO,"\x1b[?25h", 6);
    perror(message);
    exit(1);
}