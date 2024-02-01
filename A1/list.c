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
    emptyNodes.tail->next = pList->head;
    emptyNodes.tail = pList->tail;

    //add the current head to the free pool of heads
    pList->next = emptyHead;
    emptyHead = pList;
}

//0 -> success, -1 -> fail
int List_append(List* pList, void* pItem) {
    //find a free node
    Node* free = Get_free_node();

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
    }

    //last points to NULL
    heads[LIST_MAX_NUM_HEADS - 1].next = NULL;

    emptyHead = &heads[0];

    //set up the list that contains the headNodes that are free
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
