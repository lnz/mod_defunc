%{
#include <stdlib.h>
#include "y.tab.h"
extern int yylval;
extern int mod_defunc_getdefineval(char* define);
%}

NUMBER		(0x)?[0-9]+
DEFINE		[_a-zA-Z][_a-zA-Z0-9]*
%%
{NUMBER}	{
				yylval = strtol(yytext, NULL, 0); 
				return TOKNUM;
			}
			
{DEFINE}		{
				yylval = mod_defunc_getdefineval(yytext);
				return TOKNUM;
			}
			
==			return EQUALS;
!=			return NEQUALS;
&&			return LOGAND;
\|\|		return LOGOR;
\<=			return LSTHAN;
>=			return GTTHAN;

[ \t\n]+		;
.			return (int)yytext[0];



