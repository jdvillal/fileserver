#include <getopt.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <syslog.h>

#include "csapp.h"
#include "common.h"

void atender_cliente(int connfd);
void *thread(void *vargp);
void daemonize();
int onStartConnectionServer(int connfd, BLOWFISH_KEY *key);
void putSyslogMessage(char* message, int type);

int isFile(char *fname){
    int len = strlen(fname);
    int pCount = 0;
    if(len < 3){
        return 0;
    }else{
        for(int i = 0; i < len; i++){
            char c = *(fname+i);
            if(c == '.'){
                if(i == 0 || i == len){
                    return 0;
                }
                pCount++;
            }
        }
        if(pCount > 0){
            return 1;
        }else{
            return 0;
        }
    }
}

char * listDirLine(){
    DIR *dir;
    char *dirLine = (char*)malloc(1000);
    strcpy(dirLine,"Archivos:");
    struct dirent *ent;
    if ((dir = opendir ("./")) != NULL) {
      /* print all the files and directories within directory */
        int count = 0;
        while ((ent = readdir (dir)) != NULL) {
        	//printf("%d",count);
            //printf ("%s\n", ent->d_name);
            if(isFile(ent->d_name)){
                dirLine = spaceConcat(dirLine,ent->d_name);
            }
            count++;
        }
        closedir (dir);
    }
    //printf("%s\n",dirLine);
    return dirLine;
}


void daemonize(char *port){
	int fd0,fd1,fd2;
	pid_t pid;

	if((pid = fork()) < 0){
		putSyslogMessage("No fue posible ejecutar como Daemon.", -1);
		exit(1);
	}else if(pid!=0){
		printf("Escuchando como Daemon en el puerto %s\n",port);
		exit(0);
	}
	setsid();
	close(0);close(1);close(2);
	fd0 = open("/dev/null",O_RDWR);
	fd1 = dup(fd0);
	fd2 = dup(fd0);
}

void print_help(){
	printf("file_server uses Blowfish encryption to upload and download files to a\n");
	printf("server from connected clients.\n");
	printf("Usage:\n");
	printf("\tfile_server [-d] <port>\n");
	printf("\tfile_server -h\n");
	printf("Options:\n");
	printf(" -h\t\t\tHelp, show this screen.\n");
	printf(" -d\t\t\tDaemon mode.\n");
}



/**
 * Función que crea argv separando una cadena de caracteres en
 * "tokens" delimitados por la cadena de caracteres delim.
 *
 * @param linea Cadena de caracteres a separar en tokens.
 * @param delim Cadena de caracteres a usar como delimitador.
 *
 * @return Puntero a argv en el heap, es necesario liberar esto después de uso.
 *	Retorna NULL si linea está vacía.
 */


int main(int argc, char **argv){
	int opt;

	//Sockets
	int listenfd;
	int dOpt = 0;
	unsigned int clientlen;
	char *port;
	


	while ((opt = getopt (argc, argv, "dh")) != -1){
		switch(opt)
		{
			case 'd':
				dOpt = 1;
				break;
			case 'h':
				print_help();
				return 0;
			default:
				fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
				fprintf(stderr, "     %s -h\n", argv[0]);
				return -1;
		}
	}

	if(argc < 2){
		fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
		fprintf(stderr, "     %s -h\n", argv[0]);
		return -1;
	}else
		if(argc == 3){
			port = argv[2];
		}else{
			port = argv[1];
		}

	//Valida el puerto
	int port_n = atoi(port);
	if(port_n <= 0 || port_n > USHRT_MAX){
		fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
		return -1;
	}

	//Abre un socket de escucha en port
	listenfd = open_listenfd(port);

	if(listenfd < 0)
		connection_error(listenfd);

	if(dOpt){
		daemonize(port);
	}else{
		printf("server escuchando en puerto %s...\n", port);
	}
	int*connfdp;

	//Direcciones y puertos
	struct sockaddr_in clientaddr;

	while (1) {
		pthread_t tid;
		connfdp = malloc(sizeof(int));
		clientlen = sizeof(clientaddr);
		*connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		Pthread_create(&tid, NULL, thread, connfdp);
		
	}
}

