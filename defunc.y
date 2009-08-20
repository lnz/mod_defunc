%{
	#include <stdio.h>
	#include <stdlib.h>
	
	void yyerror(const char* str)
	{
		fprintf(stderr, "error %s\n",str);
	}
	
	int yywrap()
	{
		return 1;
	}
	
%}

%start stmt

%token TOKNUM EQUALS NEQUALS LSTHAN GTTHAN LOGAND LOGOR

%left LOGAND LOGOR
%left '!'
%left '<' '>' LSTHAN GTTHAN EQUALS NEQUALS
%left '|' '&' '^'
%left '+' '-'
%left '*' '/' '%'

%%

stmt:
	|
	expr
	{
		return $1;
	}
	;

	
expr:
	'(' expr ')' 
	{
		$$ = $2;
	}
	|
	expr LOGAND expr {
		$$ = (int)($1 && $3);
	}
	|
	expr LOGOR expr {
		$$ = (int)($1 || $3);
	}
	|
	'!' expr {
		$$ = (int)(!$2);
	}
	|
	expr '>' expr {
		$$ = (int)($1 > $3);
	}
	|
	expr '<' expr {
		$$ = (int)($1 < $3);
	}
	|
	expr GTTHAN expr {
		$$ = (int)($1 >= $3);
	}
	|
	expr LSTHAN expr {
		$$ = (int)($1 <= $3);
	}
	|
	expr EQUALS expr {
		$$ = (int)($1 == $3);
	}
	|
	expr NEQUALS expr {
		$$ = (int)($1 != $3);
	}
	|
	expr '*' expr {
		$$ = $1 * $3;
	}
	|
	expr '/' expr {
		$$ = $1 / $3;
	}
	|
	expr '%' expr {
		$$ = $1 % $3;
	}
	|
	expr '+' expr {
		$$ = $1 + $3;
	}
	|
	expr '-' expr {
		$$ = $1 - $3;
	}
	|
	expr '&' expr {
		$$ = $1 & $3;
	}
	|
	expr '|' expr {
		$$ = $1 | $3;
	}
	|
	expr '^' expr {
	    $$ = $1 ^ $3;
	}
	|
	'-' expr %prec '*' {
		$$ = -$2;
	}
	|
	TOKNUM
	;


%%

