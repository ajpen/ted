//
// Created by Anfernee Jervis on 6/23/22.
//
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../gapbuf/gapbuf.h"


void createAssertions(gapbuffer* testbuf){
    assert(testbuf != NULL);
    assert(testbuf->buffersize == DEFAULTGAPBUFFERSIZE);
    assert(testbuf->gap_size == DEFAULTGAPSIZE);
    assert(testbuf->gap_loc == 0);
}

int main() {

    printf("Testing CreateGapBuffer handles nonsensical inputs\n");
    gapbuffer* testbuffer = CreateGapBuffer(0, 0, 0);
    createAssertions(testbuffer);
    gapbuffer* testbuffer2 = CreateGapBuffer(5, 10, 0);
    createAssertions(testbuffer2);
    DestroyGapBuffer(testbuffer);
    DestroyGapBuffer(testbuffer2);


    char* expected_create = "\0";


    printf("Testing Append\n");
    testbuffer = CreateGapBuffer(30, 25, 0);
    int err;

    char* expected = "aaaaaaaaaaaaaaaaaaaa\0\0\0\0\0\0\0\0\0\0";

    for (int i=0; i<20; i++){
        err = GapBufferAppend(testbuffer, 'a');
        assert(err != 1);
    }

    for (int i=0; i<20; i++){
        assert(expected[i] == testbuffer->buffer[i]);
    }
}
