#include <stdio.h>


#define BLU   "\x1B[34m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"
#define CYN   "\x1B[36m"

void print_logo(){

	puts(
	GRN" _____  _   _  _____ ______ __   ________  _____  _____ ______ \n"
	"|____ || \\ | |/  __ \\| ___ \\  \\ / /| ___ \\|_   _||  _  || ___ \\ \n"
	"    / /|  \\| || /  \\/| |_/ / \\ V / | |_/ /  | |  | |/' || |_/ /\n"
	"    \\ \\| . ` || |    |    /   \\ /  |  __/   | |  |  /| ||    / \n"
	BLU".___/ /| |\\  || \\__/\\| |\\ \\   | |  | |      | |  \\ |_/ /| |\\ \\ \n"
	"\\____/ \\_| \\_/ \\____/\\_| \\_|  \\_/  \\_|      \\_/   \\___/ \\_| \\_|\n"RESET);
	printf(CYN"Welcome to 3NCRYPT0R!\nMake sure to check the help menu running the executable file as ");
	printf(GRN"./3NCRYPT0R -h\n\n"RESET);
}

void main(){
    print_logo();
    return;
}