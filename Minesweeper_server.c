//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define ARRAY_SIZE 30  /* Size of array to receive */
#define BACKLOG 10     /* how many pending connections queue will hold */
#define RETURNED_ERROR -1
#define MYPORT 12345

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

char server_board[NUM_TILES_X][NUM_TILES_Y];
char client_board[NUM_TILES_X][NUM_TILES_Y];
char *mine = "*";
char *revealed = "x";
char *flag = "+";
char *space = "-";
//char option_input;

int mine_positions[NUM_MINES][2];
int remaining_mines = NUM_MINES;
int x_input, y_input, menu_option;

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
      tile = client_board[j][i];
      //fflush(stdin);
      printf("%c",tile);
      send(socket_id, &tile, 1,0);
    }
    if(i<NUM_TILES_Y -1){
      printf("\n %d|", i+1);
    }
  }
  printf("\n");
  fflush(stdin);
}

void place_mines(){
  for (int i=0; i < NUM_MINES; i++){
    int x,y;
    do {
      x = rand() % NUM_TILES_X;
      y = rand() % NUM_TILES_Y;

    } while (tile_contains_mine(x,y));
    // place mine and (x,y)
    mine_positions[i][0] = x;
    mine_positions[i][1] = y;
    server_board[x][y] = *mine;
    remaining_mines = NUM_MINES;
  }
}

bool is_valid(int x, int y){
    return (x >= 0) && (x < NUM_TILES_X) &&
           (y >= 0) && (y < NUM_TILES_Y);
}

int check_tile(int x, int y){
	int surrounding_tiles = 0;
	if (tile_contains_mine(x,y)){
		surrounding_tiles = -1;
	} else {
		if(tile_contains_mine(x+1,y) && is_valid(x+1,y)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y) && is_valid(x-1,y)) {
			surrounding_tiles++;
		} if(tile_contains_mine(x,y-1) && is_valid(x,y-1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x,y+1) && is_valid(x,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x+1,y+1) && is_valid(x+1,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y+1) && is_valid(x-1,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x+1,y-1) && is_valid(x+1,y-1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y-1) && is_valid(x-1,y-1)){
			surrounding_tiles++;
		}
	}
	return surrounding_tiles;
}

void initialise_server_board(){
  for (int i=0; i< NUM_TILES_X; i++){
	    for (int j=0; j< NUM_TILES_Y; j++){
        int tile_no = check_tile(i,j);
        char tile_no_c = '0' + tile_no;
        if(server_board[i][j] != *mine){
          server_board[i][j] = tile_no_c;
        }
	    }
	}
}

void display_welcome(int socket_id){
  int menu_option;
  uint16_t status;

  recv(socket_id, &status, sizeof(uint16_t),0);
  menu_option = ntohs(status);
  fflush(stdin);
  printf("Client has selected option %d\n", menu_option);
	if (menu_option == 1){
    printf("In game...\n");
    initialise_client_board();
	place_mines();
    initialise_server_board();
	  game_running = true;
	} else if (menu_option == 2){
		printf("We don't have a leaderboard yet soz\n\n");
	}
  //close(socket_id);
}

void display_login(int socket_id){
  char entered_username[30];
	char entered_password[30];
	char select[20];
	int auth1 = 0;
	FILE *f;
	char valid_username[30];
	char valid_password[30];
	int match;
	uint16_t status;

	// Recieve and print the sent Username and password.
  recv(socket_id, &entered_username, 20, 0);
	printf("Username entered: %s\n", entered_username);
	recv(socket_id, &entered_password, 20, 0);
	printf("Password entered: %s\n", entered_password);

   	 // Open one file for reading
   	if((f = fopen("Authentication.txt", "r")) == NULL){
		perror("Authentcation file not found");
	}

	// Compare Input with Authentication.txt
	while(fgetc(f) != '\n'); //Skip first row
	//fscanf(Authen, "Username %s", "Password %s", username2, password2);
	while(fscanf(f, "%s %s\n", valid_username, valid_password) > 0){

		if(strcmp(entered_username, valid_username) == 0 && strcmp(entered_password, valid_password) == 0){
			printf("Login successful\n");
			int match = 1;
			status = htons(match);
			send(socket_id, &status, sizeof(uint16_t) , 0);
			fflush(stdin);
			display_welcome(socket_id);
		}
	}

	//printf("Invalid Credentials, Disconnecting\n");
 	//fclose(f);
	//close(socket_id);
	//send(socket_id, &status, sizeof(uint16_t) , 0);
}

void reveal_tile(int x ,int y){
  int tile_no = check_tile(x, y);
  char tile_no_c = '0' + tile_no;
  if(is_valid(x,y)){
    client_board[x][y] = tile_no_c;
  }
}

