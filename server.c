/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Username entered: %s\n",buffer);
     
     //verify username

     char ch;
     //2d array to store username and password
     char user_info[10][2];
     
     char str[5000];
     char *ptr;
     char *words[5000];
     FILE * fp = fopen("Authentication.txt", "r");
     fgets(str, 49, fp);             // read 49 characters
     ptr = strtok(str, ",.; ");         // split our findings around the " "
     int i = 0;
     while(ptr != NULL)  // while there's more to the string
     {
         words[i]= ptr;
         i++;
         ptr = strtok(NULL, ",.; "); // and keep splitting
     }
     fclose(fp);

     for(int j=0;j<i;j++) {
         printf("%s\n", words[j]);
     }
     
     if (strcmp(buffer, "bob")){
       n = write(newsockfd,"username accepted",256);
       //printf("%s", buffer);
     } else {
       n = write(newsockfd,"unknown username disconnecting",256);
       printf("%s", buffer);
     }

     if (n < 0) error("ERROR writing to socket");
     return 0;
}
