# zoption

zoption is a library for handling command-line options.  It is intended as a drop-in two-file library that takes little work to setup and teardown, and even less work to integrate into a project of your own.


## Installation

To keep things simple, there will probably never be an official package for this.  zhasher will build on any Unix based platform with whatever compiler you choose.

zoption can be built with:
	`gcc -Wall -Werror -std=c99 zoption.c main.c`

where main.c can be your C program.


## Usage

An example of working with zOption library is below:

<pre>
//First, we include the headers for our option parser.
#include "zoption.h"


//Second, we'll specify an array of options and what to expect.
zOption opts[] = {
	
	//This option acts like a flag that takes no options.
	{ "-c", "--cafe", "Specify whether we need to make coffee or not." }

	//This option will require an argument.  's' denotes that the argument should be a string.
, { "-t", "--type", "What kind of coffee do we want to make?", 's' }

	//This option also requires an argument.  'n' denotes that the argument should be a number.
, { NULL, "--cups", "How many cups do we want?", 'n' }
};


int main ( int argc, char \*argv[] ) {
	//If we need at least one option to run this program, make sure it's specified.
	if ( argc < 2 ) {
		return opt_usage( opts, "my program", "No options specified.", 1 );
	}

	//This will fill a structure with the user's specified options and any needed values
	opt_eval( opts, argc, argv );	

	//Check if --cafe was specified.
	if ( opt_set( opts, "--cafe" ) ) {
		//Trigger a function based on --something being called
	}

	//Check if --cups was specified.
	int cups = 0;
	if ( opt_set( opts, "--cups" ) ) {
		//Extract the value specified by the user of the CLI program
		cups = opt_get( opts, "--cups" );	
	}

	return 0;
}
</pre>
