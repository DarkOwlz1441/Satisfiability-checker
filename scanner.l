%{
    #include "logic2.tab.h"
    char *custom_strdup(const char *);
%}

/* the section below has the definition of some routines,
   responsible for recognizing the tokens. */
%%

[a-z] { yylval.s = custom_strdup(yytext); return ATOM;  }
"-"   { return NOT; }
"&"   { return AND; }
"#"   { return OR; }
">"   { return IMP; }
"("   { return LP; }
")"   { return RP; }
\n    { return EOL; }
[ \t] { /*do nothing*/ } /* this is for ignoring whitespace */
.     { return OTHER; }

%%

/* this function is defined here for avoiding use of flex library -lfl
   when compiling (compatibility issues). */
int yywrap()
{
	return 1;
}