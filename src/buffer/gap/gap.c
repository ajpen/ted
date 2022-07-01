//
// Created by Anfernee Jervis on 6/29/22.
//

#include <stdlib.h>
#include <string.h>

#include "gap.h"


/*
 * helper function for copying the contents of a GapBuffer.buffer to a new buffer, with/without
 * resizing the gap. This method is useful when moving the 'cursor' or when resizing a filled buffer.
 * instance: GapBuffer instance
 * new_capacity: The new size of the buffer. If <= original capacity, the same capacity will be used.
 *
 * return 0 on success or MEM_ERROR
 * */
int resizeBuffer(GapBuffer* instance, int new_capacity){

    int buffer_size = instance->str_len + instance->gap_len;

    if (new_capacity < buffer_size){
        new_capacity = buffer_size;
    }

    // If we increase the capacity, all the new space should go to the gap.
    int gap_size = instance->gap_len + (new_capacity - buffer_size);

    // First malloc new buffer
    char* new_buffer = malloc(sizeof(char) * new_capacity);

    if (new_buffer == NULL){
        return MEM_ERROR;
    }

    // first, copy up to the start of the gap
    memcpy(new_buffer, instance->buffer, sizeof(char) * (instance->gap_loc));

    // Copy the rest of the buffer, starting from the suffix of the gap
    memcpy(
            (new_buffer + instance->gap_loc + gap_size),                // [a, a, a, a, _, _, _, starts here>a, a, a, a]
            (instance->buffer + instance->gap_loc + instance->gap_len), // [a, a, a, a, _, starts here>a, a, a, a]
            instance->str_len - instance->gap_loc);  //  now we copy the remaining str_len - gap_loc characters

    free(instance->buffer);
    instance->buffer = new_buffer;

    return 0;
}


GapBuffer* CreateGapBuffer(int capacity){

    GapBuffer* gap_buffer = malloc(sizeof(GapBuffer));

    if (gap_buffer == NULL){
        return NULL;
    }

    gap_buffer->buffer = malloc(sizeof(char) * capacity);

    if (gap_buffer->buffer == NULL){
        free(gap_buffer);
        return NULL;
    }

    gap_buffer->gap_loc = 0;
    gap_buffer->gap_len = capacity;
    gap_buffer->str_len = 0;

    return gap_buffer;
}


void DestroyGapBuffer(GapBuffer * instance){
    free(instance->buffer);
    free(instance);
}


int InsertChar(GapBuffer* instance, char ch){

    int errno;

    // If the gap is about to close, resize it.
    if (instance->gap_len <= 1){
        int current_cap = instance->gap_len + instance->str_len;
        if ((errno = resizeBuffer(instance, current_cap * 2)) != 0){
            return errno;
        }
    }

    instance->buffer[instance->gap_loc] = ch;
    instance->str_len++;
    instance->gap_loc++;
    instance->gap_len--;

    return 0;
}


void BackSpace(GapBuffer* instance){

    if (instance->gap_loc > 0){
        instance->gap_loc--;
        instance->gap_len++;
        instance->str_len--;
    }
}


int MoveCursor(GapBuffer* instance, int location){
    if (location > instance->str_len-1){
        location = instance->str_len;
    }

    if (location < 0){
        location = 0;
    }

    int capacity = instance->gap_len + instance->str_len;
    char* new_buffer = malloc(sizeof(char) * capacity);

    if (new_buffer == NULL){
        return MEM_ERROR;
    }

    if (location < instance->gap_loc){

        // here the new location is before the current gap location
        // copy up to location
        memcpy(new_buffer, instance->buffer, sizeof(char) * (location));

        // then copy what remains up to instance.gaploc, to the position after the gap in the new buffer
        memcpy(new_buffer + location + instance->gap_len,
               instance->buffer + location,
               sizeof(char) * (instance->gap_loc-location));

        // copy the rest of the string
        memcpy((new_buffer + location + instance->gap_len + (instance->gap_loc-location)),
                (instance->buffer + instance->gap_loc + instance->gap_len),
               sizeof(char) * instance->str_len - instance->gap_loc);  //  now we copy the remaining str_len - gap_loc characters
    } else {

        // here the new location is further after the current gap location
        // copy upto original gap
        memcpy(new_buffer, instance->buffer, sizeof(char) * (instance->gap_loc));

        // copy whats left before the new location
        memcpy(new_buffer+instance->gap_loc,
               instance->buffer + instance->gap_loc + instance->gap_len,
               sizeof(char) * (location - instance->gap_loc));

        // now copy the remaining string after the gap
        memcpy(new_buffer + location + instance->gap_len,
               instance->buffer + instance->gap_loc + instance->gap_len + (location - instance->gap_loc),
               sizeof(char) * instance->str_len - location);
    }

    return 0;

}