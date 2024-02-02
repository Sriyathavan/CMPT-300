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

void testAddRemove () {
    List* testList = List_create();

    //some sample "items"
    int arr[] = {1, 2, 3, 4, 5};
    int *pointers[5];
    for (int i = 0; i < 5; i++) {
        pointers[i] = &arr[i];
    }

    List_append(testList, pointers[0]);

    assert(List_count(testList) == 1);
    Print_all();

    assert(List_first(testList) == pointers[0]);
    assert(List_last(testList) == pointers[0]);
    assert(List_curr(testList) == pointers[0]);
    List_append(testList, pointers[1]);
    
    assert(List_first(testList) == pointers[0]);
    assert(List_last(testList) == pointers[1]);
    assert(List_curr(testList) == pointers[1]);

    List_curr(testList);//trivial
    List_insert_after(testList, pointers[2]);
    assert(List_curr(testList) == pointers[2]);
}

//putting max to 5 nodes
void testPool () {
    // List* list1 = List_create();
    // List* list2 = List_create();
    // assert(list2 == NULL);

    //some sample "items"
    // int arr[] = {1, 2, 3, 4, 5};
    // int *pointers[5];
    // for (int i = 0; i < 5; i++) {
    //     pointers[i] = &arr[i];
    //     List_append(list1, pointers[i]);
    // }

    // List_first(list1);
    // List_first(list2);
    // assert(pointers[0] == pointers[1 - 1]);

}

int main() {
    
    // List* testList = List_create();

    // Print_list(testList);
    testEmptyList();
    
    testAddRemove();
    // Print_list(testList);
    Print_all();

    return 0;
}