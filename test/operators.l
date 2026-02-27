%{
#include <stdio.h>
int count = 0;
%}

DIGIT [0-9]
LETTER [a-zA-Z]
ID {LETTER}({LETTER}|{DIGIT})*

%%
{ID}        { count++; printf("ID: %s\n", yytext); }
{DIGIT}+    printf("NUM: %s\n", yytext);
"+" |
"-" |
"*" |
"/"         printf("OP: %s\n", yytext);
"\n"        { printf("NEWLINE\n"); }
[[:blank:]]
.           printf("OTHER: %c\n", *yytext);
%%

int main() {
    yylex();
    printf("Total: %d\n", count);
    return 0;
}