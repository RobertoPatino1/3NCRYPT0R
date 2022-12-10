#include <stdio.h>
#include <stdbool.h>

void print_help(){
	//TODO: SPECIFY THE OPTIONS IN A BETTER WAY
	//TODO: ADD MORE OPTIONS 
	printf("---Help menu---\n");
	printf("Options:\n");
	printf("-d\t\t\tDisplays the result of encrypting the input String\n");
	printf("-w [file]\t\t\tWrites the encrypted input String on the specified file (If the file does not exist, it is created)\n");
	printf("-s\t\t\tEncrypts the input String with the sha1 encryption algorithm\n");
	printf("-m\t\t\tEncrypts the input String with the md5 encryption algorithm\n");
}
