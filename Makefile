CC= /usr/bin/gcc

all: group03_client group03_server

group03_client: group03_client.c;
	${CC} group03_client.c -o group03_client

group03_server: group03_server.c;
	${CC} group03_server.c -o group03_server

clean:
	rm group03_server group03_client
