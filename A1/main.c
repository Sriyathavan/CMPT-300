#include <stdio.h>
#include "list.h"

int main() {
    initialize();
    List* testList = List_create();
    
    printf("Hello world");

    destruct();

    return 0;
}