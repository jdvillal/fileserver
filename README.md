# Práctica uso de sockets
Práctica sobre uso básico de sockets en Linux usando C para la materia Programación de Sistemas (CCPG1008) de la ESPOL.

## Instrucciones
La práctica consiste en crear una aplicación cliente - servidor para descargar archivos desde un servidor remoto. La aplicación debe implementar la descarga remota de archivos de la siguiente manera:
1. El cliente debe preguntar al usuario el archivo que desea descargar desde un prompt en consola.
2. El cliente debe enviar esta información al servidor a traves de un socket TCP/IP.
3. El servidor revisa si el nombre del archivo recibido corresponde a un archivo valido y disponible.
4. El servidor debe responder con el tamaño del archivo o cero si el archivo no existe.
5. El cliente debe responder con "READY" indicando al servidor que esta listo para recibir el archivo completo.
6. El servidor debe enviar el archivo completo a traves del socket.
7. El cliente debe recibir el archivo y grabarlo con el mismo nombre en el disco duro.

Toda comunicación entre cliente y servidor es enviada en modo ASCII con salto de línea al final, excepto la transmisión del contenido del archivo, el cual debe ser enviado en binario sin ninguna transformación.

El programa servidor *server* tiene el siguiente comportamiento:
```
$ ./server -h
Servidor simple de descarga de archivos.
uso:
 ./server <puerto>
 ./server -h
Opciones:
 -h			Ayuda, muestra este mensaje
```

El programa cliente *client* tiene el siguiente comportamiento:
```
$ ./client -h
Cliente simple de descarga de archivos.
uso:
 ./client <hostname> <puerto>
 ./client -h
Opciones:
 -h			Ayuda, muestra este mensaje
```

## Código ejemplo

Asumiendo que el servidor esta corriendo en una maquina con la IP 192.168.100 en el puerto 8080 y que tiene un archivo *prueba.txt* con 65 bytes de tamaño. Ejemplo de ejecución del cliente:
```
$ ./client 192.168.100 8080
Conectado exitosamente a 192.168.100 en el puerto 8080.
Ingrese el nombre del archivo a descargar: prueba.txt
Descargando y grabando en disco prueba.txt (65 bytes)...
Ingrese el nombre del archivo a descargar:
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
Para compilar cliente y servidor facilitando la depuración con gdb:
```
$ make debug
```
Para compilar cliente y servidor habilitando la herramienta AddressSanitizer, facilita la depuración en tiempo de ejecución:
```
$ make sanitize
```
## Integrantes
DARWIN BORJA: Operaciones que se deben realizar en client.c y arreglo del readme.md
JORGE VILLALTA: Operaciones que se deben realizar en server.c, Modificación del makefile y el gitignore
