CC= /usr/bin/gcc

all:	tcpclient tcpserver 

tcpclient: group03_client.c;
	${CC} group03_client.c -o tcpclient

tcpserver: group03_server.c;
	${CC} group03_server.c -o tcpserver

clean:
	rm tcpclient tcpserver 
