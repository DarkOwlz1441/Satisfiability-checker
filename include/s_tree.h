/* this is the code to a cool syntax tree */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

#define ALL_PAR FALSE // set this to true for using all parentesis possible (except external ones)
                      // set to false for omiting most unnecessary parentesis

//Structure responsible for implementing tree nodes
typedef struct tree_node
{
    const char *tok;
    struct tree_node *l;
    struct tree_node *r;
} tree_nd;

//Function that generates and allocates a new node
tree_nd *new_tree_node(const char* tok, tree_nd *l, tree_nd *r)
{   
    tree_nd *new_nd = (tree_nd *) malloc(sizeof(tree_nd));

    if(!new_nd) return NULL;

    new_nd->tok = custom_strdup(tok);
    new_nd->l   = l;
    new_nd->r   = r;

    return new_nd;
}
/*
    A modified "in order" funtion.
    Used for printing the parentesized formula from the syntax tree
*/
void show_form(tree_nd *root)
{
    if (!root)
        return;

    if (!root->l && !root->r)
    {
        printf("%s", root->tok);
        return;
    }
    
    if (!strcmp(root->tok, "-"))
    {
        printf("%s", root->tok);
    }
    
    if (root->l)
    {
        bool verify = (root->l->l && root->l->r
                     && (strcmp(root->tok, root->l->tok) || !strcmp(root->tok, ">")
                     || ALL_PAR)) ? 1: 0;

        if (verify)
            putchar('(');
        
        show_form(root->l);

        if (verify)
            putchar(')');
    }

    if (strstr("&#>", root->tok))
    {
        putchar(' ');
        printf("%s", root->tok);
        putchar(' ');
    }

    if (root->r)
    {
        bool verify = (root->r->l && root->r->r && (strcmp(root->tok, root->r->tok)
                     || ALL_PAR)) ? 1 : 0;
        if (verify)
            putchar('(');

        show_form(root->r);

        if (verify)
            putchar(')');
    }
}

void free_tree(tree_nd **ref_root)
{
    if(!ref_root || !*ref_root)
        return;

    tree_nd *root = *ref_root;

    if(root->l)
    {
        tree_nd **aux = &root->l;
        root->l = NULL;
        free_tree(aux);
    }

    if(root->l)
    {
        tree_nd **aux = &root->l;
        root->l = NULL;
        free_tree(aux);
    }

    free(root);
}

//  Function that clones a tree formed by the node structure
tree_nd* clone_tree(tree_nd *root)
{
    if (!root)
        return NULL;

    return new_tree_node(root->tok, clone_tree(root->l), clone_tree(root->r));
}

bool equal_forms(tree_nd *form1, tree_nd *form2)
{
    // returns true if the formulas are the same object in memory,
    // or if both are NULL
    if(form1 == form2)
        return TRUE;

    // returns false if just one of them is NULL
        // (the only way to make the conditional below be evaluated as true is
        // if just one of them is NULL. If both were NULL,
        // the previous conditional would be evaluated and the rest of
        // the code would never happen.)
    if(!form1 || !form2)
        return FALSE;

    // if the current nodes are not the same value, returns false
    if(strcmp(form1->tok, form2->tok))
        return FALSE;
    
    // if none of the above happened, we can suppose the current nodes
    // are equal. Thus we can do the recursive calls to left and right, 
    // that is, if both subtrees of both formulas are equal, this is
    // returned as true.
    return TRUE && equal_forms(form1->l, form2->l) && equal_forms(form1->r, form2->r);
}