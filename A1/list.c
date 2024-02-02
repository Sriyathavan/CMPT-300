#include "list.h"
#include <stdio.h>


//static variables -> means internally linked (priv)
static int uninitialized = 1;

//statically allocated mem
//empty nodes
static List emptyNodes; //will take from head when removed, append new free nodes to tail (basically a queue)
static Node nodes[LIST_MAX_NUM_NODES]; //array of nodes

//when deleting push in front of emptyHead, if emptyHead = NULL -> no free heads
static List *emptyHead;
static List heads[LIST_MAX_NUM_HEADS]; //array of heads

//declarations for private functions:
static void initializeHeads();
static void initializeNodes();
static Node* getFreeNode();
static List* getFreeHead();

static void sendFreeNodes(Node* start, Node* end);
static void setNext(Node* a, Node* b);
static void setEmpty(List* pList);
static void setNode(Node* node, void* pItem);

//these are debuigging functions (i am externally linking them to testing)
void printNode(Node* node);
void printAll(); //switch to main.c
void printList(List *pList); //swtich to main.c

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
    if (emptyHead == NULL) {
        return NULL;
    }

    //make the list (can't use new)
    List* list = getFreeHead(); //should be set as empty
    return list;
}

int List_count(List* pList){
    return pList->n;
}

void* List_first(List* pList){
    if (pList->head == NULL) {
        return NULL;
    }

    return pList->head->val; //will return NULL if empty
}

void* List_last(List* pList){
    if (pList->tail == NULL) {
        return NULL;
    }

    return pList->tail->val; //will be NULL if empty
}

void* List_next(List* pList){
    //empty
    if (pList->n == 0) {
        return NULL;
    }

    //out of bounds
    if (pList->current == NULL) {
        //before start
        if (pList->outOfBounds == LIST_OOB_START) {
            pList->current = pList->head;
            return pList->current->val;
        }
        //after end
        return NULL;
    }

    //at the tail
    if (pList->current == pList->tail) {
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_END;
        return NULL;
    }

    //includes head -> normal behaviour
    pList->current = pList->current->next;
    return pList->current->val;
}

void* List_prev(List* pList){
    //empty
    if (pList->n == 0) {
        return NULL;
    }

    //out of bounds
    if (pList->current == NULL) {
        //after end
        if (pList->outOfBounds == LIST_OOB_END) {
            pList->current = pList->tail;
            return pList->current;
        }
        //before start
        return NULL;
    }

    //at head
    if (pList->current == pList->head) {
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_START;
        return NULL;
    }

    //normal behaviour
    pList->current = pList->current->prev;
    return pList->current->val;
    
}

