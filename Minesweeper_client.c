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

int mine_positions[NUM_MINES][2], remaining_mines = NUM_MINES;
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
      recv(socket_id, &tile,1,0);
      printf("%c",tile);
    if(i<NUM_TILES_Y -1){
      printf("\n %d|", i+1);
    }
  }
  printf("\n");
}

void display_login(int socket_id){
  char username[30];
	char password[30];
	char select[20];
	int a;

  printf("Enter login details:\n");
	printf("Username: ");
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
  uint16_t status;

  printf("%s","Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose your option:");
  scanf("%d", &menu_option);
  status = htons(menu_option);

	if (menu_option == 1 || menu_option == 2){
    send(socket_id, &status, sizeof(uint16_t), 0);
	} else if (menu_option ==3){
    printf("Quitting adios!\n");
    close(socket_id);
		exit(1);
	} else {
		printf("Enter a valid option. Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose:");
	}
}

/*void reveal_tile(int x ,int y){
  int tile_no = check_tile(x, y);
  char tile_no_c = '0' + tile_no;
  if(is_valid(x,y)){
    client_board[x][y] = tile_no_c;
  }
}*/

/*void open_safe_tiles(int x, int y){
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
}*/

void reveal_mines(){
  int x, y;
  for (int i=0; i< NUM_MINES; i++){
    client_board[mine_positions[i][0]][mine_positions[i][1]] = *mine;
  }
}

/*void play_game(int sockfd){
	!valid_option;
	while (!valid_option){
		drawboard(sockfd);
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
					 drawboard(sockfd);
					 printf("You have revealed a mine! Game over :(\n\n");
					 game_running = false;
					 getchar();
					 display_welcome(sockfd);
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
			 display_welcome(sockfd);
		}
}*/

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
	// Display game menu
	recv(sockfd, &stats, sizeof(uint16_t), 0);
	int a = ntohs(stats);
  if(a == 1){
		display_welcome(sockfd);
  }else if(a == 0){
		printf("Incorrect Username or Password. Disconnecting...\n");
		return 0;
	}

  //while(game_running){
    //play_game();
  //}
  drawboard(sockfd);

	close(sockfd);
	return 0;

}
