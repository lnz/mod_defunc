/*
Copyright (c) 2009, Matthias Lanzinger
All rights reserved.

Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "httpd.h"
#include "http_config.h"
#include "http_main.h"
#include "apr_strings.h"

#include <stdio.h>

#define DEFAULT_VAL		"1" /* value used for calculations with valueless defines */
#define MAX_LINE_LEN	1024
#define MAX_NUM_LEN		20


extern void* yy_scan_string(char* str);
extern int yyparse(void);

module AP_MODULE_DECLARE_DATA defunc_module;

typedef struct{
	apr_pool_t* defunc_pool;
	apr_table_t* mod_defunc_variables;
}mod_defunc_cfg;

mod_defunc_cfg* cfg;

int mod_defunc_getdefineval(char* define)
{
	const char* val = apr_table_get(cfg->mod_defunc_variables, define);
//	printf("Getting val for %s\n",define);
	if(val == NULL)
		return 0;
	else{
//		printf("Defineval %s = %d\n",define, strtol(val,NULL,0));
		return strtol(val, NULL, 0);
	}
}

static void *mod_defunc_config(apr_pool_t* p, server_rec* s)
{
	cfg = apr_pcalloc(p, sizeof(mod_defunc_cfg));
	if(cfg == NULL)
		return NULL;
	
	if( apr_pool_create(&cfg->defunc_pool, p) )
		return NULL;
		
	cfg->mod_defunc_variables = apr_table_make(cfg->defunc_pool, 0);
	if(cfg->mod_defunc_variables == NULL)
		return NULL;
	
	return (void*)cfg;
}

static const char* mod_defunc_define(cmd_parms* cmd, void* dummy, char* define, char* val)
{
	char** newdef;
	char tmp[MAX_NUM_LEN];
	int parseval;
	
	if(cfg==NULL){
		return "Couldn't get config... weird\n";
	}
	
	if( !ap_exists_config_define(define) ){
		newdef = (char **)apr_array_push(ap_server_config_defines);
    	*newdef = apr_pstrdup(cfg->defunc_pool, define);
    }
    
	if(val == NULL){
		apr_table_set(cfg->mod_defunc_variables, define, DEFAULT_VAL);
	}else{
		yy_scan_string(val);
		parseval = yyparse();
		snprintf(tmp, MAX_NUM_LEN, "%d", parseval);
		apr_table_set(cfg->mod_defunc_variables, define, tmp);
	}
	
	return NULL;
}

static const char* mod_defunc_ifdef(cmd_parms* cmd, void* dummy, char* arg)
{
	char line[MAX_LINE_LEN];
	char* endp;
	int nestinglvl = 0;

	endp = strrchr(arg,'>');
	if(endp != NULL)
		*endp = '\0';
	
//	printf("trying: %s\n", arg);
	yy_scan_string(arg);
	
	if( !yyparse() ){
		nestinglvl = 1;
		while(nestinglvl && !ap_cfg_getline(line, MAX_LINE_LEN, cmd->config_file)){
			if( !strncmp(line,"<IfDef", 6) )
				nestinglvl+=1;
			if( !strncmp(line,"</IfDef>", 8) )
				nestinglvl-=1;
		}
		if(nestinglvl)
			return "congrats, a nesting error -.-"; 
	}
		
	return NULL;
}

static const char* mod_defunc_ifdef_end(cmd_parms* cmd, void* dummy)
{
	return NULL;
}

static const char* mod_defunc_undef(cmd_parms* cmd, void* dummy, char* undef)
{
	int i, last;
	char** defines;


	defines = (char**)ap_server_config_defines->elts;	
	last = ap_server_config_defines->nelts-1;
	
	for(i=0; i < ap_server_config_defines->nalloc; ++i){
		if( defines[i]!=NULL && !strcmp(defines[i], undef) ){
		
			if(i!=last){
				defines[i]	  = defines[last];
				defines[last] = NULL;
				last-=1;
			}else{
				defines[i]=NULL;
			}
			
			ap_server_config_defines->nelts -= 1;
		}
	}
	return NULL;
}

static command_rec mod_defunc_cmds[] = {
	{ "Define", mod_defunc_define, NULL, EXEC_ON_READ|OR_ALL, TAKE12, "define stuff"},
	{ "<IfDef", mod_defunc_ifdef, NULL,  EXEC_ON_READ|OR_ALL, RAW_ARGS, "Start of ifdef block"},
	{ "</IfDef>", mod_defunc_ifdef_end, NULL, EXEC_ON_READ|OR_ALL, NO_ARGS, "dummy"},
	{ "UnDef", mod_defunc_undef, NULL, EXEC_ON_READ|OR_ALL, TAKE1, "undefine value"},
	{NULL}
};

module AP_MODULE_DECLARE_DATA defunc_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                     /* create per-directory config structure */
    NULL,                     /* merge per-directory config structures */
    mod_defunc_config, 		  /* create per-server config structure */
    NULL,                     /* merge per-server config structures */
    mod_defunc_cmds,          /* command handlers */
    NULL			          /* register hooks */
};




