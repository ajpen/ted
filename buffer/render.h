//
// Created by Anfernee Jervis on 11/4/22.
//

#ifndef TED_RENDER_H
#define TED_RENDER_H

#include "gap.h"


/*
 * Settings for rendering
 * */
typedef struct RenderConfig {
    int tabsize;
} RenderConfig;


/*
 * TODO: Can make this a function that calls other functions that render different parts (syntax highlight, tabs, etc)
 * Renders the string in a gap buffer, populating the rendered field
 * If rendered is not null, it reuses the buffer, extending it when necessary.
 * What happens during render is determined by the RenderConfig passed
 * */
int RenderGapBuffer(GapBuffer* buffer, RenderConfig config);

#endif //TED_RENDER_H
