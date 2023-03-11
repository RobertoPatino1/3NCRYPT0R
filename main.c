#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include "resources/libsha1.h"
#include "resources/blowfish.h"

//Color constants definition: 
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

//Flag declarations
bool dflag = false;	//This flag validates if the user wants to decrypt
bool fflag = false;	//This flag validates if the user wants to encrypt/decrypt an entire directory
bool kflag = false; //This flag corresponds to the specified key



//This function assigns the hex value of the key in sha1 (16 characters) to the output variable
static void get_hex(char* input, char* output){
	int loop = 0;
	int i = 0;
	for(int j =0;j<8;j++){
		sprintf((char*)(output+i),"%02x",(unsigned char) input[loop]);
		loop+=1;
		i+=2;
    	}
	//insert NULL at the end of the output string
	output[i++] = '\0';

}


void print_help()
{
	// print_logo();
	printf("Welcome to 3NCRYPT0R!\n");
	printf("Usage:\n[-d] [-f] -k <key> <file_name|directory>\n");
	printf("Options:\n");
	printf(" -h\t\t\tHelp, shows the current message\n");
	printf(" -d\t\t\tDecrypts the specified file/directory passed as a parameter.\n");
	printf(" -f\t\t\tEncrypts/Decrypts a specified directory passed as a parameter.\n");
	printf(" -k <key>\t\tTakes the de encription key as a parameter.\n");
}