void atender_cliente(int connfd){
	int n;
	BLOWFISH_KEY key;
	if((n = onStartConnectionServer(connfd,&key)) < 0){
		return;
	}
	char request[MAXLINE];
	//Comunicación con cliente es delimitada con '\0'
	while(1){
		memset(request,0,MAXLINE);
		if((n = read(connfd, request,MAXLINE)) <= 0)
			return;
		if(strcmp(request, "BYE\n") == 0){
			return;
		}
		removeln(request);
		char **parsed = parse_comand(request," ");
		if(strcmp(*parsed,"GET")==0){
            long fileSize = validateFile(*(parsed+1));
            char fsizeChar[MAXLINE] = {0};
        	sprintf(fsizeChar,"%ld\n", fileSize);
            int toSend = open(*(parsed+1),O_RDONLY,0);
            if(fileSize > 0){
            	if((n = write(connfd,fsizeChar,strlen(fsizeChar))) < strlen(fsizeChar)){
            		putSyslogMessage("Ha ocurrido un error.",-1);
            		break;
            	}else{
            		if((n = encryptAndSend(connfd,toSend,fileSize,&key)) < 0){
            			putSyslogMessage("Ha ocurrido un problema al enviar el archivo al cliente.",-1);
            		}
            	}
            }else{
            	n = write(connfd,"0\n",2);
            }
        }else if(strcmp(*parsed,"PUT")==0){
			char *fsizeChar;
			fsizeChar = *(parsed+2);
            removeln(fsizeChar);
            long fileSize = atoi(fsizeChar);
            char prefix[MAXLINE];
            strcpy(prefix,"upload_");
            int newfd = open(strcat(prefix,*(parsed+1)),O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);
            if(n = receiveAndDecrypt(connfd,newfd,fileSize,&key) < 0){
            	putSyslogMessage("Ha ocurrido un error al recibir el archivo.",-1);
            }
        }else if(strcmp(*parsed,"LIST")==0){
        	puts("LIST\n");
            char* list = listDirLine();
            printf("%s ",list);
            if((n = write(connfd,list,strlen(list))) < strlen(list)){
            	putSyslogMessage("Ha ocurrido un problema al enviar el archivo al cliente.",-1);
            	puts("Error: Ha ocurrido un problema al enviar el archivo al cliente.\n");
            }
        }else if(strcmp(*parsed,"NULL")==0){
        	putSyslogMessage("Se ha recibido un comando desconocido.",-1);
        }
	}
}


/* Thread routine */
void *thread(void *vargp){
	int connfd = *((int *)vargp);
	Pthread_detach(pthread_self());
	free(vargp);
	putSyslogMessage("Se ha conectado un nuevo cliente.",1);
	atender_cliente(connfd);
	putSyslogMessage("Se ha desconectado un cliente.",1);
	close(connfd);
	return NULL;
}

int onStartConnectionServer(int connfd, BLOWFISH_KEY *key){
	char buffer[MAXLINE] = {0};
	int n;
	uint8_t privateThis[32] = {0};
	uint8_t publicThis[64] = {0};
    uint8_t secret[32] = {0};
    uint8_t externKey[64] = {0};
    const struct uECC_Curve_t * cv = uECC_secp160r1();
    if(!uECC_make_key(publicThis, privateThis, cv)){
        putSyslogMessage("Fallo al generar las claves (uECC).",-1);
        return -1;
    }
    n = read(connfd,buffer,6);
    if(strcmp(buffer,"START\n")==0){
    	n = write(connfd,publicThis,64);
		if(n<64){
			putSyslogMessage("No se pudo enviar la clave publica al cliente.",-1);
			return -1;
		}
		n = read(connfd,externKey,64);
		if(n<64){
			putSyslogMessage("No se pudo recibir la clave publica del cliente.",-1);
			return -1;
		}
		if(!uECC_shared_secret(externKey, privateThis, secret, cv)){
	        putSyslogMessage("Fallo al generar la clave compartida.",-1);

	        return -1;
	    }
		BYTE shaKey[32] = {0};
		getSHA256Key(secret,shaKey);
		blowfish_key_setup(shaKey, key, 32);
	    return 1;		
    }else{
    	return -1;
    }
}

void putSyslogMessage(char* message, int type){
	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("Fileserver", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	if(type > 0){
		syslog (LOG_NOTICE, "[File_server| uid:%d | pid:%d]: %s", getuid(), getpid(), message);
	}else{
		syslog (LOG_NOTICE, "[File_server| uid:%d | pid:%d]: *ERROR* %s", getuid(), getpid(), message);
	}
	closelog ();
}
