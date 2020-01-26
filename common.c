#include "common.h"

void print_hex(const unsigned char* data, size_t size) {
    int i;
    for (i = 0; i < size; ++i)
        printf("%02x", (unsigned char) data[i]);
}

void getSHA256Key(uint8_t* toHashKey, BYTE* newKey){
    BYTE buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, toHashKey, 32);
    sha256_final(&ctx, newKey);
}



void removeln(char *linea){
	for(int i = 0; i < strlen(linea); i++){
        if(*(linea+i)=='\n'){
            memset(linea+i,0,1);
            break;
        }
    }
}

char **parse_comand(char *linea, char *delim){
	char *token;
	char *linea_copy;
	int i, num_tokens = 0;
	char **argv = NULL;

	linea_copy = (char *) malloc(strlen(linea) + 1);
	strcpy(linea_copy, linea);

	/* Obtiene un conteo del número de argumentos */
	token = strtok(linea_copy, delim);
	/* recorre todos los tokens */
	while( token != NULL ) {
		token = strtok(NULL, delim);
		num_tokens++;
	}
	free(linea_copy);

	/* Crea argv en el heap, extrae y copia los argumentos */
	if(num_tokens > 0){

		/* Crea el arreglo argv */
		argv = (char **) malloc((num_tokens + 1) * sizeof(char **));

		/* obtiene el primer token */
		token = strtok(linea, delim);
		/* recorre todos los tokens */
		for(i = 0; i < num_tokens; i++){
			argv[i] = (char *) malloc(strlen(token)+1);
			strcpy(argv[i], token);
			token = strtok(NULL, delim);
		}
		argv[i] = NULL;
	}

	return argv;
}

void connection_error(int connfd){
	fprintf(stderr, "Error de conexión: %s\n", strerror(errno));
	close(connfd);
	exit(-1);
}

int encryptAndSend(int connfd, int toReadfd, long filesize, BLOWFISH_KEY *key){
	int n;
	long toSendSize = filesize;
	char buffer[8] = {0};
	BYTE enc_buf[8] = {0};
 	while(1){
    	toSendSize++;
        if(toSendSize%8==0){
            break;
        }
    }
	if((n = read(connfd,buffer,6)) < 6){
    	return-1;
    }
    if(strcmp(buffer,"READY\n")!=0){
    	return -1;
    }
    for(int i = 0; i < toSendSize; i){
        memset(buffer, 0,8);
        memset(enc_buf,0,8);
        if((n = read(toReadfd,buffer,8)) < 1){
            return -1;
        }
		blowfish_encrypt(buffer, enc_buf, key);
        if((n = write(connfd,enc_buf,8)) < 8){
            return -1;
        }
        i = i+8;    
    }
    return 1;
}

int receiveAndDecrypt(int connfd, int toWritefd, long filesize, BLOWFISH_KEY *key){
	int n;
	long toReadSize = filesize;
	char buffer[8] = {0};
	BYTE dec_buf[8] = {0};
 	while(1){
    	toReadSize++;
        if(toReadSize%8==0){
            break;
        }
    }
    if((n = write(connfd,"READY\n",6)) < 6){
        return -1;
    }
    for(int i = 0; i < toReadSize; i){
        memset(buffer, 0, 8);
        memset(dec_buf,0,8);
        if((n = safeRead(connfd,buffer,8)) < 0){
            return -1;
        }
		blowfish_decrypt(buffer, dec_buf, key);
		if(i+8>filesize){
			int towrite = filesize%8;
            n = write(toWritefd,dec_buf,towrite);
            if(n<towrite){
                return -1;
            }
            
		}else{
            n = write(toWritefd,dec_buf,8);
            if(n<8){
                return -1;
            }
        }
        i = i+8; 
    }
    return 1;
}

//retoran el tamaño en bytes si el archivo existe, retorn 0 si el archivo no existe.
long validateFile(char* filename){
	int n = open(filename,O_RDONLY,0);
	if(n<0){
		close(n);
		return 0;
	}else{
	    struct stat st;
	    stat(filename, &st);
	    char b[MAXLINE];
	    sprintf(b,"%ld", st.st_size);
	    return st.st_size;
	}
}

int safeRead(int fd,char *buffer, int toReadSize){
    int beenRead = 0;
    int scape = 0;
    while(1){
        if(scape > toReadSize*10){
            return -1;
        }
        int n;
        if((n = read(fd,buffer+beenRead,toReadSize-beenRead)) < 0){
            return -1;
        }
        beenRead = beenRead + n;
        if(beenRead == toReadSize){
            return 1;
        }
        scape = scape + 1;
    }
}

void addln(char *linea){
    for(int i = 0; i < strlen(linea); i++){
        if(*(linea+i)=='\0'){
            memset(linea+i,'\n',1);
            break;
        }
    }
}

char * spaceConcat(char* str1, char *str2){
    char *toReturn;
    toReturn = (char*)malloc(MAXLINE*sizeof(char));
    memset(toReturn,0,MAXLINE);
    sprintf(toReturn, "%s %s", str1, str2);
    return toReturn;
}