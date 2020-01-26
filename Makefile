CC = gcc
CFLAGS = -I -wall

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread ./libcrypto.a

all: client server

client: fileclient.c  csapp.o common.o libcrypto.a
	$(CC) -c fileclient.c
	$(CC) $(CFLAGS) -o file_client fileclient.o common.o csapp.o $(LIB)
server: fileserver.c csapp.o common.o
	$(CC) -c fileserver.c
	$(CC) $(CFLAGS) -o file_server fileserver.o common.o csapp.o $(LIB)

csapp: csapp.c
	$(CC) -c csapp.c

common: common.c csapp.o libcrypto
	$(CC) $(CFLAGS) cab.c

sha256: sha256.c
	$(CC) -c sha256.c

blowfish: blowfish.c
	$(CC) -c blowfish.c

uECC: ./micro-ecc/uECC.c
	$(CC) -c ./micro-ecc/uECC.c

libcrypto.a: uECC sha256.o blowfish.o 
	ar rcs libcrypto.a uECC.o blowfish.o sha256.o

clean:
	rm -rf file_server file_client *.o *.a upload_*.* download_*.*

