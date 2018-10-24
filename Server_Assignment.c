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

// Structures
typedef struct{
	int adjacent_mines;
	bool revealed;
	bool is_mine;
	bool is_flagged;
} Tile;

typedef struct{
	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
	bool game_over;
	bool game_won;
	int mines_left;
	time_t start_time;
	time_t end_time;
} GameState;


bool reveled_tile(GameState* gamestate, int x, int y){
	return gamestate->tiles[x][y].revealed;
}

bool tile_contains_mine(GameState* gamestate, int x, int y){
	return gamestate->tiles[x][y].is_mine;
}

bool flagged_tile(GameState* gamestate, int x, int y){
	return gamestate->tiles[x][y].is_flagged;
}

// Algorithm specified in brief
void place_mines(GameState* gamestate){
	for (int i = 0; i < NUM_MINES; i++) {
		int x, y;
		do {
			x = rand() % NUM_TILES_X;
			y = rand() % NUM_TILES_Y;
		} while(tile_contains_mine(gamestate,x,y));

		// put mines down
		gamestate->tiles[x][y].is_mine;
		// Adjacent mines
		if(x > 0 && y > 0)
		{ // top left
			gamestate->tiles[x-1][y-1].adjacent_mines++;
		}else if(y > 0){ // above
			gamestate->tiles[x][y-1].adjacent_mines++;
		}
		else if(x < NUM_TILES_X-1 && y > 0){// top right
			gamestate->tiles[x+1][y-1].adjacent_mines++;
		}
		else if(x > 0){ // left
			gamestate->tiles[x-1][y].adjacent_mines++;
		}
		else if(x < NUM_TILES_X-1){// right
			gamestate->tiles[x+1][y].adjacent_mines++;
		}
		else if (x > 0 && y < NUM_TILES_Y-1) {// bottom left
			gamestate->tiles[x-1][y+1].adjacent_mines++;
		}
		else if (y < NUM_TILES_Y-1) {// bottom
			gamestate->tiles[x][y+1].adjacent_mines++;
		}
		else if (x < NUM_TILES_X-1 && y < NUM_TILES_Y-1) {// bottom right
			gamestate->tiles[x+1][y+1].adjacent_mines++;
		}
	}
}

int reveal_tile(GameState* gamestate, int x, int y){
	if (tile_contains_mine(gamestate, x, y)) {
		return MINE;
	}


}

int menu(int socket_id){
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
	}*/
	close(socket_id);

}

int recieve(int socket_id){
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
			menu(socket_id);
			return 1;
		}
	}

	printf("Invalid Credentials, termiate connection\n");
 	fclose(Authen);
	close(socket_id);
	send(socket_id, &stats, sizeof(uint16_t) , 0);
	return 0;

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
		printf("server: got connection from %s\n", \
			inet_ntoa(their_addr.sin_addr));

		//Create a thread to accept client

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&client_thread, &attr, recieve, new_fd);

		pthread_join(client_thread,NULL);

		/*if (send(new_fd, "All of array data sent by server\n", 40 , 0) == -1)
				perror("send");*/

	}

	close(new_fd);
}
