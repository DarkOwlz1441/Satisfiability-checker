%{
    #include <stdio.h>

    #include "include/s_tree.h"
    #include "include/stack.h"
    #include "include/tableau.h"
    #include "include/exp_tab.h"

    int yylex();
    int yyerror();
    // int yydebug = 1; // debugger session is active

    typedef enum enum_reading_style{ONLY_ONE, MULTIPLE} reading_style;

    // sets the read_style for scanning multiple formulas at once
    reading_style read_style = MULTIPLE;
    stk_nd *formulae = NULL;
%}

/* below are the declaration of the types that will be present in the symbols. */

%union
{
    char *s;
    struct tree_node *n;
}

/* declaration of tokens below. The terminal symbols (tokens) are considered strings. */
%token <s> EOL
%token <s> ATOM
%token <s> LP RP
%token <s> AND OR IMP NOT
%token <s> EXIT
%token <s> OTHER

/* below are the nonterminal symbols, represented by nodes (subtrees) of a syntax tree. */

%type <n> proposition implication disjunction conjunction term

%%

/* the routines below are executed at the same time a syntax tree is built. */

proposition: /*nothing*/ {}
|   proposition EOL {fputs("> ", stdout);}
|   proposition EXIT EOL {
        if(read_style == MULTIPLE)
            return 0;
        else
            fputs("> ", stdout);
    }

|   proposition implication EOL {
        stk_push(&formulae, $2);
        
        if(read_style == ONLY_ONE)
            return 0;
        
        fputs("> ", stdout);
    }
;

/* an implication can be either a single disjunction or
   two or more disjunctions on an implication or several implications (IMP).
   note the recursion in this second case.
   also note how the order of the operands is different from
   the other binary operators. That's what causes the associativity
   directed to the right of the implication operator. */

implication: disjunction {$$ = $1;}
|    disjunction IMP implication {$$ = new_tree_node(">", $1, $3);}
;

/* a disjunction can be either a single conjunction or 
   two or more conjunctions in a disjunction or several disjunctions (OR).
   note the recursion in this second case. */

disjunction: conjunction {$$ = $1;}
|   disjunction OR conjunction {$$ = new_tree_node("#", $1, $3);}
;

/* a conjunction can be either a single term or
   two or more terms in a conjunction or several conjunctions (AND).
   note the recursion in this second case. */

conjunction: term {$$ = $1;}
|   conjunction AND term {$$ = new_tree_node("&", $1, $3);}
;

/* a term can be either an ATOM, or an parenthesised expression, 
or one of the above negated one or more times. (note the recursion in this last case) */

term: ATOM {$$ = new_tree_node($1, NULL, NULL);}
|   LP implication RP {$$ = $2;}
|   NOT term {$$ = new_tree_node("-", $2, NULL);}
;

%%

int main (int argc, char *argv[])
{
    puts("=-=-=-=-=-= SEQUENT SOLVER WITH ANALYTIC TABLEAUX =-=-=-=-=-=\n");
    puts("enter gamma formulae, ! to stop");
    fputs("> ", stdout);
    yyparse();
    read_style = ONLY_ONE;
    puts("enter inferred formula");
    fputs("> ", stdout);
    yyparse();
    puts("initial tableau forms test:");
    tab_nd *tableau = init_tableau(&formulae);
    // add_forms_alpha(&tableau, new_tab_node(new_branch_closer(), NULL, NULL), NULL);
    show_tableau_fitch(tableau);
    expand_tableau(&tableau, tableau);
    puts("after expanding:");
    show_tableau_fitch(tableau);
}

int yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);
}