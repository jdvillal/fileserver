# Proyecto final Fileserver
Proyecto final; cliente y servidor de subida y descarga encriptada de archivos en Linux usando C para la materia Programación de Sistemas (CCPG1008) de la ESPOL.

## Servidor

El programa servidor *file_server* tiene el siguiente comportamiento:
### *HELP
Si ejecutamos el servidor con la ocpión -h (help), la salida del programa es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_server 8080 -h
file_server uses Blowfish encryption to upload and download files to a
server from connected clients.
Usage:
	file_server [-d] <port>
	file_server -h
Options:
 -h			Help, show this screen.
 -d			Daemon mode.

```
### *Ejecucion en consola
Si ejecutamos el servidor enviando como argumento el puerto 8080, la salida del programa es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_server 8080
server escuchando en puerto 8080...

```
### *Daemon mode
El servidor puede ejecutarse en segundo plano si el usuario lo requiere. Si ejecutamos el servidor enviando como argumento el puerto 8080 y la opción -d, la salida del programa es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_server 8080 -d
Escuchando como Daemon en el puerto 8080

```
### *(OPCIONAL)SYSLOG
El servidor registra eventos en la bitácora general del sistema. Dichos eventos pueden ser: Nuevo cliente conectado o desconectado. Errores lectura o escritura de un archivo. Errores de lectura o escritura en el socket. Solicitudes con comandos desconocidos. Ejemplo:

```
09:39:32 file_server: [File_server| uid:1000 | pid:3242]: Se ha conectado un nuevo cliente.
09:39:26 file_server: [File_server| uid:1000 | pid:3242]: Se ha desconectado un cliente.
09:39:17 file_server: [File_server| uid:1000 | pid:3242]: Se ha conectado un nuevo cliente.
09:37:49 file_server: [File_server| uid:1000 | pid:3242]: Se ha desconectado un cliente.
09:37:46 file_server: [File_server| uid:1000 | pid:3242]: *ERROR* Se ha recibido un comando desconocido.
09:37:46 file_server: [File_server| uid:1000 | pid:3242]: *ERROR* Se ha recibido un comando desconocido.
09:37:40 file_server: [File_server| uid:1000 | pid:3242]: Se ha conectado un nuevo cliente.
09:36:29 file_server: [File_server| uid:1000 | pid:3242]: Se ha desconectado un cliente.
09:36:15 file_server: [File_server| uid:1000 | pid:3242]: Se ha conectado un nuevo cliente.
```

## Cliente

El programa servidor *file_server* tiene el siguiente comportamiento:
### *HELP
Si ejecutamos el cliente con la opción -h (help), la salida del programa es la siguiente:

```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client -h
file_client connects to a remote file_server service and allows the
user to upload and download files from the server.
Usage:
	file_client <ip> <port>
	file_server -h
Options:
-h			Help, show this screen.

```
Si ejecutamos el cliente con la ip y el puerto en el que escucha el servidor, la salida del programa es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexion/encriptacion con el servidor establecida de manera exitosa.

> 

```

### *GET
El caracter ">" indica al usuario que el cliente espera que se ingrese un comando para hacer la solicitud al servidor. Si ingresamos el comando "GET prueba.txt" el cliente enviará la solicitud al servidor para que este último busque y reponsa con dicho archivo. Ejemplo:

```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexion/encriptacion con el servidor establecida de manera exitosa.

> GET prueba.txt
El archivo se ha descargado con exito y se ha guardado con el nombre: download_prueba.txt (143-bytes)

> 
```
Si el servidor encuentra el archivo, lo envia al cliente usando encriptación blowfish, si el archivo solicitado no se encuentra en el servidor, la salida es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexion/encriptacion con el servidor establecida de manera exitosa.

> GET texto.txt
El archivo solicitado no existe.

> 
```
### *PUT
Si el usuario desea subir un archivo al servidor, puede usar el comando "PUT" para ello. La salida de la ejecución del comando "PUT prueba.txt" es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexión/encriptación con el servidor establecida de manera exitosa.

> PUT prueba.txt
El archivo prueba.txt (143 bytes) ha sido enviado al servidor con exito.

> 
```
Si el archivo que el usuario desea subir al servidor no exite, la salida es la siguiente:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexion/encriptacion con el servidor establecida de manera exitosa.

> PUT texto.txt
El archivo que intenta enviar al servidor no existe.

> 
```
### *(OPCIONAL)LIST
También se ha implementado el comando "LIST". El comando permite al servidor responder al cliente con una lista de todos los archivos disponibles en el para desargar. Ejemplo:
```
jorge@jorge:~/Cfiles/proyecto-final-fileserver-borja-villalta$ ./file_client 127.0.0.1 8080
Conexion/encriptacion con el servidor establecida de manera exitosa.

> LIST
Archivos-disponibles-en-el-servidor:
uECC_vli.h
blowfish.c
prueba.txt
fileclient.o
uECC.h
csapp.h
README.md
uECC.c
common.o
types.h
sha256.o
blowfish.o
common.h
curve-specific.inc
sha256.c
prueba.zip
platform-specific.inc
fileserver.o
fileserver.c
sha256.h
fileclient.c
common.c
platform-specific
(copy).inc
csapp.o
uECC.o
csapp.c
blowfish.h
libcrypto.a
> 
```

## Compilación
Para compilar cliente y servidor:
```
$ make
```
Para compilar solo el servidor:
```
$ make server
```
Para compilar solo el client:
```
$ make client
```
Para eliminar archivos temporales .o .a y archivos descargados o subidos durante los test de ejecucion:
```
$ make clean
```

## Integrantes
DARWIN BORJA: Proyecto base (descarga de archivos), archivos con funciones de encriptación y archivos de prueba, implementación del cliente, modificación del archivo readme.
JORGE VILLALTA: Implementación de funciones commmon, implementación del servidor, makefile con compilación de libreria estatica, codigo documentado en comentarios.