void* List_curr(List* pList){
    //empty, outOfBounds
    if (pList->current == NULL) {
        return NULL;
    }

    return pList->current->val;
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    //free each item in List
    pList->current = pList->head;
    while (pList->current != NULL) {
        //takes pointer to the item to be freed
        (*pItemFreeFn)(pList->current->val);
        pList->current->val = NULL; //clear pointer
        pList->current = pList->current->next;
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
    if (free == NULL) {
        return -1;
    }
    
    //add val
    setNode(free, pItem);

    //append, condition: empty list
    if (pList->n == 0) {
        pList->head = free; //tail is set later anyways
    } else {
        //last-> next = free
        setNext(pList->tail, free);
    }
    
    //edit List data
    pList->current = pList->tail = free;
    pList->n++;

    return 0;
}

int List_prepend(List* pList, void* pItem) {
    //find a free node
    Node* free = getFreeNode();

    //no nodes available
    if (free == NULL) {
        return -1;
    }

    //add val
    setNode(free, pItem);

    if (pList->n == 0) {
        pList->current = pList->head = free; //tail is set later anyways
    } else {
        //last-> next = free
        setNext(free, pList->head);
    }

    //edit list data
    pList->head = free;
    pList->n++;

    return 0;
}

//insert after current, make it the new current
int List_insert_after(List* pList, void* pItem) {
    //emmpty list, current > n: will handle 
    if (pList->n == 0 || (pList->current == NULL && pList->outOfBounds == LIST_OOB_END)) {
        //need to update current
        if (List_append(pList, pItem) == -1) {
            return -1;
        }
        //trivially will be 0
        pList->current = pList->tail; //might be done twice but it's fine

        return 0;
    }

    Node* free = getFreeNode();
    
    //no nodes available
    if (free == NULL) {
        return -1;
    }

    //add val
    setNode(free, pItem);

    //conditions:
    //current < 0 -> becomes head
    if (pList->current == NULL && pList->outOfBounds == LIST_OOB_START) {
        //we will make this the new head
        setNext(free, pList->head);
        pList->head = free;
    } else {
        //normal functionality -> if tail -> free->next becomes NULL
        setNext(free, pList->current->next);
        setNext(pList->current, free);
    }
    
    //iterate forward
    pList->current = free;
    pList->n++; //increment size
    return 0;
}

//make new item the current one
int List_insert_before(List* pList, void* pItem) {
    //conditions: empty
    if (pList->n == 0) {
        return List_append(pList, pItem); //also sets current
    }

    //inserting from outofbounds end -> becomes new tail
    if (pList->current == NULL && pList->outOfBounds == LIST_OOB_END) {
        return List_insert_after(pList, pItem); //same functionality
    }

    Node* free = getFreeNode();
    
    //no nodes available
    if (free == NULL) {
        return -1;
    }

    //add val
    setNode(free, pItem);

    //conditions:
    //current is before head (Out of bounds)
    if (pList->current == NULL && pList->outOfBounds == LIST_OOB_START) {
        pList->current = pList->head; //then we will have normal behaviour
    }

    //inserting before head -> becomes new head
    if (pList->current == pList->head) {
        pList->head = free;
    }

    //current = tail -> should be fine

    //link
    setNext(free, pList->current);
    //iterate back
    pList->current = free;
    pList->n++;
    return 0;
}

//removes current, to do: if we remove and it is now empty (size is 1)
void* List_remove(List* pList) {
    //conditions:
    //empty, before start, after start
    if (pList->current == NULL) {
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

    //current == tail: will go out of bounds end, tail will have to be tail-> prev
    if (pList->current == pList->tail) {
        pList->current = NULL;
        pList->outOfBounds = LIST_OOB_END;
        pList->tail = pList->tail->prev;
        goto end;
    }

    //given: !head, !tail, n > 1 means n >= 3
    //normal functionality
    setNext(pList->current->prev, pList->current->next);
    pList->current = pList->current->next;

    end: {
        //send free node back to pool
        sendFreeNodes(free, free);
        //update pList
        pList->n--;
    }

    return item;
}

//basically pop last, return NULL if empty
void* List_trim(List* pList) {
    //empty;
    if (pList->n == 0) {
        return NULL;
    }

    Node* temp = pList->current;
    pList->current = pList->tail;

    void* item = List_remove(pList);

    pList->current = temp;

    return item;
}

//can't use free -> still need those nodes
void List_concat(List* pList1, List* pList2) {
    //if pList1 is empty -> current = head pList2
    if (pList1->n == 0) {
        pList1->current = pList2->head;
    } else {
        //if list2 is empty -> will just point to NULL
        pList1->tail->next = pList2->head;
        //current stays the same as pList1
    }

    //combine lengths
    pList1->n += pList2->n;

    //set tail (if 2 is empty-> remain tail the same)
    if (pList2->n != 0) {
        pList1->tail = pList2->tail;
    }

    setEmpty(pList2); //clear out pList2
    emptyHead->next = pList2; //add it back to pool
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    //condition: empty
    if (pList->n == 0) {
        return NULL;
    }

    //current out of bounds
    if (pList->current == NULL) {
        //past end
        if (pList->outOfBounds == LIST_OOB_END) {
            return NULL;
        }
        //before start
        pList->current = pList->head;
    }

    //iterate until end
    while (true) {
        if (pList->current == NULL) {
            pList->outOfBounds = LIST_OOB_END;
            return NULL; //not found
        }

        //if match -> returns 1
        bool match = pComparator(pList->current->val, pComparisonArg);
        if (match) {
            return pList->current->val;
        }

        //else -> iterate
        pList->current = pList->current->next; //if NULL -> handled
    }

    //should not reach
    return NULL;
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
        nodes[i].val = NULL; //should be NULL -> trivial since our node is empty for now
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

//will link prev and next for 2 nodes
void setNext (Node* a, Node* b) {
    //safety
    if (a != NULL) {
        a->next = b;
    }
    
    if (b != NULL) {
        b->prev = a;
    }
}

void setNode(Node* node, void* pItem) {
    node->next = NULL; //break from emptyNodes, prev should be NULL (we are taking from head)
    node->val = pItem;
    node->empty = false; //set as occupied
}

//finds an unused head (list), none -> return NULL
List* getFreeHead () {
    //check if there are any heads left
    if (emptyHead == NULL) {
        return NULL;
    }

    //pop
    List *free = emptyHead;

    //if last -> should become NULL
    emptyHead = emptyHead -> next;

    return free;
}

void setEmpty (List* pList) {
    pList->current = NULL;
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

//debugger
void Print_all () {
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        printNode(&nodes[i]);
        printf("\n");
    }
    printf("\n");
}

void Print_list (List* pList) {
    Node* current = emptyNodes.head;
    while (current != NULL) {
        printNode(current);
        printf("\n");
        current = current->next;
    }
    printf("\n");
}
