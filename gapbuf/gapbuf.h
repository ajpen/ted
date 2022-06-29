/*
 * gapbuffer.h
 * Defines the interface for working with the gapbuffer data structure.
 * Note that the gapbuffer datatype should not be manually populated.
 * Use the interface methods for creating/destroying the structure.
 *
 * */

#ifndef TEDITOR_GAPBUF_H
#define TEDITOR_GAPBUF_H


#define DEFAULTGAPBUFFERSIZE 500
#define DEFAULTGAPSIZE 500

#define MEMERROR -1


/*
 * Gap Buffer Data structure
 * Allocates a fixed character array, with a "gap" of size `gap_size`.
 * New text is always inserted at the gap.
 * Deleted text is added as a prefix to the gap.
 *
 * Text can be added char by char or a string is copied.
 * If the gap is filled, a new buffer is allocated, at double the size, with the gap placed at the same location.
 * The gap moves, similar to a cursor, depending on where text is ment to be inserted. Movement is done by copying
 * the string.
 *
 * For example, if this is the gap
 *
 *    [aaaaaaaasasdsdadasdasdasdas__________________adasdasdadsdasdasdasdas]
 *
 * Adding "randomtext" does
 *
 *    [aaaaaaaasasdsdadasdasdasdasrandomtext________adasdasdadsdasdasdasdas]
 *
 *
 * Adding "tex" at index 3 does
 *
 *    [aaatex_________aaaaasasdsdadasrandomtextdsasdadasdasdadsdasdasdasdas]
 *
 * Deleting "random" does
 *
 *    [aaatexaaaaasasdsdadas_______________textdsasdadasdasdadsdasdasdasdas]
 *
 * The interface for the gapbuffer, along with the struct definition is below
 * */
typedef struct gapbuffer{
    int buffersize;
    int gap_loc; // offset to the start of the buffer. buffer[gap_loc] is the first character of the buffer
    int gap_size;
    char* buffer;
} gapbuffer;



//////////////////////////////////////// Interface //////////////////////////////////////////////////////////////////

/*
 * CreateGapBuffer creates and allocates a new gap buffer struct and returns a pointer to the structure.
 * buffersize: size of the overall buffer
 * gapsize: How big should the gap be
 * gaploc: initial location of the gap/cursor
 *
 * if the gapbuffer is larger than the buffersize, or the buffersize/gapsize is 0,
 * default values are used.
 *
 * returns: gapbuffer* with allocated attributes OR
 * NULL if there was an error allocating a new gapbuffer.
 * NOTE: This structure must be freed.
 * */
gapbuffer* CreateGapBuffer(int buffersize, int gapsize, int gaploc);

/*
 * DestroyGapBuffer Safely deallocates a gapbuffer. The pointer given is set to NULL after.
 * Passing a NULL pointer does nothing.
 * buffer: pointer to an allocated gapbuffer.
 * */
void DestroyGapBuffer(gapbuffer* buffer);


/*
 * GapBufferAppend Adds a character to the gap in the gap buffer. If the gap is filled, it automatically
 * allocates a new buffer and gap.
 * buffer: gapbuffer in question
 * ch: Character to append
 *
 * returns:
 *   1 if successful,
 *  MEMERROR if there was an any issue with memory (OOM)
 * */
int GapBufferAppend(gapbuffer* buffer, char ch);


/*
 * GapBufferInsert Copies the passed string to the gap buffer. If the gap is filled, it automatically
 * allocates a new buffer and gap.
 *
 * buffer: gapbuffer in question
 * str: char* of string to be copied
 * len: Length of the string
 *
 * returns:
 *   1 if successful,
 *  MEMERROR if there was an any issue with memory (OOM)
 * */
int GapBufferInsert(gapbuffer* buffer, char* str, int len);


/*
 * GapBufferErase Erases n characters backwards from the start of the gap (Analogous to hitting backspace n times)
 * Erased characters are automatically added to the gap.
 *
 * If the start of the buffer is reached before n characters are erased, the rest is ignored.
 *
 * buffer: gapbuffer in question
 * n: Number of characters to erase
 *
 * returns:
 * the number of characters erased
 *
 * */
int GapBufferErase(gapbuffer* buffer, int n);


/*
 * GapBufferMoveGap Moves the gap to the new location specified. The new location should be
 * between 0 to buffersize-gapsize. If its larger than buffersize-gapsize, it'll default to that.
 * If the new location is less than 0, it defaults to 0
 * */
void GapBufferMoveGap(gapbuffer* gapbuffer, int newloc);


#endif //TEDITOR_GAPBUF_H

