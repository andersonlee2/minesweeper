#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

// 22.10.18 1:13am

#define MAXDATASIZE 100 /* max number of bytes we can get at once */
#define ARRAY_SIZE 30
#define RETURNED_ERROR -1
#define PORT 12345


void display_login(int socket_id){
	char username[30];
	char password[30];
	char select[20];
	int a;

	// Login page
	printf("Enter username and password\nUsername: ");
	scanf("%s", username);
	fflush(stdin);
	printf("Password: ");
	scanf("%s", password);

	// Send Username and Password to Server
	send(socket_id, username, 20, 0);
	send(socket_id, password, 20, 0);

}

void display_welcome(int socket_id){
	int menu_option;
	uint16_t selection;

  printf("%s","Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose your option:");
  scanf("%d", &menu_option);

	selection = htons(menu_option);
	send(socket_id, &selection, sizeof(uint16_t), 0);

	if (menu_option == 1){
		send(socket_id, menu_option, 20, 0);
	} else if (menu_option == 2){
		printf("We don't have a leaderboard yet soz\n\n");
    display_welcome(socket_id);
	} else if (menu_option ==3){
		printf("Disconnecting...\n");
		close(socket_id);
		exit(1);
	} else {
		printf("Enter a valid option. Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose:");
	}
}

int main(int argc, char *argv[]) {
	int sockfd, numbytes, port, i=0;
	uint16_t stats;

	struct hostent *he;
	struct sockaddr_in their_addr; /* connector's address information */

	if (argc < 2) {
		fprintf(stderr,"usage: client_hostname port_number\n");
		exit(1);
	}

	if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
		herror("gethostbyname");
		exit(1);
	}

 	if (argc > 2){
		port = atoi(argv[2]);
	}else port = PORT;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}


	their_addr.sin_family = AF_INET;      /* host byte order */
	their_addr.sin_port = htons(port);    /* short, network byte order */
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

	if (connect(sockfd, (struct sockaddr *)&their_addr, \
	sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

	// Display login page
  display_login(sockfd);
	// Display game menu
	recv(sockfd, &stats, sizeof(uint16_t), 0);
	int a = ntohs(stats);
  if(a == 1){
		display_welcome(sockfd);
  }else if(a == 0){
		printf("Incorrect Username or Password. Disconnecting...\n");
		close(sockfd);
	}

	close(sockfd);
	return 0;
}
