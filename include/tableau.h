/* 
    tableau structures and methods implementation.
    
    
*/

#include <stdlib.h>
#include "s_tree.h"
#include "queue.h"
#include "utils.h"

enum mark{MARK_T, MARK_F};

typedef struct marked_formula
{
    mark mk;        // "mark" of the formula, a T or a F.
    tree_nd *form;  // the formula itself, passed as the root of the syntax tree.
} marked;

marked new_marked_form(mark mk, tree_nd *form)
{
    marked new_nd;
    new_nd.form = form;
    new_nd.mk = mk;

    return new_nd;
}

typedef struct tableau_node
{
    marked mk_form;          // the marked formula.
    struct tableau_node *l;  // pointer to left.
    struct tableau_node *r;  // pointer to right.
} tab_nd;



tab_nd *new_tab_node(marked mk_form, tab_nd *left, tab_nd *right)
{
    tab_nd *new_nd = (tab_nd *) malloc(sizeof(tab_nd));
    
    if(!new_nd)
        return NULL; // returns NULL if the allocation failed

    new_nd->l = left;
    new_nd->r = right;
    new_nd->mk_form = mk_form;
}