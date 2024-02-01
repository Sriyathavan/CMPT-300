#include "list.h"
#include <stdio.h>


//static variables
static int uninitialized = 1, activeHeads = 0;

//statically allocated mem
//empty nodes
static List emptyNodes; //will take from head when removed, append new free nodes to tail (basically a queue)
static Node nodes[LIST_MAX_NUM_NODES]; //array of nodes

//when deleting push in front of emptyHead, if emptyHead = NULL -> no free heads
List *emptyHead;
static List heads[LIST_MAX_NUM_HEADS]; //array of heads

//declarations for private functions:
void initializeHeads();
void initializeNodes();
Node* getFreeNode();
List* getFreeHead();

void sendFreeNodes(Node* start, Node* end);

void printNode(Node* node);
void printAll();
void printList(List *pList);

//public:
//creating a list -> we need to make the static section before doing anything
//lists will act as "heads"
List* List_create() {
    //check if the structures have been set up
    if (uninitialized) {
        initializeNodes();
        initializeHeads();
        uninitialized = 0; //intialization code will not run after first time
    }

    //check if we have room -> else: NULL
    if (activeHeads >= LIST_MAX_NUM_HEADS) {
        return NULL;
    }

    //make the list (can't use new)
    List* list = getFreeHead(); //should be set as empty

    return list;
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    //free each item in List
    pList->current = pList->head;
    while (pList->current != NULL) {
        //takes pointer to the item to be freed
        (*pItemFreeFn)(pList->current->val);
        // pList->current->val = NULL; //clear pointer
    }

    //append the nodes to the free pool of nodes
    sendFreeNodes(pList->head, pList->tail);

    //add the current head to the free pool of heads
    pList->next = emptyHead; //trivialy, if NULL this will be okay
    emptyHead = pList;
}

//0 -> success, -1 -> fail
int List_append(List* pList, void* pItem) {
    //find a free node
    Node* free = getFreeNode();

    //no nodes available
    if (free = NULL) {
        return -1;
    }
    
    //add val
    free->next = NULL; //break from emptyNodes, prev should be NULL (we are taking from head)
    free->val = pItem;
    free->empty = false; //set as occupied

    //append, condition: empty list
    if (pList->n == 0) {
        pList->current = pList->head = free; //tail is set later anyways
    } else {
        //last-> next = free
        pList->tail->next = free;
    }
    
    //edit List data
    pList->tail = free;
    pList->n++;

    return 0;
}

void* List_remove(List* pList) {
    //conditions:
    //empty (current == LIST_OOB_START), before start, after start
    if (pList->current == LIST_OOB_START || pList->current == LIST_OOB_END) {
        return NULL;
    }

    //remove current item
    void *item = pList->current->val;
    Node *free = pList->current;

    //conditions:
    //n == 1: list is now empty
    if (pList->n == 1) {
        setEmpty(pList);
        goto end;
    }

    //given: n > 1
    //current == head: we are removing the head
    if (pList->current == pList->head) {
        pList->current = pList->head->next;
        pList->head = pList->current;
        goto end;
    }

    //current == tail: will go out of bounds, tail will have to be tail-> prev
    if (pList->current == pList->tail) {
        pList->current = LIST_OOB_END;
        pList->tail = pList->tail->prev;
        goto end;
    }

    //normal functionality
    pList->current = pList->current->next;

    end: {
        //send free node back to pool
        sendFreeNodes(free, free);
        //update pList
        pList->n++;
    }

    return item;
}

//private:
//sets up nodes -> starts as freeNodes
void initializeNodes () {
    //set up the empty nodes (a list of empty nodes), last->next point to NULL, first-> prev also NULL
    //idea: use dummy nodes in front and back
    //asumption: n > 1?

    //first node 
    nodes[0].next = &nodes[1];
    nodes[0].prev = NULL;
    nodes[0].empty = true;
    nodes[0].val = 0; //check below

    //middle nodes
    for (int i = 1; i < LIST_MAX_NUM_NODES - 1; i++) {
        nodes[i].next = &nodes[i + 1];
        nodes[i].prev = &nodes[i - 1];
        nodes[i].empty = true;
        nodes[i].val = 0; //should be NULL -> trivial since our node is empty for now
    }

    //last node
    Node *last = &nodes[LIST_MAX_NUM_NODES - 1];
    last->prev = &nodes[LIST_MAX_NUM_NODES - 2];
    last->empty = true;
    last->next = NULL;

    emptyNodes.current = &nodes[0];
    emptyNodes.head = &nodes[0];
    emptyNodes.tail = &nodes[LIST_MAX_NUM_NODES - 1];
    emptyNodes.n = LIST_MAX_NUM_NODES;
}

void initializeHeads () {
    //treat the "Lists/heads" as another linked list

    //want each head to point the i + 1 (except last)
    for (int i = 0; i < LIST_MAX_NUM_HEADS - 1; i++) {
        heads[i].next = &heads[i + 1];
        setEmpty(&heads[i]);
    }

    //last points to NULL
    heads[LIST_MAX_NUM_HEADS - 1].next = NULL;
    setEmpty(&heads[LIST_MAX_NUM_HEADS]);

    emptyHead = &heads[0];
}

//return NULL if no free nodes
Node* getFreeNode() {
    if (emptyNodes.n == 0) {
        return NULL;
    }

    //pop
    Node* free = emptyNodes.head;
    emptyNodes.head = emptyNodes.head->next;
    free->next = NULL;
    //prev, val should be NULL

    return free;
}

//add nodes back to the free nodes pool
void sendFreeNodes(Node* start, Node* end) {
    emptyNodes.tail->next = start;
    emptyNodes.tail = end;
}

//finds an unused head (list), none -> return NULL
List* getFreeHead () {
    //check if there are any heads left
    if (emptyHead == NULL) {
        return NULL;
    }

    //pop
    List *free = emptyHead;
    emptyHead = emptyHead -> next;

    return free;
}

void setEmpty (List* pList) {
    pList->current = LIST_OOB_START;
    pList->head = NULL;
    pList->tail = NULL;
    pList->n = 0;
}

void printNode (Node* node) {
    if (node->empty) {
        printf("EMPTY");
        return;
    }

    printf("VAL"); //TO DO
}

void Print_all () {
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        printNode(&nodes[i]);
        printf("\n");
    }
}

void Print_list (List* pList) {
    Node* current = emptyNodes.head;
    while (current != NULL) {
        printNode(current);
        printf("\n");
        current = current->next;
    }
}
