#include <stdio.h>
#include "list.h"
#include <assert.h>

//test empty
void testEmptyList () {
    List* emptyList = List_create();
    assert(List_count(emptyList) == 0);
    assert(List_first(emptyList) == NULL);
    assert(List_last(emptyList) == NULL);
    assert(List_next(emptyList) == NULL);
    assert(List_prev(emptyList) == NULL);
    assert(List_curr(emptyList) == NULL);
    assert(List_remove(emptyList) == NULL);
    assert(List_trim(emptyList) == NULL);
}

void testAppend () {
    List* testList = List_create();

    //some sample "items"
    int arr[] = {1, 2, 3, 4, 5};
    int *pointers[5];
    for (int i = 0; i < 5; i++) {
        pointers[i] = &arr[i];
    }

    List_append(testList, pointers[0]);
    assert(List_first(testList) == pointers[0]);
    assert(List_last(testList) == pointers[0]);
    assert(List_curr(testList) == pointers[0]);
}

int main() {
    
    List* testList = List_create();

    Print_list(testList);
    testEmptyList();

    return 0;
}