#include "common.h"
#include <limits.h>

int onStartConectionClient(int connfd, BLOWFISH_KEY *key);

void print_help(){
    printf("file_client connects to a remote file_server service and allows the\n");
    printf("user to upload and download files from the server.\n");
    printf("Usage:\n");
    printf("\tfile_client <ip> <port>\n");
    printf("\tfile_server -h\n");
    printf("Options:\n");
    printf(" -h\t\t\tHelp, show this screen.\n");
}

int main(int argc, char **argv){
	int clientfd;
	char *port;
	char *ip, buf[MAXLINE];
	rio_t rio;

	int opt;
	while ((opt = getopt(argc, argv, "hd")) != -1) {
        switch (opt) {
                /*En caso de recibir la opcion -h se muestra el mensaje de informacion y retorna main*/
            case 'h':
            print_help();
                return 0;
            case '?':
                /*Si se desconoce la opcion ingresada se informa al usuario y termina la ejecucion*/
            print_help();
                return 0;
        }
    }

	if (argc != 3) {
		fprintf(stderr, "usage: %s <ip> <port>\n", argv[0]);
		exit(0);
	}
	ip = argv[1];
	port = argv[2];

	clientfd = Open_clientfd(ip, port);
    int n;
    BLOWFISH_KEY key;
    if((n = onStartConectionClient(clientfd,&key))<0){
        puts("ERROR: No se ha podido establecer la conexion con el servidor.\n");
        return 0;
	}
    int l = 1;
    size_t max = MAXLINE;
    char *linea_consola = (char *) calloc(1, MAXLINE);
    while(l > 0){
        printf("> ");
        memset(linea_consola,0,MAXLINE);
        l = getline(&linea_consola, &max, stdin); //lee desde consola
        char lineacopy[MAXLINE]; strcpy(lineacopy,linea_consola);
        char **parsed = parse_comand(lineacopy," ");
        if(strcmp(*(parsed),"GET")==0){
            if((n = write(clientfd,linea_consola,strlen(linea_consola))) < strlen(linea_consola)){break;}
            removeln(*(parsed+1));
            char fsizeChar[MAXLINE] = {0};
            n = read(clientfd,fsizeChar,MAXLINE);
            removeln(fsizeChar);
            long fileSize = atoi(fsizeChar);
            if(fileSize > 0){
                char prefix[MAXLINE];
                strcpy(prefix,"download_");
                int newfd = open(strcat(prefix,*(parsed+1)),O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);
                if((n = receiveAndDecrypt(clientfd,newfd,fileSize,&key))>0){
                    printf("El archivo se ha descargado con exito y se ha guardado con el nombre: %s (%ld-bytes)\n",prefix,fileSize);
                }else{
                    puts("ERROR: No se ha podido descargar el archivo solicitado.\n");
                }
            }else{
                puts("El archivo solicitado no existe.\n");
            }
        }else if(strcmp(*parsed,"PUT")==0){
            removeln(*(parsed+1));
            long fileSize = validateFile(*(parsed+1));
            if(fileSize > 0){
                int toSend = open(*(parsed+1),O_RDONLY,0);
                char fsizeChar[MAXLINE] = {0};
                sprintf(fsizeChar,"%ld\n", fileSize);
                char *ln = spaceConcat(*parsed,*(parsed+1));
                char *toSendLine = spaceConcat(ln,fsizeChar);
                n = write(clientfd,toSendLine,strlen(toSendLine));
                if((n = encryptAndSend(clientfd,toSend,fileSize,&key))>0){
                    printf("El archivo %s (%ld bytes) ha sido enviado al servidor con exito.\n",*(parsed+1),fileSize);
                }else{
                    puts("Error: Ha ocurrido un problema al enviar el archivo al servidor.\n");
                }
            }else{
                puts("El archivo que intenta enviar al servidor no existe.\n");
            }
        }else if(strcmp(*parsed,"LIST\n")==0){
            char BUFF[MAXLINE] = {0};
            if((n = write(clientfd,"LIST\n",5) < 5)){break;}
            n = read(clientfd,BUFF,MAXLINE);
            printf("%s\n",BUFF);
            char **list = parse_comand(BUFF," ");
            int count = 0;
            while(1){
                if(*(list+count)==NULL){
                    break;
                }else{
                    printf("%s\n",*(list+count));
                }
                count++;
            }

        }else if(strcmp(*parsed,"BYE\n")==0){
            n = write(clientfd,"BYE\n",4);
            break;
        }else{
            puts("Comando no valido.\n");
            n = write(clientfd,"NULL\n",5);
        }
    }
	Close(clientfd);
	exit(0);
}

int onStartConectionClient(int clientfd, BLOWFISH_KEY *key){
    int n;
    char start[7];
    uint8_t privateThis[32] = {0};
    uint8_t publicThis[64] = {0};
    uint8_t secret[32] = {0};
    uint8_t externKey[64] = {0};
    const struct uECC_Curve_t * cv = uECC_secp160r1();
    if(!uECC_make_key(publicThis, privateThis, cv)){
        puts("Error: no se pudo generar la clave publica y/o privada\n");
        return -1;
    }
    strcpy(start,"START\n");
    if((n = write(clientfd, start, 6)) <= 0){
        return 0;
    }
    n = read(clientfd,externKey,64);
    if(n<64){
        puts("Error: no se pudo recivir la clave publica externa\n");
        return -1;
    }
    n = write(clientfd,publicThis,64);
    if(n<64){
        puts("Error: no se pudo enviar la clave publica al servidor\n");
        return -1;
    }
    if(!uECC_shared_secret(externKey, privateThis, secret, cv)){
        puts("Failed to generated shared key\n");
        return -1;
    }
    BYTE shaKey[32] = {0};
    getSHA256Key(secret,shaKey);
    //print_hex(shaKey,32);
    //puts("\n");
    blowfish_key_setup(shaKey, key, 32);
    //print_hex(key,32);
    //puts("\n");
    return 1; 
}