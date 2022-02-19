/*
    tableau expansion functions

    they were many enough to create a single
    file just for them.
*/

#pragma once

#include <stdlib.h>
#include "utils.h"
#include "tableau.h"

/*
    auxiliary function to add formulae to all the branches, 
    in a alpha expansion
*/
void add_forms_alpha(tab_nd **root, tab_nd *add_root,  tab_nd *add)
{    
    if(!root)
    {
        fprintf(stderr, "ERROR on alpha expansion: NULL root reference.\n");
        return;
    }

    if(!add) // no formulae to add
        return;

    if(!*root && !add_root) // if the tableau is empty, just add
    {
        tab_nd *add_cpy = clone_tableau(add);
        *root = add_cpy;
    }

    else if(!*root && add_root)
    {
        fprintf(stderr, "ERROR on alpha expansion: add_root cannot belong to a empty tableau.\n");
        return;
    }

    else if(root && !add_root)
    {
        fprintf(stderr, "ERROR on alpha expansion: empty add_root on a non-empty tableau.\n");
        return;
    }

    else if(!add_root->l && !add_root->l)
    {
        // branch is closed, does not add.
        if(add_root->mk_form.tp == CLOSE_BRANCH)
            return;

        tab_nd *add_cpy = clone_tableau(add);
        add_root->l = add_cpy;
    }

    if(add_root->l)
        add_forms_alpha(root, add_root->l, add);
    
    if(add_root->r)
        add_forms_alpha(root, add_root->r, add);
}
/*
    auxiliary function to add formulae to all the branches, 
    in a beta expansion
*/
void add_forms_beta(tab_nd **root, tab_nd *add1, tab_nd *add2) // TODO: refatorate this
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

    if(!(*root)->l && !(*root)->r)
    {
        // adds only if the branch is not closed
        if((*root)->mk_form.tp != CLOSE_BRANCH)
        {
            (*root)->l = add1;
            (*root)->r = add2;
        }
        return;
    }

    if((*root)->r)
        add_forms_beta(&(*root)->r, add1, add2);
}
/*
    handle cases for an alpha expansion
*/
void alpha_exp(tab_nd **root, tab_nd *form)
{
    if(!root || !form) 
        return;
    
    tree_nd *formula = form->mk_form.form;
    
    tab_nd *add_form = NULL;

    // T_¬A   |->  F_A
    if(!strcmp(formula->tok, "-") && form->mk_form.mk == MARK_T)
    {
        add_form = new_tab_node(new_marked_form(MARK_F, formula->l), NULL, NULL);
    }
    // F_¬A   |->  T_A
    else if(!strcmp(formula->tok, "-") && form->mk_form.mk == MARK_F)
    {
        add_form = new_tab_node(new_marked_form(MARK_T, formula->l), NULL, NULL);
    }
    // T_A∧B  |->  T_A   T_B
    else if(!strcmp(formula->tok, "&"))
    {
        tab_nd *aux = new_tab_node(new_marked_form(MARK_T, formula->r), NULL, NULL);
        add_form = new_tab_node(new_marked_form(MARK_T, formula->l), aux, NULL);
    }
    // F_A∨B  |->  F_A   F_B
    else if(!strcmp(formula->tok, "#"))
    {
        tab_nd *aux = new_tab_node(new_marked_form(MARK_F, formula->r), NULL, NULL);
        add_form = new_tab_node(new_marked_form(MARK_F, formula->l), aux, NULL);
    }
    // F_A→B | T_A   F_B 
    else
    {
        tab_nd *aux = new_tab_node(new_marked_form(MARK_T, formula->r), NULL, NULL);
        add_form = new_tab_node(new_marked_form(MARK_F, formula->l), aux, NULL);
        
        if(equal_forms(formula->l, formula->r)) // Handle case where the formulas // TODO: debug this case
        {                                      // are the same, closing the branch
            aux->l = new_tab_node(new_branch_closer(), NULL, NULL);
        }
    }

    add_forms_alpha(root, form, add_form);
}
/*
    handle cases for a beta expansion
*/
void beta_exp(tab_nd **root, tab_nd *form) // refatorate
{
    tree_nd *formula = form->mk_form.form;

    tab_nd *form1 = NULL;
    tab_nd *form2 = NULL;

    // F_A∧B  |->  F_A   F_B
    if(!strcmp(formula->tok, "&"))
    {
        form1 = new_tab_node(new_marked_form(MARK_F, formula->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_F, formula->r), NULL, NULL);
    }
    // T_A∨B | T_A   T_B
    else if(!strcmp(formula->tok, "#"))
    {
        form1 = new_tab_node(new_marked_form(MARK_T, formula->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_T, formula->r), NULL, NULL);
    }
    // T_A→B | F_A   T_B
    else
    {
        form1 = new_tab_node(new_marked_form(MARK_F, formula->l), NULL, NULL);
        form2 = new_tab_node(new_marked_form(MARK_T, formula->r), NULL, NULL);
    }

    add_forms_beta(root, form1, form2);
}
/*
    expands a formula
*/
void expand_form(tab_nd **tableau, tab_nd *form) // TODO: handle add_forms changes to add only in
{                             // proper subtree
    if(!tableau || !*tableau || !form)
        return;

    // alpha expansions
    if(form->mk_form.tp == TYPE_ALPHA)
    {
        alpha_exp(tableau, form);        
    }
    
    // beta expansions
    else if(form->mk_form.tp == TYPE_BETA)
    {
        beta_exp(tableau, form);
    }
}
/*
    function for checking either a tableau is closed or not. If yes, returns TRUE.
    if no, returns FALSE.
*/
bool is_tableau_closed(tab_nd *root)
{
    if(root->l && root->r)
        return is_tableau_closed(root->l) && is_tableau_closed(root->r);

    if(root->l)
        return is_tableau_closed(root->l);
    
    if(root->r)
        return is_tableau_closed(root->r);
    
    return root->mk_form.tp == CLOSE_BRANCH ? TRUE : FALSE;
}
/*
    expands all tableau formulas, one by one
*/
void expand_tableau(tab_nd **tableau, tab_nd *formula)
{
    if(!tableau || !*tableau || !formula)
        return;
    
    // for debugging //
    // if(formula->mk_form.mk == NO_MARK) // represents a "branch-closing" formula (x)
    //     fputs("*", stdout);
    // else if(formula->mk_form.mk == MARK_T)
    //     fputs("T", stdout);
    // else
    //     fputs("F", stdout);
    
    // show_form(formula->mk_form.form);
    // puts("");
    show_tableau_fitch(*tableau);
    getchar();
    ////////////////////

    expand_form(tableau, formula);

    puts("========================="); // delete this after debugging 

    expand_tableau(tableau, formula->l);
    expand_tableau(tableau, formula->r);
}
/*
    expand all alpha formulas, one by one
*/
void expand_all_alpha(tab_nd **tableau, tab_nd *formula)
{
    if(!tableau || !*tableau || formula)
        return;

    if(formula->mk_form.tp == TYPE_ALPHA)
        alpha_exp(tableau, formula);
    
    expand_all_alpha(tableau, formula->l);
    expand_all_alpha(tableau, formula->r);
}
/*
    expands all beta formulas, one by one
*/
void expand_all_beta(tab_nd **tableau, tab_nd *formula)
{
    if(!tableau || !*tableau || formula)
        return;

    if(formula->mk_form.tp == TYPE_BETA)
        beta_exp(tableau, formula);
    
    expand_all_beta(tableau, formula->l);
    expand_all_beta(tableau, formula->r);
}