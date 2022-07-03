//
// Created by Anfernee Jervis on 7/1/22.
//

#include <assert.h>
#include <stdlib.h>
#include "stdio.h"
#include <string.h>
#include "../buffer/gap.h"


// Test Suites
void TestGapBuffer();


int main(){
    TestGapBuffer();
}


void string_comp_assert(char* str1, const char* str2){
    assert(str1 != NULL);
    assert(str2 != NULL);
    assert(strcmp(str1, str2) == 0);
    free(str1);
}

void TestGapBuffer(){

    GapBuffer* buffer = CreateGapBuffer(20);
    char* string_holder = NULL;
    char* string_holder2 = NULL;
    int err;

    const char sample1[] = "";
    const char sample2[] = "a";
    const char sample3[] = "aaaaaaaaaaa";  // 11 characters
    const char sample4[] = "aaaaaaaaaaaaaaaaaaaaa";  // 21 characters
    const char sample5[] = "aaaaaaaaaaaaaaaaaaaa";  // 20 characters
    const char sample6[] = "aaabbbaaaaaaaaaaaaaaaaa";
    const char sample7[] = "cccaaabbbaaaaaaaaaaaaaaaaa";
    const char sample8[] = "cccaaabbbaaaaaaaaaaaaaaaaaddd";
    const char sample9[] = "cccaaa";
    const char sample10[] ="bbbaaaaaaaaaaaaaaaaaddd";


    printf("Test 1, empty string\n");
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample1);


    printf("Test 2 insert char\n");
    err = GapBufferInsertChar(buffer, 'a');
    assert(err == 0);
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample2);


    printf("Test 2.1 insert char\n");
    for (int i=0; i<10; i++){
        err = GapBufferInsertChar(buffer, 'a');
        assert(err == 0);
    }
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample3);


    printf("Test 2.2 insert char (resize) \n");
    for (int i=0; i<10; i++){
        err = GapBufferInsertChar(buffer, 'a');
        assert(err == 0);
    }
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample4);


    printf("Test 3 Backspace \n");
    GapBufferBackSpace(buffer);
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample5);


    printf("Test 4 GapBufferMoveGap, TextBufferInsert and Backspace \n");

    err = GapBufferMoveGap(buffer, 3);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = GapBufferInsertChar(buffer, 'b');
        assert(err == 0);
    }
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample6);


    printf("Test 4.1 GapBufferMoveGap, TextBufferInsert and Backspace \n");

    err = GapBufferMoveGap(buffer, 0);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = GapBufferInsertChar(buffer, 'c');
        assert(err == 0);
    }
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample7);


    printf("Test 4.2 GapBufferMoveGap, TextBufferInsert and Backspace \n");

    err = GapBufferMoveGap(buffer, buffer->str_len);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = GapBufferInsertChar(buffer, 'd');
        assert(err == 0);
    }
    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample8);


    printf("Test 5 Split\n");
    err = GapBufferMoveGap(buffer, 6);
    assert(err == 0);

    GapBuffer* buffer2 = GapBufferSplit(buffer);
    assert(buffer2 != NULL);

    string_holder = GapBufferGetString(buffer);
    string_holder2 = GapBufferGetString(buffer2);

    printf("Buffer 1: %s\n", string_holder);
    printf("Buffer 2: %s\n", string_holder2);


    string_comp_assert(string_holder, sample9);
    string_comp_assert(string_holder2, sample10);


    printf("Test 6 Backspace\n");
    for (int i=0; i<6; i++){
        GapBufferBackSpace(buffer);
    }

    string_holder = GapBufferGetString(buffer);
    string_comp_assert(string_holder, sample1);


    printf("Cleanup...\n");
    DestroyGapBuffer(buffer);
    DestroyGapBuffer(buffer2);

    printf("All Tests Passed.");
}