int main(int argc, char **argv){

	struct stat mi_stat;
	char *input_file = NULL;
	char *keyword = NULL;
	char *input_folder = NULL;

	int opt, index;

	size_t n = 0;
	ssize_t l = 0;
	int i;
	
	
	while ((opt = getopt (argc, argv, "dhk:f")) != -1){
		switch(opt)
		{
			case 'd':
				dflag = true;
				break;
			case 'f':
				fflag = true;
				break;
			case 'h':
				print_help();
				return 0;
			case 'k':
				kflag = true;
				keyword = optarg;
        		break;
			case '?':
				print_help();
				break;

			default:
				print_help();
				fprintf(stderr, "Usage: %s [-d] -k <key> <file_name|directory>\n", argv[0]);
				fprintf(stderr, "     %s -h\n", argv[0]);
				return 1;
		}
	}

	if(fflag){	
		//We obtain the specified directory
		for (index = optind; index < argc; index++)
			input_folder = argv[index];
	}else{
		//Otherwise get the file name
		for (index = optind; index < argc; index++)
			input_file = argv[index];
	}

	//If the user does not specify the -k option
	if(!kflag){
		fprintf(stderr, RED "The option -k must be used in order to specify the keyword.\n" RESET);
		fprintf(stderr, "Usage: %s [-d] -k <key> <file_name|directory>\n", argv[0]);
		fprintf(stderr, "Help menu:     %s -h\n", argv[0]);
		return 1;
	}

	if(!input_file && !input_folder){
		fprintf(stderr, RED "Please specify the name of the file or directory.\n" RESET);
		fprintf(stderr, "Usage: %s [-d] -k <key> <file_name|directory>\n", argv[0]);
		fprintf(stderr, "     %s -h\n", argv[0]);
		return 1;

	}else if(fflag){
		if(stat(input_folder, &mi_stat) < 0){
			fprintf(stderr, RED "The specified directory \"%s\" does not exist!\n" RESET, input_folder);
			return 1;
		}else
			printf(GRN "Reading the directory \"%s\" (%ld bytes)...\n" RESET, input_folder, mi_stat.st_size);
	}else{
		//User specified a file
		if(stat(input_file, &mi_stat) < 0){
			fprintf(stderr, RED "The specified file \"%s\" does not exist!\n" RESET, input_file);
			return 1;
		}else
			printf(GRN "Reading the file \"%s\" (%ld bytes)...\n" RESET, input_file, mi_stat.st_size);
	}

	l = strlen(keyword)+1;
	/*
	Encrypting the specified keyword
	passed as an argument to the -k option
	*/
	sha1_ctx ctx; //Context for sha1 hash
	char dgst[SHA1_DIGEST_SIZE]; //Final hash as sha1
	sha1_begin(&ctx);
	for(i = 0; i < l-1; ++i)
		sha1_hash(keyword+i, 1, &ctx);
	sha1_end(dgst, &ctx);


	//We get only the first 16 characters of the sha1 hash
	char new_keyword[16];
	get_hex(dgst,new_keyword);
	if(!dflag){
		printf(GRN "Your 16 character encrypted keyword is: " RESET );
		printf(YEL "%s\n" RESET,new_keyword);
		printf(RED "WARNING: " RESET);
		printf("Make sure to save the encrypted keyword as it is the only way of decrypting this file/folder\n");
	}
	//Creating the directories to store encrypted/decrypted files at runtime
	int dir1Check;
	int dir2Check;
	DIR* encr_dir = opendir("encrypted_files");
	DIR* decr_dir = opendir("decrypted_files");
	if(!encr_dir){
		printf(BLU "Creating directory: \"encrypted_files\"\n" RESET);
		dir1Check = mkdir("encrypted_files",0777);
		printf(GRN "Encryption directory successfully created\n" RESET);
	}
	
	if(!decr_dir){
		printf(BLU "Creating directory: \"decrypted_files\"\n" RESET);
		dir2Check = mkdir("decrypted_files",0777);
		printf(GRN "Decryption directory successfully created\n" RESET);
	}

	/*
	Encrypting or decrypting the specified file/folder
	according to the given parameters
	*/

	//Obtaining the numeric value of the key in ASCII hex
	BYTE key_arg[8];
	unsigned short byte,k;
	if(dflag){
		//On decryption we use the passed key directly
		for(k=0;k<8;k++){
                        sscanf(keyword + 2*k,"%2hx", &byte);
                        key_arg[k] = (BYTE) byte;
                }
	}else{
		//If we encrypt, the key is transformed
		for(k=0;k<8;k++){
			sscanf(new_keyword + 2*k,"%2hx", &byte);
			key_arg[k] = (BYTE) byte;
		}
	}


	/*
	Creating and locating the encrypted/decrypted files on
	their corresponding directory
	*/

	if(fflag){
		//Case 1: Directory...
		struct dirent *de; // Pointer for directory entry

		// opendir() returns a pointer of DIR type.
  		DIR *dr = opendir(input_folder);

  		if (dr == NULL) {
   			 printf(RED "ERROR: Unable to open specified directory\n" RESET);
   			 return 0;
 		 }
		strcat(input_folder,"/");

  		while ((de = readdir(dr)) != NULL){
			//Iterating through all the files in the directory
			if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){
				char *output_file = (char*) calloc(strlen(de->d_name)+5,sizeof(char));
				input_file = de->d_name;
				//Source and destination of the files in use
				char *dest;
				char *src = (char*) calloc(strlen(input_folder)+strlen(input_file)+5,sizeof(char));

				strcpy(src,input_folder);
				strcpy(output_file,input_file); //Passing content from input_file to output_file
				if(dflag){
					dest = (char*) calloc(strlen("decrypted_files/")+strlen(input_file)+5,sizeof(char));
					strcpy(dest,"decrypted_files/");
		 		}else{
					dest = (char*) calloc(strlen("encrypted_files/")+strlen(input_file)+5,sizeof(char));
					strcpy(dest,"encrypted_files/");
				}
				strcat(src,input_file);
				strcat(dest,output_file);

				//Encryption|Decryption proccess
				int fd_output = open(dest,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
				BYTE input_buf[BLOWFISH_BLOCK_SIZE];
				BYTE output_buf[BLOWFISH_BLOCK_SIZE];
				BLOWFISH_KEY key;
				blowfish_key_setup(key_arg, &key, BLOWFISH_BLOCK_SIZE);

				//Reading the file
				int fd_input = open(src,O_RDONLY,0);

				while(read(fd_input,input_buf,BLOWFISH_BLOCK_SIZE)>0){
					if(dflag){
						//Decrypt
						blowfish_decrypt(input_buf, output_buf, &key);
					}else{
						//Encrypt
						blowfish_encrypt(input_buf, output_buf, &key);
					}

					write(fd_output,output_buf,BLOWFISH_BLOCK_SIZE);
					memset(input_buf,0,BLOWFISH_BLOCK_SIZE);
				}

				if(dflag){
					printf(GRN "The file" RESET);
					printf(BLU " \"%s\"" RESET,de->d_name);
					printf(GRN " has been successfully decrypted and stored as:" RESET);
					printf(BLU " \"%s\"\n" RESET,dest);

				}else{
					printf(GRN "The file" RESET);
					printf(BLU " \"%s\"" RESET,de->d_name);
					printf(GRN " has been successfully encrypted and stored as:" RESET);
					printf(BLU " \"%s\"\n" RESET,dest);
				}
				// closes and free calloc

				close(fd_input);
				close(fd_output);
				free(output_file);
				free(dest);


			}

		}
  		closedir(dr);

	}else{
		//Case 2: File...
		char *output_file = (char*) calloc(strlen(input_file)+5,sizeof(char));
		char *dest;

		strcpy(output_file,input_file);	//Passing content from input_file to output_file

		if(dflag){
			dest = (char*) calloc(strlen("decrypted_files/")+strlen(input_file)+5,sizeof(char));
			strcpy(dest,"decrypted_files/");
                }else{
			dest = (char*) calloc(strlen("encrypted_files/")+strlen(input_file)+5,sizeof(char));
			strcpy(dest,"encrypted_files/");
		}
		strcat(dest,output_file);
		int fd_output = open(dest,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
		BYTE input_buf[BLOWFISH_BLOCK_SIZE];
		BYTE output_buf[BLOWFISH_BLOCK_SIZE];
		BLOWFISH_KEY key;
		blowfish_key_setup(key_arg, &key, BLOWFISH_BLOCK_SIZE);

		//Reading the file
		int fd_input = open(input_file,O_RDONLY,0);
		while(read(fd_input,input_buf,BLOWFISH_BLOCK_SIZE)>0){
			if(dflag){
				//Decrypt
				blowfish_decrypt(input_buf, output_buf, &key);
			}else{
				//Encrypt
				blowfish_encrypt(input_buf, output_buf, &key);
			}

			write(fd_output,output_buf,BLOWFISH_BLOCK_SIZE);
			memset(input_buf,0,BLOWFISH_BLOCK_SIZE);
		}

		if(dflag){
			printf(GRN "The file" RESET);
			printf(BLU " \"%s\"" RESET,input_file);
			printf(GRN " has been successfully decrypted and stored as:" RESET);
			printf(BLU " \"%s\"\n" RESET,dest);
		}else{
			printf(GRN "The file" RESET);
			printf(BLU " \"%s\"" RESET,input_file);
			printf(GRN " has been successfully encrypted and stored as:" RESET);
			printf(BLU " \"%s\"\n" RESET,dest);
		}
		// closes and free calloc
		close(fd_input);
		close(fd_output);
		free(output_file);
		free(dest);

	}



}



