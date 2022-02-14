/* 
    tableau structures and methods implementation.

    the tableau itself is structured as a binary tree.
    
    types of conjugated (marked) formulae and it's expansions:
    _____________________    _____________________
    |    α FORMULAS     |    |    β FORMULAS     |
    |___________________|    |___________________|
    | FORM. | EXPANSION |    | FORM. | EXPANSION |
    |_______|___________|    |_______|___________|
    | T_A∧B | T_A   T_B |    | F_A∧B | F_A   F_B |
    | F_A∨B | F_A   F_B |    | T_A∨B | T_A   T_B |
    | F_A→B | T_A   F_B |    | T_A→B | F_A   T_B |
    | T_¬A  |    F_A    |    |||||||||||||||||||||
    | F_¬A  |    T_A    |    
    |||||||||||||||||||||
*/

#pragma once

#include <stdlib.h>
#include "s_tree.h"
#include "stack.h"
#include "utils.h"

typedef enum mark{MARK_T, MARK_F, MARK_ERROR} mark;
typedef enum type{TYPE_ALPHA, TYPE_BETA, TYPE_ATOM, TYPE_ERROR} type;

typedef struct marked_formula
{
    mark mk;        // "mark" of the formula, a T or a F.
    type tp;        // "type" of the formula, either Alpha (α), Beta (β) or an simple Atom.
    tree_nd *form;  // the formula itself, passed as the root of the syntax tree.
} marked;

// AUXILIARY that analyses a formula and returns its type
type which_type(tree_nd *form, mark mk)
{
    if(!form || mk == MARK_ERROR)
        return TYPE_ERROR;

    if(!form->l && !form->r)
        return TYPE_ATOM;
    
    if(( mk == MARK_F && !strcmp(form->tok, "&") ) || 
       ( mk == MARK_T && (!strcmp(form->tok, "#") || !strcmp(form->tok, ">")) ))
        return TYPE_BETA;

    return TYPE_ALPHA;
}

marked new_marked_form(mark mk, tree_nd *form)
{
    marked new_marked;
    new_marked.form = form;
    new_marked.mk = mk;
    new_marked.tp = which_type(form, mk);

    return new_marked;
}

typedef struct tableau_node
{
    marked mk_form;         // the marked formula.
    struct tableau_node *l; // pointer to left.
    struct tableau_node *r; // pointer to right.
} tab_nd;

tab_nd *new_tab_node(marked mk_form, tab_nd *left, tab_nd *right)
{
    tab_nd *new_nd = (tab_nd *) malloc(sizeof(tab_nd));
    
    if(!new_nd)
        return NULL; // returns NULL if the allocation failed

    new_nd->l = left;
    new_nd->r = right;
    new_nd->mk_form = mk_form;

    return new_nd;
}



// function that builds the initial tableau, using the entered formulae,
// which were put in a stack.
tab_nd *init_tableau(stk_nd **formulae) // needs debugging
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

// auxiliary function to add formulae to all the branches, 
// in a alpha expansion
void add_forms_alpha(tab_nd **root, tab_nd *add1, tab_nd *add2)
{    
    if(!root)
    {
        fprintf(stderr, "ERROR on alpha expansion: NULL root reference.\n");
        return;
    }

    if(!add1 && !add2) // no formulas to add
        return;

    if(!*root) // if the tableau is empty, just add
    {
        if(add1)
        {
            *root = add1;
            add1->l = add2;
        }

        else
            *root = add2;
        
        return;
    }

    if((*root)->l)
        add_forms_alpha(&(*root)->l, add1, add2);
    
    if((*root)->r)
        add_forms_alpha(&(*root)->r, add1, add2);
    
    if(!(*root)->l && !(*root)->l)
    {
        if(add1)
        {
            (*root)->l = add1;
            add1->l = add2;
        }

        else
            (*root)->l = add2;
    }

}

// auxiliary function to add formulae to all the branches, 
// in a beta expansion
void add_forms_beta(tab_nd **root, tab_nd *add1, tab_nd *add2)
{
    if(!root)
    {
        fprintf(stderr, "ERROR on function add_forms_beta: NULL ROOT REFERENCE.\n");
        return;
    }

    if(!*root)
    {
        fprintf(stderr, "ERROR on function add_forms_beta: cannot do a beta expansion on a empty tableau.\n");
        return;
    }

    if(!add1 || !add2)
    {
        fprintf(stderr, "ERROR on function add_forms_beta: missing beta formulae.\n");
        return;
    }

    if((*root)->l)
        add_forms_beta(&(*root)->l, add1, add2);

    else
    {
        (*root)->l = add1;
        (*root)->r = add2;
        return;
    }

    if((*root)->r)
        add_forms_beta(&(*root)->r, add1, add2);
}

// handle cases for an alpha expansion
void alpha_exp(tab_nd **root, tab_nd *form)
{
    if(!root || !form) 
        return;
    
    tree_nd *formula = form->mk_form.form;
    
    tab_nd *form1 = NULL;
    tab_nd *form2 = NULL;

    // T_¬A   |->  F_A
    if(!strcmp(formula->tok, "-") && form->mk_form.mk == MARK_T)
    {
        form1 = new_tab_node(new_marked_form(MARK_F, form->mk_form.form->l), NULL, NULL);
    }
    // F_¬A   |->  T_A
    else if(!strcmp(formula->tok, "-") && form->mk_form.mk == MARK_F)
    {
        form1 = new_tab_node(new_marked_form(MARK_T, form->mk_form.form->l), NULL, NULL);
    }
    // T_A∧B  |->  T_A   T_B
    if(!strcmp(formula->tok, "&") && form->mk_form.mk == MARK_T)
    {
        form1 = new_tab_node(new_marked_form(MARK_T, form->mk_form.form->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_T, form->mk_form.form->r), NULL, NULL);
    }
    // F_A∨B  |->  F_A   F_B
    else if(!strcmp(formula->tok, "#") && form->mk_form.mk == MARK_F)
    {
        form1 = new_tab_node(new_marked_form(MARK_F, form->mk_form.form->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_F, form->mk_form.form->r), NULL, NULL);
    }
    // F_A→B | T_A   F_B // TODO: Handle case where the formulas are the same, closing the branch
    else
    {
        form1 = new_tab_node(new_marked_form(MARK_F, form->mk_form.form->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_F, form->mk_form.form->r), NULL, NULL);
    }

    add_forms_alpha(root, form1, form2);
}

// handle cases for a beta expansion
void beta_exp(tab_nd **root, tab_nd *form)
{
    // TODO: beta expansion cases
}

// expands a formula
void expand_form(tab_nd **root, tab_nd *form)
{
    if(!form)
        return;

    // alpha expansions
    if(form->mk_form.tp == TYPE_ALPHA)
    {
        alpha_exp(root, form);        
    }
    
    // beta expansions
    else if(form->mk_form.tp == TYPE_BETA)
    {
        beta_exp(root, form);
    }
}

// this is just printing the formulas of the tableau.
// not organized.
void show_tableau(tab_nd *tableau)
{
    if(!tableau)
        return;

    if(tableau->mk_form.mk == MARK_T)
        fputs("T", stdout);
    else
        fputs("F", stdout);

    show_form(tableau->mk_form.form);
    puts("");

    show_tableau(tableau->l);
    show_tableau(tableau->r);
}

/*
    show tableau in a "fitch" way, organized in "boxes"
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

    if(root->mk_form.mk == MARK_T)
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