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
#include "libsha1.h"
#include "blowfish.h"

//Declaracion de flags
bool dflag = false;	//Flag para validar si tenemos que desencriptar un archivo
bool fflag = false;	//Flag para validar si tenemos que encriptar/desencriptar una carpeta completa


//Imprime cadena de bytes en formato hexadecimal
static void print_hex(const char* data, size_t size){
        int i;
        for(i = 0; i < size; ++i)
                printf("%02x",(unsigned char) data[i]);
}

//Funcion que asigna a la variable output el valor hexadecimal de la clave en sha1 (16 caracteres solamente)
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


void print_help(char *command)
{
        printf("secret encripta o desincripta un archivo o carpeta usando el algoritmo Blowfish.\n");
        printf("uso:\n %s [-d] [-f] -k <key> <nombre_archivo|nombre_carpeta>\n", command);
        printf(" %s -h\n", command);
        printf("Opciones:\n");
        printf(" -h\t\t\tAyuda, muestra este mensaje\n");
        printf(" -d\t\t\tDesincripta el archivo en lugar de encriptarlo.\n");
	printf(" -f\t\t\tEncripta/Desencripta una carpeta en vez de un archivo.\n");
        printf(" -k <key>\t\tEspecífica la clave (key) de encriptación, 8 bytes en hex.\n");
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
				print_help(argv[0]);
				return 0;
			case 'k':
				keyword = optarg;
        		break;
			case '?':
			default:
				fprintf(stderr, "uso: %s [-d] -k <key> <nombre_archivo|nombre_carpeta>\n", argv[0]);
				fprintf(stderr, "     %s -h\n", argv[0]);
				return 1;
		}
	}

	if(fflag){	//Se uso la flag -f por lo tanto se recibe el nombre de una carpeta
		for (index = optind; index < argc; index++)
			input_folder = argv[index];
	}else{
		//Caso contrario se trata de un archivo
		for (index = optind; index < argc; index++)
			input_file = argv[index];
	}

	if(!input_file && !input_folder){
		fprintf(stderr, "Especifique el nombre del archivo o carpeta.\n");
		fprintf(stderr, "uso: %s [-d] -k <key> <nombre_archivo|nombre_carpeta>\n", argv[0]);
		fprintf(stderr, "     %s -h\n", argv[0]);
		return 1;

	}else if(fflag){
		/* Ejemplo como verificar existencia y tamaño de una carpeta */
		if(stat(input_folder, &mi_stat) < 0){
			fprintf(stderr, "Carpeta %s no existe!\n", input_folder);
			return 1;
		}else
			printf("Leyendo la carpeta %s (%ld bytes)...\n", input_folder, mi_stat.st_size);
	}else{
		//Estamos en el caso de un archivo
                /* Ejemplo como verificar existencia y tamaño de un archivo */
                if(stat(input_file, &mi_stat) < 0){
                        fprintf(stderr, "Archivo %s no existe!\n", input_file);
                        return 1;
                }else
                        printf("Leyendo el archivo %s (%ld bytes)...\n", input_file, mi_stat.st_size);
	}
	
	l = strlen(keyword)+1;
	/*
	Encriptando con SHA-1 la palabra clave ingresada
	*/
	/*
	sha1_ctx ctx; //Contexto para hash sha1
	char dgst[SHA1_DIGEST_SIZE]; //resultado final hash sha1
	sha1_begin(&ctx);
	for(i = 0; i < l-1; ++i)
		sha1_hash(keyword+i, 1, &ctx);

	sha1_end(dgst, &ctx);

	//TODO: OMITIR ESTA PARTE???
	printf("La clave \'%s\' en SHA1 es: \n",keyword);
	print_hex(dgst, SHA1_DIGEST_SIZE);

	//Obteniendo solo los primeros 16 caracteres
	char new_keyword[16];
	get_hex(dgst,new_keyword);
	if(!dflag)
		printf("\nLos 16 caracteres de la clave en SHA1 son: %s\n",new_keyword);
	*/
	//Creando los directorios en tiempo de ejecucion
	int dir1Check;
	int dir2Check;
	dir1Check = mkdir("archivos_encriptados",0777);
	dir2Check = mkdir("archivos_desencriptados",0777);



	/*
	ENCRIPTANDO Y DESENCRIPTANDO LOS ARCHIVOS EN EN DEBIDO CASO
	*/

	//Extrayendo el valor numerico de la clave en ASCII hex
	BYTE key_arg[8];
	unsigned short byte,k;
	if(dflag){
		//Si desencriptamos, usamos la clave pasada directo
		for(k=0;k<8;k++){
                        sscanf(keyword + 2*i,"%2hx", &byte);
                        key_arg[i] = (BYTE) byte;
                }
	}else{
		//Si no desencriptamos, transformamos la clave
		for(k=0;k<8;k++){
			sscanf(keyword + 2*i,"%2hx", &byte);
			key_arg[i] = (BYTE) byte;
		}
	}

	//Creando los archivos encritpados/desencriptados y ubicandolos en sus respectivas carpetas
	
	if(fflag){
		//En caso de tratarse de una carpeta...
		struct dirent *de; // Pointer for directory entry

 		 // opendir() returns a pointer of DIR type.
  		DIR *dr = opendir(input_folder);

  		if (dr == NULL) {
   			 printf("No se pudo abrir el directorio especificado");
   			 return 0;
 		 }

  		while ((de = readdir(dr)) != NULL){
			//Iteramos solo sobre los archivos del directorio
			if(strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0){
				//Ahora solo replicamos la logica para cada archivo iterado
				char *output_file = (char*) calloc(strlen(de->d_name)+5,sizeof(char));
				char *dest;
				strcpy(output_file,de->d_name); //Usando strcopy para pasar contenido de input_file a output_file
				if(dflag){
					dest = (char*) calloc(strlen("archivos_desencriptados/")+strlen(de->d_name)+5,sizeof(char));
					strcpy(dest,"archivos_desencriptados/");
		 		}else{
					dest = (char*) calloc(strlen("archivos_encriptados/")+strlen(de->d_name)+5,sizeof(char));
					strcpy(dest,"archivos_encriptados/");
				}
				strcat(dest,output_file);
				//Encriptando o desencriptando cada archivo:
				int fd_output = open(dest,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
				BYTE input_buf[BLOWFISH_BLOCK_SIZE];
				BYTE output_buf[BLOWFISH_BLOCK_SIZE];
				BLOWFISH_KEY key;
				blowfish_key_setup(key_arg, &key, BLOWFISH_BLOCK_SIZE);
				//Leyendo el archivo
				int fd_input = open(de->d_name,O_RDONLY,0);
				while(read(fd_input,input_buf,BLOWFISH_BLOCK_SIZE)>0){
					if(dflag){
						//Desencriptar
						blowfish_decrypt(input_buf, output_buf, &key);
					}else{
						//Encriptar
						blowfish_encrypt(input_buf, output_buf, &key);
					}

					write(fd_output,output_buf,BLOWFISH_BLOCK_SIZE);
					memset(input_buf,0,BLOWFISH_BLOCK_SIZE);
				}

				if(dflag){
					printf("Archivo %s desencriptado exitosamente en %s...\n",de->d_name,dest);
				}else{
					printf("Archivo %s encriptado exitosamente en %s...\n",de->d_name,dest);
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
		//En caso de tratarse de un archivo...
		char *output_file = (char*) calloc(strlen(input_file)+5,sizeof(char));
		char *dest;

		strcpy(output_file,input_file);	//Usando strcopy para pasar contenido de input_file a output_file

		if(dflag){
			dest = (char*) calloc(strlen("archivos_desencriptados/")+strlen(input_file)+5,sizeof(char));
			strcpy(dest,"archivos_desencriptados/");
                }else{
			dest = (char*) calloc(strlen("archivos_encriptados/")+strlen(input_file)+5,sizeof(char));
			strcpy(dest,"archivos_encriptados/");
		}
		strcat(dest,output_file);
		//Encriptando o desencriptando el archivo:
		int fd_output = open(dest,O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
		BYTE input_buf[BLOWFISH_BLOCK_SIZE];
		BYTE output_buf[BLOWFISH_BLOCK_SIZE];
		BLOWFISH_KEY key;
		blowfish_key_setup(key_arg, &key, BLOWFISH_BLOCK_SIZE);

		//Leyendo el archivo
		int fd_input = open(input_file,O_RDONLY,0);
		while(read(fd_input,input_buf,BLOWFISH_BLOCK_SIZE)>0){
			if(dflag){
				//Desencriptar
				blowfish_decrypt(input_buf, output_buf, &key);
			}else{
				//Encriptar
				blowfish_encrypt(input_buf, output_buf, &key);
			}

			write(fd_output,output_buf,BLOWFISH_BLOCK_SIZE);
			memset(input_buf,0,BLOWFISH_BLOCK_SIZE);
		}

		if(dflag){
			printf("Archivo %s desencriptado exitosamente en %s...\n",input_file,dest);
		}else{
			printf("Archivo %s encriptado exitosamente en %s...\n",input_file,dest);
		}
		// closes and free calloc
		close(fd_input);
		close(fd_output);
		free(output_file);
		free(dest);

	}



}



