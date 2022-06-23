//
// Created by Anfernee Jervis on 6/20/22.
//

#include <stdlib.h>
#include "string.h"
#include "gapbuf.h"


/*
 * reallocate the buffer to increase the gapsize.
 * buffer: gapbuffer in question
 * new_size: new buffer size. If < buffer.buffersize x2 is passed, defaults to buffer.buffersize * 2
 * new_gapsize must not be more than new_size/2. If greater than that or less than 0, it defaults to new_size/2
 *
 * returns:
 *  1 if successful,
 *  MEMERROR if there was an any issue with memory (OOM)
 * */
int reallocBuffer(gapbuffer* buffer, int new_size, int new_gapsize){

    new_size = new_size < buffer->buffersize * 2 ? buffer->buffersize * 2: new_size;

    if (new_gapsize > (new_size/2) || new_gapsize <= 0 ){
        new_gapsize = new_size/2;
    }

    char* newbuf = malloc(sizeof(char) * new_size);

    if (newbuf == NULL){
        return MEMERROR;
    }

    /*
     * Assume the following gapbuf:
     * size = 17
     * gapsize = 5
     * gaploc = 4
     * [t,e,x,t,\0,\0,\0,\0,\0,s,o,m,e,m,o,r,e]
     *
     * Assume the new buffer
     * [-----------------------------------------------------------------------------------]
     *
     * We want to copy from 0 to (gaploc-1) (copy `gaploc` number of characters)
     * set from gaploc to gaploc + new_gapsize as \0
     * then copy from gaploc + gapsize to size-1 (copy `size-(gaploc + gapsize)` number of characters)
     * */

    // first, copy up to the start of the gapbuffer
    memcpy(newbuf, buffer->buffer, sizeof(char) * (buffer->gap_loc));

    // initialize the gap (set its contents to \0)
    memset(
        newbuf + buffer->gap_loc,
        '\0',
        new_gapsize);

    // finally, copy the rest of the gapbuffer
    memcpy(
        (newbuf + buffer->gap_loc + new_gapsize),
        (buffer->buffer + buffer->gap_loc + buffer->gap_size),
        buffer->buffersize - (buffer->gap_loc + buffer->gap_size));

    return 1;
}


gapbuffer* CreateGapBuffer(int buffersize, int gapsize, int gaploc){
    gapbuffer* newBuffer = malloc(sizeof(gapbuffer));
    if(newBuffer == NULL){
        return NULL;
    }

    newBuffer->buffer = malloc(sizeof(char) * buffersize);
    if(newBuffer->buffer == NULL){
        return NULL;
    }

    newBuffer->gap_size = gapsize;
    newBuffer->buffersize = buffersize;
    newBuffer->gap_loc = gaploc;

    return newBuffer;
}


void DestroyGapBuffer(gapbuffer* buffer){
    if (buffer != NULL){
        free(buffer->buffer);
        free(buffer);
        buffer = NULL;
    }
}


int GapBufferAppend(gapbuffer* buffer, char ch){

    if (buffer->gap_size == 0){
        int result = reallocBuffer(buffer, 0, 0); // Use the defaults

        if (result != 1){
            return result;
        }
    }

    buffer->buffer[buffer->gap_loc] = ch;
    buffer->gap_size--;
    buffer->gap_loc++;
    return 1;
}


int GapBufferInsert(gapbuffer* buffer, char* str, int len){

    if (buffer->gap_size <= len){
        int result = reallocBuffer(buffer, 0, 0); // Use the defaults

        if (result != 1){
            return result;
        }
    }

    memcpy(
       buffer->buffer + buffer->gap_loc,
       str,
       len);

    buffer->gap_size -= len;
    buffer->gap_loc += len;

    return 1;
}


int GapBufferErase(gapbuffer* buffer, int n){

    // we can remove at most `buffer->gap_loc` characters
    if (buffer->gap_loc < n){
        n = buffer->gap_loc;
    }

    memset((buffer->buffer + (buffer->gap_loc - n)),  // Goes to the gap location - n (since memset moves -> direction)
           '\0',
           n);

    return n;
}


void GapBufferMoveGap(gapbuffer* buffer, int newloc){

    if (newloc > (buffer->buffersize - buffer->gap_size)){
        newloc = buffer->buffersize - buffer->gap_size;
    }

    if (newloc < 0){
        newloc = 0;
    }

    // TODO: Create a new buffer to copy the contents. Copying in place can overwrite parts of the buffer
    //  that wasn't copied as yet
}