all: server client 

server: server.c 
	gcc -g server.c -o server

client: client.c
	gcc -g client.c -o client

clean:
	rm -rf server client
