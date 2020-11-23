/* main.c */
#include "zoption.h"

#define PROGRAM "TEST"

zOption opts[] = {
	{ "-s", "--short", "Specify short arguments." }
, { "-d", "--div", "Specify a number.", 'n' }
, { .sentinel = -1 }
};

int main ( int argc, char *argv[] ) {
	//...
	if ( argc < 2 ) {
		return opt_usage( opts, PROGRAM, "No options specified.", 1 );
	}

	//Evaluate all the options
	opt_eval( opts, argc, argv ); 

	//Define stuff
	int num; 

	//Check for finished arguments
	if ( opt_set( opts, "--short" ) ) {
		fprintf( stderr, "Option --short was specified.\n" );
	}

	//Pull a number 
	if ( opt_set( opts, "--div" ) ) {
		fprintf( stderr, "Option --div was specified.\n" );
		num = opt_get( opts, "--div" ).n;
		fprintf( stderr, "%d\n", num );
	}

	return 0;
}
