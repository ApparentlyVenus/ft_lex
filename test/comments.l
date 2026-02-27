%{
/* Multi
   line
   comment */
int x = 0;
%}

/* Header comment */
D [0-9]

%%
{D}+    printf("NUM");
%%