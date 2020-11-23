#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef OPT_H
#define OPT_H

typedef struct zOptval zOptval;
struct zOptval {
	#if 0
	union { int32_t n; char *s; char c; void *v; } value;
	#endif
	int n; 
	char *s; 
	char c; 
	void *v; 
};

typedef struct zOption zOption;
struct zOption {
	const char *sht;
	const char *lng;
	const char *description;
	char  type; /*n, s or c*/	
	zOptval v;
	_Bool (*callback)(char **av, zOptval *v, char *err);
	_Bool set;  /*If set is not bool, it can define the order of demos*/
	_Bool sentinel;
 #ifndef ERR_H
  int error;
	#ifndef ERRV_H
	//char  errmsg[ ERRV_LENGTH ];
	char  *errmsg;
	#endif 
 #endif
};


int opt_set_value (char **av, zOptval *v, char type, char *err);
int opt_usage ( zOption *opts, const char *prog, const char *msg, int status);
int opt_set ( zOption *opts, const char *flag );
zOptval opt_get ( zOption *opts, const char *flag );
int opt_eval ( zOption *opts, int argc, char **av );

#endif
