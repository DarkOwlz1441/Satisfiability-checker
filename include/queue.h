#include <stdlib.h>
#include "s_tree.h"


typedef struct queue_node
{
    tree_nd *formula;
    struct queue_node *next;
} que_nd;

typedef struct queue
{
    que_nd *head;
    que_nd *tail;
} queue;

que_nd *new_nd_que(tree_nd *formula)
{
    que_nd *new_node = (que_nd *) malloc(sizeof(que_nd));
    
    if(!new_node)
        return NULL;

    new_node->next = NULL;
    new_node->formula = formula;

    return new_node;
}

queue new_queue()
{
    queue new_queue;
    new_queue.head = NULL;
    new_queue.tail = NULL;
}

void que_push(queue q, tree_nd *formula)
{
    que_nd* new_node = new_nd_que(formula);
    
    if(q.tail)
    {
        q.tail->next = new_node;    
        q.tail = new_node;
    }

    else
        q.head = q.tail = new_node;
}

void que_pop(queue q)
{
    if(q.head == NULL)
        return;
    
    que_nd *garbage = q.head;

    if(q.head == q.tail)
        q.head = q.tail = NULL;

    else
        q.head = q.head->next;

    free(garbage);
}

void que_pop_all(queue q)
{
    while(q.head != NULL)
        que_pop(q);
}