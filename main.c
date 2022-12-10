#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "help.h"

/*
Flag declarations:
	TODO
	The combinations of each switch are yet to be defined
*/
bool dFlag = false;	//-d -> Displays the result of encrypting a given String
bool wFlag = false;	//-w -> Writes in a file the result of encrypting a given String
bool hFlag = false;	//-h -> Prints the help menu
bool sha1Flag = false;	//-sha1 -> Asks for a String to encrypt with sha1 encryption algorythm
bool md5Flag = false;	//-md5 -> Asks for a String do encrypt with md5 encryption
/*
TODO
Some more switches to add....
*/

//TODO: ADD PRINT HELP FUNCTION


int main(int argc, char **argv){
	printf("This is 3NCRYPT0R\n");

	int opt,index;

	/*
	Loops each passed argument looking for the
	required options

	TODO: Add mandatory parameters logic
	*/
	while ((opt = getopt (argc, argv, "hdwsm")) != -1){
        	 switch(opt){
                        case 'h':
				//TODO
                                print_help();	//Prints the help menu
                                return 0;
                        case 'd':
				//TODO: Call a display function
                                dFlag = true;
                                break;
                        case 'w':
				//TODO: Call a write function
				//Add mandatory parameter for the file's name
                                wFlag = true;
                                break;
                        case 's':
				//TODO: Change s switch (char) to sha1 (string)???
                                sha1Flag = true;
                                break;
                        case 'm':
				//TODO: Change m switch (char) to sha1 (string)???
                                md5Flag = true;
                                break;
                        case '?':
                        default:
                                fprintf(stderr, "used: %s [-i] [-e] [arg 1] [arg 2] ... [arg n]\n", argv[0]); //TODO: update>                                fprintf(stderr, "     %s -h\n", argv[0]);
                                return -1;
                }
        }
	return 0;
}
