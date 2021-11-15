/* tcp_ client.c */
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2021 */

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 1024

#define CL_HOSTNAME "localhost"
#define SV_HOSTNAME "localhost"
#define CL_PORTNO 45821
#define SV_PORTNO 46821     /*Phase 1: 46821      Phase 2: 48821*/
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

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct sockaddr_in client_addr;
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port = SV_PORTNO;  /* Port number used by server \
                                                          (remote port) */
   unsigned short client_port = CL_PORTNO;


   message sendMessage, recvMessage;
   //unsigned int msg_len;  /* length of message */
   int bytes_sent, bytes_recd; /* number of bytes sent or received */

   /* I/O setup*/
   FILE *travelFile;
   FILE *tempFile;
   int found = 0;
   char line[250];
   /* stepNumber, svPortNo, svSecretCode, svTravelLocation */
   char *rd_StepNumber, *rd_svPortNo, *rd_svSecretCode, *rd_svTravelLocation;


   // Get hostname,
   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);


for(int i = 48000; i < 49000; i++){
  server_port = i;
   /* open a socket */
   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   // Client side stuff for connection:
   memset(&client_addr, 0, sizeof(client_addr));
   client_addr.sin_family = AF_INET;
   client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   client_addr.sin_port = htons(client_port);

   /* bind local socket and port */
   if (bind(sock_client, (struct sockaddr *) &client_addr,
                                    sizeof (client_addr)) < 0) {
      perror("Client: can't bind to local address\n");
      close(sock_client);
      exit(1);
   }



   /* initialize server address information */
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);



    /* connect to the server */

   if (connect(sock_client, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }

   /* I/O loop */
   travelFile = fopen("./Travel.txt", "r");
   tempFile = fopen("./tempTravel.txt", "w");
   memset(line, 0, 250);
   while (fgets(line, 250, travelFile)){
     rd_StepNumber = strtok(line, ",");
     rd_svPortNo = strtok(NULL, ",");
     rd_svSecretCode = strtok(NULL, ",");
     rd_svTravelLocation = strtok(NULL, ",");

     if(atoi(rd_svPortNo) == server_port){
       found = 1;
       if(atoi(rd_StepNumber) == 1){
         printf("goto step2");
         /* step two */
         sendMessage.stepNumber = htons(2);
         sendMessage.clPortNo = htons(CL_PORTNO);
         sendMessage.svPortNo = htons(atoi(rd_svPortNo));
         sendMessage.svSecretCode = htons(0);
         sendMessage.text[0] = '*';
         sendMessage.text[1] = '\0';

         bytes_sent = send(sock_client, &sendMessage, sizeof(message), 0);
              /* sends new step number, cl port no, sv port no */
         printf("Message sent:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(sendMessage.stepNumber), ntohs(sendMessage.clPortNo),
              ntohs(sendMessage.svPortNo), ntohs(sendMessage.svSecretCode),
              sendMessage.text);

         /* get response from server */
         bytes_recd = recv(sock_client, &recvMessage, sizeof(message), 0);
              /* recvs matching step number, cl port no, sv port no, secret code
                if valid, otherwise recvs prev message */
         printf("Message received:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(recvMessage.stepNumber), ntohs(recvMessage.clPortNo),
              ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
              recvMessage.text);

         fprintf(tempFile, "%d,%d,%d,%s", ntohs(recvMessage.stepNumber),
                    ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
                    recvMessage.text);
       } else if(atoi(rd_StepNumber) == 2){
         printf("goto step3");
         /* step three */
         sendMessage.stepNumber = htons(3);
         sendMessage.clPortNo = htons(CL_PORTNO);
         sendMessage.svPortNo = htons(atoi(rd_svPortNo));
         sendMessage.svSecretCode = htons(atoi(rd_svSecretCode));
         sendMessage.text[0] = '*';
         sendMessage.text[1] = '\0';

         bytes_sent = send(sock_client, &sendMessage, sizeof(message), 0);
              /* sends new step number, cl port no, sv port no, secret code */
         printf("Message sent:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(sendMessage.stepNumber), ntohs(sendMessage.clPortNo),
              ntohs(sendMessage.svPortNo), ntohs(sendMessage.svSecretCode),
              sendMessage.text);

         /* get response from server */
         bytes_recd = recv(sock_client, &recvMessage, sizeof(message), 0);
              /* recvs matching step number, cl port no, sv port no, secret code
                and text if valid, otherwise recvs prev message */
         printf("Message received:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(recvMessage.stepNumber), ntohs(recvMessage.clPortNo),
              ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
              recvMessage.text);

         fprintf(tempFile, "%d,%d,%d,%s", ntohs(recvMessage.stepNumber),
                    ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
                    recvMessage.text);
       } else if(atoi(rd_StepNumber) == 3){
         printf("Already Connected to this 3 times...");
       }
     } else {
       fprintf(tempFile, "%s,%s,%s,%s", rd_StepNumber, rd_svPortNo,
                            rd_svSecretCode, rd_svTravelLocation);
     }
   }

   fclose(travelFile);


   if(!found){
     /* step one */
     sendMessage.stepNumber =  htons(1);
     sendMessage.clPortNo = htons(CL_PORTNO);
     sendMessage.svPortNo = htons(0);
     sendMessage.svSecretCode = htons(0);
     sendMessage.text[0] = '*';
     sendMessage.text[1] = '\0';

     bytes_sent = send(sock_client, &sendMessage, sizeof(message), 0);
          // sends step number, cl port no
     printf("Message sent:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(sendMessage.stepNumber), ntohs(sendMessage.clPortNo),
              ntohs(sendMessage.svPortNo), ntohs(sendMessage.svSecretCode),
              sendMessage.text);

     /* get response from server */
     bytes_recd = recv(sock_client, &recvMessage, sizeof(message), 0);
          // recvs matching step number, cl port no, and sv port no
     printf("Message received:\t%d,\t%d,\t%d,\t%d,\t%s\n",
              ntohs(recvMessage.stepNumber), ntohs(recvMessage.clPortNo),
              ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
              recvMessage.text);

     fprintf(tempFile, "%d,%d,%d,%s", ntohs(recvMessage.stepNumber),
                ntohs(recvMessage.svPortNo), ntohs(recvMessage.svSecretCode),
                recvMessage.text);
   }


   /* close the socket */
   fclose(tempFile);
   system("mv ./tempTravel.txt ./Travel.txt");
   close (sock_client);
 }
}
