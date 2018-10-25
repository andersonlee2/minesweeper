//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

#define MAXDATASIZE 100 /* max number of bytes we can get at once */
#define ARRAY_SIZE 30
#define RETURNED_ERROR -1
#define PORT 12345

char server_board[NUM_TILES_X][NUM_TILES_Y];
char client_board[NUM_TILES_X][NUM_TILES_Y];
char *mine = "*";
char *revealed = "x";
char *flag = "+";
char *space = "-";
char option_input;

int mine_positions[NUM_MINES][2];
int menu_option;

bool valid_option, game_running;
bool tile_revealed[NUM_TILES_X][NUM_TILES_Y];

bool tile_contains_mine(int x, int y){
  if(server_board[x][y]== *mine){ //change to == space
    return true;
  } else {
    return false;
  }
}

void initialise_client_board(){
	for (int i=0; i< NUM_TILES_X; i++){
	    for (int j=0; j< NUM_TILES_Y ; j++){
	    	client_board[j][i]= *space;
        server_board[j][i]= *space;
	    }
	}
}

void drawboard(int socket_id){
  char tile;
  printf("%s","   012345678\n   =========\n 0|");
  for (int i=0; i< NUM_TILES_X; i++){
    for (int j=0; j< NUM_TILES_Y ; j++){
      recv(socket_id, &tile, 1,0);
      printf("%c",tile);
      //fflush(stdin);
    }
    if(i<NUM_TILES_Y -1){
      printf("\n %d|", i+1);
    }
}
  printf("\n");
  fflush(stdin);
}

void display_welcome(int socket_id){
  int menu_option;
  uint16_t status;

  printf("%s","Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose your option:");
  scanf("%d", &menu_option);
  status = htons(menu_option);

	if (menu_option == 1 || menu_option == 2){
    send(socket_id, &status, sizeof(uint16_t), 0);
    game_running =true;
	} else if (menu_option ==3){
    printf("Quitting adios!\n");
    close(socket_id);
		exit(1);
	} else {
		printf("\nEnter a valid option:\n Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose:");
	}
}

void display_login(int socket_id){
  char username[30];
	char password[30];
	char select[20];
	int match;
  uint16_t status;

  printf("Enter login details:\n");
	printf("Username: ");
	scanf("%s", username);
	fflush(stdin);
	printf("Password: ");
	scanf("%s", password);

	// Send Username and Password to Server
	send(socket_id, username, 20, 0);
	send(socket_id, password, 20, 0);

  recv(socket_id, &status, sizeof(uint16_t), 0);
  match = ntohs(status);
  if(match == 1){
		display_welcome(socket_id);
  }else if(match == 0){
		printf("Incorrect Username or Password. Disconnecting...\n");
		close(socket_id);
	}
  fflush(stdin);
}

void reveal_mines(){
  int x, y;
  for (int i=0; i< NUM_MINES; i++){
    client_board[mine_positions[i][0]][mine_positions[i][1]] = *mine;
  }
}

void play_game(int sockfd){

  //int remaining_mines;
  int x_input;
  int y_input;
  int outcome;
  uint16_t status;
  while(game_running){
	  !valid_option;
	  	while (!valid_option){
	  		drawboard(sockfd);
	  		fflush(stdin);
	  		recv(sockfd, &status, sizeof(uint16_t),0); //receive no. of mines
	  		int remaining_mines = ntohs(status);
	  		printf("\n%d mines left\n", remaining_mines);
	  		fflush(stdin);
	  		printf("%s", "\nChoose a move:\n<R> Reveal tile\n<F> Place flag\n<Q> Quit\n\nMove(R,F,Q):");
	  		getchar();
	  		scanf("%c", &option_input);
	      
	  		if(option_input == 'r' || option_input == 'f' ||option_input == 'q'){
	  			 send(sockfd, &option_input,1,0); //send option input
	  			 fflush(stdin);
	  			 valid_option = true;
	  		} else {
	  			printf("Choose a valid move:");
	  		}
	  	}

	    if (option_input == 'r' || option_input == 'f'){
	  	printf("\nEnter x coordinate:");
	      scanf("%d", &x_input);
	      fflush(stdin);
	  	printf("Enter y coordinate:");
	  	scanf("%d", &y_input);
	  	status = htons(x_input);
	  	send(sockfd, &status, sizeof(uint16_t),0);
	      fflush(stdin);
	      status = htons(y_input);
	      send(sockfd, &status, sizeof(uint16_t),0);
	      fflush(stdin);

	       recv(sockfd, &status, sizeof(uint16_t),0);
	       outcome = ntohs(status);
	       if (outcome == -1){ //mine revealed game over
	         drawboard(sockfd);
	         printf("You have revealed a mine! Game over :(\n\n");
	         game_running = false;
	         getchar();
	         display_welcome(sockfd);
	       } else if(outcome == 0){ //safe tile clicked

	       }
	  		 valid_option = false; //return to options
	  		}if(option_input == 'q'){
	  			 game_running = false;
	  			 display_welcome(sockfd);
	  		}
  }
	
}

int main(int argc, char *argv[]) {
  int sockfd, numbytes, port, i=0;
	uint16_t stats;
  srand(RANDOM_NUMBER_SEED);

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
  
  //while(game_running){
    play_game(sockfd);
  //}

	close(sockfd);
	return 0;

}
