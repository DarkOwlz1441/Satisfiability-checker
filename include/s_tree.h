/* this is the code to a cool syntax tree */

#define OMIT_PAR 0 // set this constant to zero to omit unnecessary parenthesis when printing formulas.
                   // set to nonzero to omit only external parenthesis.
#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

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

//A modified "in order" funtion. Used for printing the syntax tree as a parentesized expression
void in_order(tree_nd *root)
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
        int verify = (root->l->l && root->l->r
                     && (strcmp(root->tok, root->l->tok) || !strcmp(root->tok, ">")
                     || OMIT_PAR)) ? 1: 0;

        if (verify)
            putchar('(');
        
        in_order(root->l);

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
        int verify = (root->r->l && root->r->r && (strcmp(root->tok, root->r->tok)
                     || OMIT_PAR)) ? 1 : 0;
        if (verify)
            putchar('(');

        in_order(root->r);

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
