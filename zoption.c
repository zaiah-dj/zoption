#include "zoption.h"

#define OPT_ERRV_LENGTH 128

#define OPT_ERR(err,opt,last) \
	( opt->error = snprintf( opt->errmsg, OPT_ERRV_LENGTH - 1, "[%s:%d, %s] ", __FILE__, __LINE__, __func__ ) ) && \
	( snprintf( opt_errmsg, OPT_ERRV_LENGTH - 1, opt_errmsgs[ err ], last ) )

static char opt_errmsg[ OPT_ERRV_LENGTH ] = { 0 };

enum err {
	 ERR_OPT_UNINITIALIZED
	,ERR_OPT_TOO_LONG
	,ERR_OPT_UNEXPECTED_FLAG
	,ERR_OPT_UNEXPECTED_ARGUMENT
	,ERR_OPT_EXPECTED_ANY
	,ERR_OPT_EXPECTED_STRING_ARG
	,ERR_OPT_EXPECTED_NUMERIC_ARG
	,ERR_OPT_VALIDATION_FAILED
};

static char *opt_errmsgs[] = {
	[ERR_OPT_UNINITIALIZED] = "Option module uninitialized\n",
	[ERR_OPT_TOO_LONG] = "Option flag too long\n",
	[ERR_OPT_UNEXPECTED_FLAG] = "Received unexpected flag: %s\n",
	[ERR_OPT_UNEXPECTED_ARGUMENT] = "Received unexpected argument\n",
	[ERR_OPT_EXPECTED_ANY] = "Expected argument after flag %s\n" ,
	[ERR_OPT_EXPECTED_STRING_ARG] = "Expected string after flag %s\n" ,
	[ERR_OPT_EXPECTED_NUMERIC_ARG] = "Expected number after flag %s\n" ,
	[ERR_OPT_VALIDATION_FAILED] = "Validation failed.\n" 
};



//Set values when the user asks for them
int opt_set_value (char **av, zOptval *v, char type, char *err) {
	/*Get original flag and other things.*/
	char flag[64]={0}; 
	snprintf(flag, 63, "%s", *av);
	av++;

	/*Catch what may be a flag*/
	if (!*av || (strlen(*av) > 1 && *av[0] == '-' && *av[1] == '-'))
		return (snprintf(err, 1023, "Expected argument after flag %s\n", flag) && 0); 
	
	/*Evaluate the three different types*/
	if (type == 'c') {
		v->c = *av[0];	
	}
	else if (type == 's') {
		_Bool isstr=0;
		for (int i=0;i<strlen(*av); i++) { 
			/*We can safely assume this is an ascii string, if this passes*/
			if ((*av[i] > 32 && *av[i] < 48) || (*av[i] > 57 && *av[i] < 127)) {
				isstr = 1;
				break;
			}
		}

		if (!isstr)
			return (snprintf(err, 1023, "Expected string after flag %s\n", flag) && 0);
		v->s = *av;	
	}
	else if (type == 'n') {
		char *a = *av; /*Crashes for some reason if I just use dereference*/
		for (int i=0;i<strlen(a); i++)
			if ((int)a[i] < 48 || (int)a[i] > 57) /*Not a number check*/
				return (snprintf(err, 1023, "Expected number after flag %s\n", flag) && 0);
		v->n = atoi(*av);	
	}

	return 1; 
}


//Dump all options and show a usage message.
int opt_usage ( zOption *opts, const char *prog, const char *msg, int status) {
	if (prog && msg)
		fprintf( stderr, "%s: %s\n", prog, msg );
	else if (msg) {
		fprintf( stderr, "%s\n", msg );
	}

	while ( !opts->sentinel ) { 
		if (opts->type != 'n' && opts->type != 'c' && opts->type != 's') 
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , opts->lng, opts->description);
		else {
			char argn[1024]; memset(&argn, 0, 1024);
			snprintf(argn, 1023, "%s <arg>", opts->lng);
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , argn, opts->description);
		}
		opts++;
	}
	
	return status;
}


//Check if an option was set by a user
int opt_set ( zOption *opts, const char *flag )  {
	zOption *o = opts;
	while (!o->sentinel) {
		if (strcmp(o->lng, flag) == 0 && o->set)
			return 1; 
		o++;
	}
	return 0;
}


//Return a value if it was set, or a nil value (NULL for strings, 0 for numbers)
zOptval opt_get ( zOption *opts, const char *flag ) {
	zOption *o = opts;
	while (!o->sentinel) {
		if (strcmp(o->lng, flag) == 0)
			return o->v; 
		o++;
	}
	return o->v; /*Should be the last value, and it should be blank*/
}



//Evaluate options that the user gave and die with a message
int opt_eval ( zOption *opts, int argc, char **av ) {
	char buf[1024] = { 0 };

	while ( *av ) {
		zOption *o = opts;
		while ( !o->sentinel ) {
			//Find option, set boolean, and run a validator callback
			if ( (o->sht && strcmp(*av, o->sht) == 0) || (o->lng && strcmp(*av, o->lng) == 0) ) {
				o->set = 1;
				if ( o->callback ) {
					if ( !o->callback( ++av, &o->v, buf ) ) {
						//NOTE: This is stupid, but the user will have no need for the zOption array if an error occurs.
						o = &opts[0];
						o->errmsg = opt_errmsg;
						return OPT_ERR( ERR_OPT_VALIDATION_FAILED, o, NULL );
					}
				}
				else if ( o->type == 'n' || o->type == 's' || o->type == 'c' ) {
					//Move all args up
					++av;

					//Why would this ever be?
					if ( !( *av ) ) {
						o = &opts[0];
						o->errmsg = opt_errmsg;
						return OPT_ERR( ERR_OPT_EXPECTED_ANY, o, NULL );
					}

					//Catch what may be a flag
					if ( strlen( *av ) > 1 && *av[0] == '-' && *av[1] == '-' ) {
						o = &opts[0];
						o->errmsg = opt_errmsg;
						return OPT_ERR( ERR_OPT_UNEXPECTED_FLAG, o, *av );
					}
			

					//Evaluate the three different types
					if ( o->type == 'c' ) {
						(&o->v)->c = *av[0];
					}
					else if ( o->type == 'n' ) {
						//char *a = *av; //Crashes for some reason if I just use dereference
						for ( int i=0; i < strlen( *av ); i++ ) {
							if ( (int)(*av)[i] < 48 || (int)(*av)[i] > 57 ) { //Not a number check
								o = &opts[0];
								o->errmsg = opt_errmsg;
								return OPT_ERR( ERR_OPT_EXPECTED_NUMERIC_ARG, o, *av );
							}
						}
						(&o->v)->n = atoi( *av );	
					}
					else if ( o->type == 's' ) {
						_Bool isstr=0;
						for ( int i=0; i < strlen(*av); i++ ) { 
							//We can safely assume this is an ascii string, if this passes
							if ( ( *av[i] > 32 && *av[i] < 48 ) || ( *av[i] > 57 && *av[i] < 127 ) ) {
								isstr = 1;
								break;
							}
						}

						if ( !isstr ) {
							o = &opts[0];
							o->errmsg = opt_errmsg;
							return OPT_ERR( ERR_OPT_EXPECTED_STRING_ARG, o, *av );
						}
						(&o->v)->s = *av;
					}
				}
			}
			o++;
		}
		av++;
	}
	return 1;
}
