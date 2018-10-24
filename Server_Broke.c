#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

// 22.10.18 1:13am

#define ARRAY_SIZE 30  /* Size of array to receive */
#define BACKLOG 10     /* how many pending connections queue will hold */
#define RETURNED_ERROR -1
#define MYPORT 12345
#define RANDOM_NUMBER_SEED 42
//Minesweeper Defines
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10
#define MINE -2

void display_welcome(){
	int selection;
	uint16_t stats;

	recv(socket_id, &stats, sizeof(uint16_t), 0);
	selection = ntohs(stats);

  printf("The client has sent option %d\n", selection);
	if (selection == 1){
    initialise_client_board();
		place_mines();
    initialise_server_board();
	  game_running = true;
	} else if (menu_option == 2){
		printf("We don't have a leaderboard yet soz\n\n");
    display_welcome();
	}
}

/*int menu(int socket_id){
	int select;
	uint16_t stats;

	// Play Minesweeper, display Leaderboard or Quit
	recv(socket_id, &stats, sizeof(uint16_t), 0);
	select = ntohs(stats);
	printf("The selection made: %d\n", select);
	if(select == 1){
		printf("Play Minesweeper\n");
	}else if(select == 2){
		printf("Display Leaderboard\n");
	}/*else if(select == 3){
		printf("User Quit\n");
		return 0;
	}
	close(socket_id);

}*/

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

void drawboard(){
  int i, j;
  char *p = ".";
  //printf("%s","   012345678\n   =========\n 0|");
	char first_line = ("   012345678\n   =========\n 0|")
	send(socket_id, &first_line, sizeof(first_line), 0);
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){
      //printf("%c",client_board[j][i]);
			send(socket_id, &client_board[j][i], sizeof(client_board[j][i]), 0);
    }
    if(i<NUM_TILES_Y -1){
      printf("\n %d|", i+1);
			char new_row = ("\n %d|", i+1);
			send(socket_id, &client_board[j][i], sizeof(client_board[j][i]), 0);
    }
  }
	char new_line = ("\n");
	send(socket_id, &new_line, sizeof(new_line), 0);
  //printf("\n");
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



int authenticate(int socket_id){
	char username[30];
	char password[30];
	char select[20];
	int auth1 = 0;
	FILE *Authen;
	char username2[30];
	char password2[30];
	int a;
	uint16_t stats;

	// Recieve and print the sent Username and password.
  recv(socket_id, &username, 20, 0);
	printf("User Recieved: %s\n", username);
	recv(socket_id, &password, 20, 0);
	printf("Pass Recieved: %s\n", password);

 	 // Open one file for reading
 	if((Authen = fopen("Authentication.txt", "r")) == NULL){
		perror("NO file");
	}

	// Compare Input with Authentication.txt
	while(fgetc(Authen) != '\n'); //Skip first row
	//fscanf(Authen, "Username %s", "Password %s", username2, password2);
	while(fscanf(Authen, "%s %s\n", username2, password2) > 0){

		if(strcmp(username, username2) == 0 && strcmp(password, password2) == 0){
			printf("Valid Credentials\n");
			int a = 1;
			//printf("a = %d\n",a);
			stats = htons(a);
			//printf("stats = %d\n",stats);
			send(socket_id, &stats, sizeof(uint16_t) , 0);
			display_welcome(socket_id);
			return 1;
		}
	}

	printf("Invalid Credentials, terminate connection\n");
 	fclose(Authen);
	close(socket_id);
	send(socket_id, &stats, sizeof(uint16_t) , 0);
	return 0;

}

void handle_sigint(int sig){
	printf(" Disconnecting from server. Goodbye...\n");
	exit(1);
}

void play_game(int socket_id){
	!valid_option;
	while (!valid_option){
		drawboard();
    printf("\n%d mines left\n", remaining_mines);
		printf("%s", "\nChoose a move:\n<R> Reveal tile\n<F> Place flag\n<Q> Quit\n\nMove(R,F,Q):");
    getchar();
    scanf("%c", &option_input);
		if(option_input == 'r' || option_input == 'f' ||option_input == 'q'){
			  valid_option = true;
		}
	}
  if (option_input == 'r' || option_input == 'f'){
	   printf("\nEnter x coordinate:");
     scanf("%d", &x_input);
		 printf("Enter y coordinate:");
		 scanf("%d", &y_input);
		 if(option_input == 'r'){
       int tile_no = check_tile(x_input, y_input);
			 if (tile_no == -1){
           reveal_mines();
					 client_board[x_input][y_input] = *revealed;
					 drawboard();
					 printf("You have revealed a mine! Game over :(\n\n");
					 game_running = false;
					 getchar();
					 display_welcome();
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
			 valid_option = false; //return to options
		} else if(option_input == 'q'){
			 game_running = false;
			 display_welcome();
		}
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
	/* for every accepted connection, use a seperate process or thread to serve it */
	while(1) {  /* main accept() loop */
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
		&sin_size)) == -1) {
			perror("accept");
			continue;
		}
		printf("server: got connection from %s\n", \
			inet_ntoa(their_addr.sin_addr));

		//Create a thread to accept client

		//pthread_attr_t attr;
		//pthread_attr_init(&attr);
		//pthread_create(&client_thread, &attr, recieve, new_fd);
		authenticate(new_fd);
		//pthread_join(client_thread,NULL);

		/*if (send(new_fd, "All of array data sent by server\n", 40 , 0) == -1)
				perror("send");*/
				while(game_running){
					play_game(new_fd);
				}
	}

	close(new_fd);
}
