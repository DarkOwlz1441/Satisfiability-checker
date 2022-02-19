/*
    marked formulas structure and methods implementation.

    marked formulas, or conjugated formulae, are a special
    kind of formula used on analytic tableaux. They are very
    alike regular propositional formulae, but they can be
    marked as T or F (representing True or False) on the
    tableau.
    
    Once in tableau, they can also be expanded.

    Marked formulae can only be expanded one time.

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

typedef enum mark{MARK_T, MARK_F, MARK_ERROR, NO_MARK} mark;
typedef enum type{TYPE_ALPHA, TYPE_BETA, TYPE_ATOM, TYPE_ERROR, CLOSE_BRANCH} type;

typedef struct marked_formula
{
    mark mk;        // "mark" of the formula, a T or a F.
    type tp;        // "type" of the formula, either Alpha (α), Beta (β) or an simple Atom.
    tree_nd *form;  // the formula itself, passed as the root of the syntax tree.
}
marked;
/*
    auxiliary function that analyses a formula and returns its type
*/
type which_type(tree_nd *form, mark mk)
{
    if(!form)       // a tableau formula without formula will be 
        return CLOSE_BRANCH;  // that "x" that closes the branch

    if(mk == MARK_ERROR)
        return TYPE_ERROR;

    if(!form->l && !form->r)
        return TYPE_ATOM;
    
    if(( mk == MARK_F && !strcmp(form->tok, "&") ) || 
       ( mk == MARK_T && (!strcmp(form->tok, "#") || !strcmp(form->tok, ">")) ))
        return TYPE_BETA;

    return TYPE_ALPHA;
}
/*
    function that creates a new marked formula and returns it
*/
marked new_marked_form(mark mk, tree_nd *form)
{
    marked new_marked;
    new_marked.form = form;
    new_marked.mk = mk;
    new_marked.tp = which_type(form, mk);

    return new_marked;
}
/*
    function that creates a special formula used to close a tableau branch
*/
marked new_branch_closer()
{
    return new_marked_form(NO_MARK, NULL);
}