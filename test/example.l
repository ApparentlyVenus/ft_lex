%{
#include <stdio.h>
#include <stdlib.h>

int line_count = 0;
int word_count = 0;
%}

DIGIT   [0-9]
LETTER  [a-zA-Z]
ID      {LETTER}({LETTER}|{DIGIT})*

%%

{ID}            { word_count++; printf("IDENTIFIER: %s\n", yytext); }
{DIGIT}+        { printf("NUMBER: %s\n", yytext); }
"+"             printf("PLUS\n");
"-"             printf("MINUS\n");
"*"             printf("MULTIPLY\n");
"/"             printf("DIVIDE\n");
"="             printf("ASSIGN\n");
"\n"            { line_count++; }
[[:blank:]]     /* ignore whitespace */
.               { printf("UNKNOWN: %c\n", *yytext); }

%%

int main(int argc, char **argv) {
    if (argc > 1)
        yyin = fopen(argv[1], "r");
    yylex();
    printf("Lines: %d\n", line_count);
    printf("Words: %d\n", word_count);
    return 0;
}

int yywrap(void) {
    return 1;
}