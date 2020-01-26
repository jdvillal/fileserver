#include "csapp.h"
#include "uECC.h"
#include "sha256.h"
#include "blowfish.h"

void print_hex(const unsigned char* data, size_t size);

void getSHA256Key(uint8_t* toHashKey, unsigned char* newKey);

int getBlowfishKey(uint8_t  *externKey, uint8_t *publicThis, BLOWFISH_KEY *finalKey);

void removeln(char *linea);

char **parse_comand(char *linea, char *delim);

void connection_error(int connfd);

int encryptAndSend(int connfd, int toReadfd, long filesize, BLOWFISH_KEY *key);

int receiveAndDecrypt(int connfd, int toWritefd, long filesize, BLOWFISH_KEY *key);

long validateFile(char* filename);

int safeRead(int fd,char *buffer, int toReadSize);

void addln(char *linea);

char * spaceConcat(char* str1, char *str2);