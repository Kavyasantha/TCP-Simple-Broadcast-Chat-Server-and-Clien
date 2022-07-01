all: client server

client: client.c SBCPlib.h
	gcc -o client client.c

server: server.c SBCPlib.h
	gcc -o server server.c
	
clean:
	rm server client
