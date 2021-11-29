/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2021 */

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

//#define SERV_TCP_PORT 65000

#define CL_HOSTNAME "localhost"
#define SV_HOSTNAME "localhost"
#define CL_PORTNO 45821
#define SV_PORTNO 48821     /*Phase 1: 46821      Phase 2: 48821*/
#define CL_VISITORNAME "Davis-Joseph"
#define SV_TRAVELLOCATION "Wilmington"
#define SV_SECRETCODE 4242

typedef struct message{
  unsigned short stepNumber;
  unsigned short clPortNo;
  unsigned short svPortNo;
  unsigned short svSecretCode;
  char text[80];
} message;

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port = SV_PORTNO;  /* Port number used by server (local port) */

   unsigned short rc_sv_port = SV_PORTNO;

   unsigned short client_port = CL_PORTNO;  //CLient port

   unsigned short rc_secret = SV_SECRETCODE;

   char send_text[80] = SV_TRAVELLOCATION;

   unsigned short step_no = 0;

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char sentence[STRING_SIZE];  /* receive message */
   char modifiedSentence[STRING_SIZE]; /* send message */
   unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */

   message sendMessage, recvMessage;

   FILE *visitorFile;
   FILE *tempFile;
   int cmdSize = sizeof("cat Visitor.txt | sed -e '/xxxxx/d' > tempVisitor.txt");
   char command[cmdSize];

   int found = 0;

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);
   }

   /* initialize server address information */

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */
   server_port = SV_PORTNO; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   //printf("I am here to listen ... on port %hu\n\n", server_port);

   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr,
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n");
         close(sock_server);
         exit(1);
      }

      /* receive the message */

      sendMessage.svPortNo = htons(server_port);

      bytes_recd = recv(sock_connection, &recvMessage, sizeof(message), 0);

      client_port = ntohs(recvMessage.clPortNo);

      step_no = ntohs(recvMessage.stepNumber);

      rc_sv_port = ntohs(recvMessage.svPortNo);

      rc_secret = ntohs(recvMessage.svSecretCode);
      snprintf(command, cmdSize, "cat Visitor.txt | sed -e '/%d/d' > tempVisitor.txt", client_port);
      system(command);
      //printf("test: %s", command); //DB
      visitorFile = fopen("./Visitor.txt", "r");
      tempFile = fopen("./tempVisitor.txt", "a");

      if(step_no == 1) {
              /* step 1 */
              sendMessage.stepNumber = htons(1);
              sendMessage.clPortNo = htons(client_port);
              sendMessage.svSecretCode = htons(0);
              sendMessage.text[0] = '*';
              sendMessage.text[1] = '\0';

              bytes_sent = send(sock_connection, &sendMessage, sizeof(message), 0);

              fprintf(tempFile, "%d,%d,%s\n", step_no, client_port, recvMessage.text);
      }

      else if (step_no == 2) {
              /* step nu 2 */
              //sendMessage.stepNumber = htons(1);
              sendMessage.clPortNo = htons(client_port);
              //sendMessage.svSecretCode = htons(0);
              sendMessage.text[0] = '*';
              sendMessage.text[1] = '\0';

              if(rc_sv_port == server_port) {
                      sendMessage.stepNumber = htons(2);
                      sendMessage.svSecretCode = htons(SV_SECRETCODE);
              }

              else {
                      sendMessage.stepNumber = htons(1);
                      sendMessage.svSecretCode = htons(0);
              }

              bytes_sent = send(sock_connection, &sendMessage, sizeof(message), 0);

              fprintf(tempFile, "%d,%d,%s\n", step_no, client_port, recvMessage.text);
      }

      else {
              /* step 3 */
              sendMessage.clPortNo = htons(client_port);

              if(rc_sv_port == server_port && rc_secret == SV_SECRETCODE) {
                      sendMessage.stepNumber = htons(3);
                      sendMessage.svSecretCode = htons(SV_SECRETCODE);
                      strcpy(sendMessage.text, send_text);
              }

              else {
                      sendMessage.stepNumber = htons(1);
                      sendMessage.svSecretCode = htons(0);
                      sendMessage.text[0] = '*';
                      sendMessage.text[1] = '\0';
              }

              bytes_sent = send(sock_connection, &sendMessage, sizeof(message), 0);

              fprintf(tempFile, "%d,%d,%s\n", step_no, client_port, recvMessage.text);
      }

      /* close the socket */
      fclose(visitorFile);
      fclose(tempFile);
      system("mv ./tempVisitor.txt ./Visitor.txt");
      close(sock_connection);
   }
}
