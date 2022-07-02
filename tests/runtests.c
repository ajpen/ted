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

void print_string(char* string, int len){
    for (int i=0; i<len; i++){
        printf("%c", string[i]);
    }

    printf("\n");
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
    int err;

    const char sample1[] = "";
    const char sample2[] = "a";
    const char sample3[] = "aaaaaaaaaaa";  // 11 characters
    const char sample4[] = "aaaaaaaaaaaaaaaaaaaaa";  // 21 characters
    const char sample5[] = "aaaaaaaaaaaaaaaaaaaa";  // 20 characters
    const char sample6[] = "aaabbbaaaaaaaaaaaaaaaaa";
    const char sample7[] = "cccaaabbbaaaaaaaaaaaaaaaaa";
    const char sample8[] = "cccaaabbbaaaaaaaaaaaaaaaaaddd";


    printf("Test 1, empty string\n");
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample1);


    printf("Test 2 insert char\n");
    err = InsertChar(buffer, 'a');
    assert(err == 0);
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample2);


    printf("Test 2.1 insert char\n");
    for (int i=0; i<10; i++){
        err = InsertChar(buffer, 'a');
        assert(err == 0);
    }
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample3);


    printf("Test 2.2 insert char (resize) \n");
    for (int i=0; i<10; i++){
        err = InsertChar(buffer, 'a');
        assert(err == 0);
    }
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample4);


    printf("Test 3 Backspace \n");
    BackSpace(buffer);
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample5);


    printf("Test 4 MoveGap, Insert and Backspace \n");

    err = MoveGap(buffer, 3);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = InsertChar(buffer, 'b');
        assert(err == 0);
    }
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample6);


    printf("Test 4.1 MoveGap, Insert and Backspace \n");

    err = MoveGap(buffer, 0);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = InsertChar(buffer, 'c');
        assert(err == 0);
    }
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample7);


    printf("Test 4.2 MoveGap, Insert and Backspace \n");

    err = MoveGap(buffer, buffer->str_len);
    assert(err == 0);

    for (int i=0; i<3; i++) {
        err = InsertChar(buffer, 'd');
        assert(err == 0);
    }
    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample8);


    printf("Test 5 Backspace\n");
    for (int i=0; i<29; i++){
        BackSpace(buffer);
    }

    string_holder = GetString(buffer);
    string_comp_assert(string_holder, sample1);


    printf("Cleanup...\n");
    DestroyGapBuffer(buffer);

    printf("All Tests Passed.");
}

