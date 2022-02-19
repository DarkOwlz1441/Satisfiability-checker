/* 
    here are tableau structure and methods implementation.
    the tableau itself is structured as a binary tree.
*/

#pragma once

#include <stdlib.h>
#include "s_tree.h"
#include "marked.h"
#include "stack.h"
#include "utils.h"

/*
    the tableau node structure
*/
typedef struct tableau_node
{
    marked mk_form;         // the marked formula.
    struct tableau_node *l; // pointer to left.
    struct tableau_node *r; // pointer to right.
}
tab_nd;
/*
    Function that generates and allocates a new tableau node
*/
tab_nd *new_tab_node(marked mk_form, tab_nd *left, tab_nd *right) // TODO: handle NULL case in other functions
{
    tab_nd *new_nd = (tab_nd *) malloc(sizeof(tab_nd));
    
    if(!new_nd)
        return NULL; // returns NULL if the allocation failed

    new_nd->l = left;
    new_nd->r = right;
    new_nd->mk_form = mk_form;

    return new_nd;
}
/*
    returns an exact copy of a given tableau
*/
tab_nd *clone_tableau(const tab_nd *tab_to_copy)
{
    if(!tab_to_copy)
        return NULL;
    
    return new_tab_node(tab_to_copy->mk_form,
                        clone_tableau(tab_to_copy->l),
                        clone_tableau(tab_to_copy->r));
}
/*
    function that builds the initial tableau, using the entered formulae,
    which were put in a stack.
*/
tab_nd *init_tableau(stk_nd **formulae)
{
    if(!formulae || !*formulae)
    {
        fprintf(stderr, "ERROR on init tableau: stack empty\n");
        return NULL;
    }
    
    // pops the first formula from the stack and adds to the tableau.
    // that is the inferred formula of the sequent.
    tab_nd *new_tab = new_tab_node(new_marked_form(MARK_F, stk_pop(formulae)), NULL, NULL);
    
    tab_nd *aux = new_tab;

    // pops and add the rest of the formulas to the tableau
    // those are the formulae that are used to infer in the sequent.
    while(*formulae)
    {
        aux->l = new_tab_node(new_marked_form(MARK_T, stk_pop(formulae)), NULL, NULL);
        aux = aux->l;
    }
    
    return new_tab;
}
/*
    this is just printing the formulas of the tableau.
    not organized.
*/
void __show_tableau(tab_nd *root)
{
    if(!root)
        return;

    if(root->mk_form.mk == NO_MARK) // represents a "branch-closing" formula (x)
        fputs("*", stdout);
    if(root->mk_form.mk == MARK_T)
        fputs("T", stdout);
    else
        fputs("F", stdout);

    show_form(root->mk_form.form);
    puts("");

    __show_tableau(root->l);
    __show_tableau(root->r);
}
void show_tableau(tab_nd *root)
{
    if(!root)
        fprintf(stderr, "EMPTY TABLEAU\n");
    else
        __show_tableau(root);
}
/*
    show tableau in a "fitch" stylized way, organized in "sections"
    it gives us the possibility of visualizing the branches
    better, in a linear way. It is an easier to display
    alternative to the original tree format.
    __________________________________________________________
    | tableau in regular format  |  tableau in FITCH format  |
    |____________________________|___________________________|
    |           F_A∧B            | F_A∧B                     |
    |           T_A∨B            | T_A∨B                     |
    |           T_¬A             | T_¬A                      |
    |        /         \         | | F_B                     |
    |      F_A         F_B       | | | T_B                   |
    |     /   \       /   \      | | | x                     |
    |   T_A   T_B   T_A    T_B   | | T_A                     |
    |    x    F_A   F_A     x    | | F_A                     |
    |                x           | | x                       |
    |                            | F_A                       |
    |                            | | T_B                     |
    |                            | | F_A                     |
    |                            | F_A                       |
    |                            | T_A                       |
    |                            | x                         |
    |____________________________|___________________________|
*/
void __show_tableau_fitch(tab_nd *root, unsigned counter)
{
    if(!root)
        return;
    
    for(int i = 0; i < counter; i++)
    {
        fputs("| ", stdout);
    }

    if(root->mk_form.mk == NO_MARK) // represents a "branch-closing" formula (x)
        fputs("*", stdout);
    else if(root->mk_form.mk == MARK_T)
        fputs("T", stdout);
    else
        fputs("F", stdout);
    
    show_form(root->mk_form.form);
    puts("");
    
    __show_tableau_fitch(root->r, counter + 1);
    __show_tableau_fitch(root->l, counter);
}
void show_tableau_fitch(tab_nd *root)
{
    if(!root)
        fprintf(stderr, "EMPTY TABLEAU\n");
    else
        __show_tableau_fitch(root, 0);
}