void open_safe_tiles(int x, int y){
  if ((check_tile(x,y)) == 0){
    reveal_tile(x-1,y);
    if (server_board[x-1][y] == '0' && tile_revealed[x-1][y] == false && is_valid(x-1,y)){
        tile_revealed[x-1][y] = true;
        open_safe_tiles(x-1,y);
      }
      reveal_tile(x,y+1);
      if (server_board[x][y+1] == '0' && tile_revealed[x][y+1] == false && is_valid(x,y+1)){
        tile_revealed[x][y+1] = true;
        open_safe_tiles(x,y+1);
      }
      reveal_tile(x,y-1);
      if (server_board[x][y-1] == '0' && tile_revealed[x][y-1] == false && is_valid(x,y-1)){
        tile_revealed[x][y-1] = true;
        open_safe_tiles(x,y-1);
      }
      reveal_tile(x+1,y);
      if (server_board[x+1][y] == '0' && tile_revealed[x+1][y] == false && is_valid(x+1,y)){
        tile_revealed[x+1][y] = true;
        open_safe_tiles(x+1,y);
      }
      reveal_tile(x-1,y-1);
      if (server_board[x-1][y-1] == '0'  && tile_revealed[x-1][y-1] == false && is_valid(x-1,y-1)){
        tile_revealed[x-1][y-1] = true;
        open_safe_tiles(x-1,y-1);
      }
      reveal_tile(x+1,y-1);
      if (server_board[x+1][y-1] == '0' && tile_revealed[x+1][y-1] == false && is_valid(x+1,y-1)){
        tile_revealed[x+1][y-1] = true;
        open_safe_tiles(x+1,y-1);
      }
      reveal_tile(x+1,y+1);
      if (server_board[x+1][y+1] == '0'  && tile_revealed[x+1][y+1] == false && is_valid(x+1,y+1)){
        tile_revealed[x+1][y+1] = true;
        open_safe_tiles(x+1,y+1);
      }
      reveal_tile(x-1,y+1);
      if (server_board[x-1][y+1] == '0'  && tile_revealed[x-1][y+1] == false && is_valid(x-1,y+1)){
        tile_revealed[x-1][y+1] = true;
        open_safe_tiles(x-1,y+1);
      }
  }
}

void reveal_mines(){
  int x, y;
  for (int i=0; i< NUM_MINES; i++){
    client_board[mine_positions[i][0]][mine_positions[i][1]] = *mine;
  }
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
        ;
}

void play_game(int socket_id){
  char option_input;
  int x_input;
  int y_input;
  int outcome;
  uint16_t status;

  while(game_running){
	  //fflush(stdin);
	  	drawboard(socket_id);
	      status = htons(remaining_mines);
	      send(socket_id, &status, sizeof(uint16_t),0); //send no. of mines
	      //fflush(stdin);
	      printf("\nIn game with %d mines left\n", remaining_mines);
	      recv(socket_id, &option_input, 1,0); //recieve option input
	      fflush(stdin);
	  	printf("Client has chosen: %c",option_input);
	      //fflush(stdin);
	       recv(socket_id, &status, sizeof(uint16_t),0);
	       x_input = ntohs(status);
	       //fflush(stdin);
	       printf("\nx value received: %d\n", x_input);
	       recv(socket_id, &status, sizeof(uint16_t),0);
	       y_input = ntohs(status);
	       printf("\ny value received: %d\n", y_input);
	       fflush(stdin);
	  		 if(option_input == 'r'){
	         int tile_no = check_tile(x_input, y_input);
	  			 if (tile_no == -1){
	             reveal_mines();
	  					 client_board[x_input][y_input] = *revealed;
	             outcome = -1;
	             status = htons(outcome);
	             send(socket_id, &status, sizeof(uint16_t),0);
	  					 drawboard(socket_id);
	  					 printf("Mine revealed, Game over!\n\n");
	  					 display_welcome(socket_id);
	             game_running = false;
	  			 }
	         reveal_tile(x_input, y_input);
	  			 open_safe_tiles(x_input, y_input);
	  		   } else {
	           if(server_board[x_input][y_input] == *mine){
	              server_board[x_input][y_input] = *flag; //fix
	              client_board[x_input][y_input] = *flag;
	             remaining_mines--;
	           } else if(client_board[x_input][y_input] == *flag){
	             printf("\nYou have already flagged this tile\n");
	           } else {
	             printf("\nNo mine there try again\n");
	           }
	           if(remaining_mines == 0){
	             printf("Ya won dingus :D\n");
	             game_running = false;
	           }
	  	}
  }
  
		
  }

void handle_sigint(int sig){
	printf(" Disconnecting from server. Goodbye...\n");
	exit(1);
}

int main(int argc, char *argv[]) {



  	srand(RANDOM_NUMBER_SEED); // seed the random number generator
  	// SIGINT
  	signal(SIGINT, handle_sigint);

  	/* Thread and thread attributes */
  	pthread_t client_thread;
  	pthread_attr_t attr;


  	int sockfd, new_fd, port;  /* listen on sock_fd, new connection on new_fd */
  	struct sockaddr_in my_addr;    /* my address information */
  	struct sockaddr_in their_addr; /* connector's address information */
  	socklen_t sin_size;
  	int i=0;

  	/* Get port number for server to listen on */
  	if (argc < 1) {
  		fprintf(stderr,"usage: client port_number\n");
  		exit(1);
  	}

  	/* generate the socket */
  	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  		perror("socket");
  		exit(1);
  	}

  	if (argc > 1){
  		port = atoi(argv[1]);
  	}else port = MYPORT;

  	/* generate the end point */
  	my_addr.sin_family = AF_INET;         /* host byte order */
  	my_addr.sin_port = htons(port);     /* short, network byte order */
  	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  		/* bzero(&(my_addr.sin_zero), 8);   ZJL*/     /* zero the rest of the struct */

  	/* bind the socket to the end point */
  	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
  	== -1) {
  		perror("bind");
  		exit(1);
  	}

  	/* start listnening */
  	if (listen(sockfd, BACKLOG) == -1) {
  		perror("listen");
  		exit(1);
  	}

  	printf("server starts listnening ...\n");

  	/* repeat: accept, send, close the connection */
  	/* for every accepted connection, use a sepetate process or thread to serve it */
  	while(1) {  /* main accept() loop */
  		sin_size = sizeof(struct sockaddr_in);
  		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
  		&sin_size)) == -1) {
  			perror("accept");
  			continue;
  		}
  		printf("Connection from %s establised\n", \
  			inet_ntoa(their_addr.sin_addr));
  		
       
        
          display_login(new_fd);

          //while(game_running){
            play_game(new_fd);
          //}

  	}

  	close(new_fd);


  return 0;
}
