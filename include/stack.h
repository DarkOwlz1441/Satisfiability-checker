/* stack data structure to store multiple formulae */

#pragma once

#include <stdlib.h>
#include "s_tree.h"

typedef struct stack_node
{
    tree_nd *formula;
    struct stack_node *next;
} stk_nd;

stk_nd *new_stk_nd(tree_nd *formula)
{
    stk_nd *new_node = (stk_nd *) malloc(sizeof(stk_nd));
    
    if(!new_node)
        return NULL;

    new_node->next = NULL;
    new_node->formula = formula;

    return new_node;
}

void stk_push(stk_nd **head, tree_nd *formula)
{
    stk_nd* new_node = new_stk_nd(formula);
    
    new_node->next = *head;
    *head = new_node;
}

// pops from the stack and returns the popped formula
tree_nd *stk_pop(stk_nd **head) // TODO: handle deep pop, that is, pop the stack nodes 
{                               // AND the formulas inside (trees)
    if(!head || !*head)
        return NULL;

    tree_nd *popped_val = (*head)->formula; 
    stk_nd *garbage = *head;
    *head = (*head)->next;
    free(garbage);
    
    return popped_val;
}

void stk_pop_all(stk_nd **head)
{
    if(!head)
        return;

    while(*head)
        stk_pop(head);
}

void stk_show(stk_nd *head)
{
    if(!head)
        return;

    show_form(head->formula);
    puts("");
    stk_show(head->next);
}