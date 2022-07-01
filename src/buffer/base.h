//
// Created by Anfernee Jervis on 6/29/22.
// Interface style is inspired by https://chris-wood.github.io/2016/02/12/Polymorphism-in-C.html
//

#ifndef TEDITOR_BUFFER_H
#define TEDITOR_BUFFER_H


/*
 * Defines the interface for the Buffer structure that holds the text in memory when editing.
 * Whatever implementation must have these functions defined before they can be used.
 * */
typedef struct BufferInterface {
    void  (* MoveCursor) (void* buffer_instance);
    int   (* InsertChar) (void* buffer_instance, char ch);
    int   (* BackSpace)  (void* buffer_instance);
    char* (* GetString)  (void* buffer_instance);
    void  (* Destroy)    (void* buffer_instance);
} BufferInterface;

#endif //TEDITOR_BUFFER_H
