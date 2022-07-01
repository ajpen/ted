//
// Created by Anfernee Jervis on 6/29/22.
// Defines the buffer and methods that support its creation with varying implementation details.
//

#ifndef TED_BUFFER_H
#define TED_BUFFER_H
#include "base.h"

/*
 * Buffer: The object used by the editor to store and manipulate text loaded from file/user input.
 * This layout allows varying implementation details and the ability to switch between them.
 *
 * */
typedef struct Buffer {
    void* instance;
    BufferInterface* interface;
} Buffer;


Buffer* CreateBuffer(void* implementation_instance, BufferInterface* interface);
Buffer* DestroyBuffer(Buffer* buffer_instance);

#endif //TED_BUFFER_H
