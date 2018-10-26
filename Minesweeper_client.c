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
#include <time.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10
#define PORT 12345

bool game_running;

//Leaderbaord elements
char Username[30];
double time_taken;
int games_won;
int games_played;

void drawboard(int socket_id) {
  char tile;
  printf("%s", "   012345678\n   =========\n 0|");
  for (int i = 0; i < NUM_TILES_X; i++) {
    for (int j = 0; j < NUM_TILES_Y; j++) {
      recv(socket_id, & tile, 1, 0);
      printf("%c", tile);
    }
    if (i < NUM_TILES_Y - 1) {
      printf("\n %d|", i + 1);
    }
  }
  printf("\n");
  fflush(stdin);
}

//Displays options for users after successfully authenticatiing
void display_welcome(int socket_id) {
  int menu_option;
  uint16_t status;
  fflush(stdin);
  printf("%s", "Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose your option:");
  scanf("%d", & menu_option);
  status = htons(menu_option);

  if (menu_option == 1 || menu_option == 2) { //1 = playgame, 2= display leaderboard
    send(socket_id, & status, sizeof(uint16_t), 0);
    fflush(stdin);
    if(menu_option == 1){
      game_running = true;
    } else {
      printf(" There's no leaderboard here because you don't need to compare yourself with others on there. You're talented in you're own way <3\n");
      fflush(stdin);
      display_welcome(socket_id);
      fflush(stdin);
    }
  } else if (menu_option == 3) { //Quit
    printf("Disconnecting.. Adios!\n");
    close(socket_id);
    exit(1);
  } else {
    printf("\nEnter a valid option:\n Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose:");
  }
}

void display_login(int socket_id) {
  char username[30];
  char password[30];
  char select[20];
  int match;
  uint16_t status;

  //Prompt for login details
  printf("Enter login details:\n");
  printf("Username: ");
  scanf("%s", username);
  fflush(stdin);
  printf("Password: ");
  scanf("%s", password);

  // Send Username and Password to Server
  send(socket_id, username, 20, 0);
  send(socket_id, password, 20, 0);

  //Receive and integer indicting valid login credentials. 1=valid
  int a = recv(socket_id, & status, sizeof(uint16_t), 0);
  match = ntohs(status);
  fflush(stdin);
  if (match == 1) {
    strcpy(Username, username); //Save username for leaderboard
    display_welcome(socket_id);
  } else {
    printf("Incorrect Username or Password. Disconnecting...\n");
    close(socket_id);
  }
  fflush(stdin);
}

void play_game(int sockfd) {
  char option_input;
  int x_input;
  int y_input;
  int outcome;
  bool valid_option;
  uint16_t status;
  clock_t start_time;
  time_taken = 0;

  start_time = clock();
  while (game_running) {
    !valid_option;
    while (!valid_option) {
      fflush(stdin);

      //receive board and no. of mines
      drawboard(sockfd);
      fflush(stdin);
      recv(sockfd, & status, sizeof(uint16_t), 0);
      int remaining_mines = ntohs(status);
      printf("\n%d mines left\n", remaining_mines);
      fflush(stdin);

      //Prompt for option input
      printf("%s", "\nChoose a move:\n<R> Reveal tile\n<F> Place flag\n<Q> Quit\n\nMove(R,F,Q):");
      getchar();
      scanf("%c", & option_input);
      if (option_input == 'r' || option_input == 'f' || option_input == 'q' ||
        option_input == 'R' || option_input == 'F' || option_input == 'Q') {
        send(sockfd, & option_input, 1, 0); //send option input
        fflush(stdin);
        valid_option = true;
      }
    }

    if (option_input == 'r' || option_input == 'f' ||
      option_input == 'R' || option_input == 'F') {
      printf("\nEnter x coordinate:");
      scanf("%d", & x_input);
      fflush(stdin);
      printf("Enter y coordinate:");
      scanf("%d", & y_input);
      status = htons(x_input);
      send(sockfd, & status, sizeof(uint16_t), 0);
      fflush(stdin);
      status = htons(y_input);
      send(sockfd, & status, sizeof(uint16_t), 0);
      fflush(stdin);

      //Receive outcome integer
      recv(sockfd, & status, sizeof(uint16_t), 0);
      outcome = ntohs(status);
      fflush(stdin);
      if (outcome == 0) { //mine revealed game over
        printf("You have revealed a mine! Game over :(\n\n");
        game_running = false;
        games_played++;
        drawboard(sockfd);
        display_welcome(sockfd);
        //fflush(stdin);
        //getchar();
      } else if (outcome == 3) { //Already flagged
        printf("\nYou have already flagged this tile\n");
      } else if (outcome == 4) {
        printf("\nNo mine there try again\n"); //No mine at flagged
      } else if (outcome == 5) {
        time_taken = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Congratulations! You cleared all the mines in %f seconds\n", time_taken);
        game_running = false;
        games_won++;
        drawboard(sockfd);
        display_welcome(sockfd);
      }
      valid_option = false; //return to options
    }
    if (option_input == 'q') {
      game_running = false;
      games_played++;
      display_welcome(sockfd);
    }
  }
}

int main(int argc, char * argv[]) {
  int sockfd, numbytes, port, i = 0;
  uint16_t stats;
  srand(RANDOM_NUMBER_SEED);

  struct hostent * he;
  struct sockaddr_in their_addr; /* connector's address information */

  if (argc < 2) {
    fprintf(stderr, "usage: client_hostname port_number\n");
    exit(1);
  }

  if ((he = gethostbyname(argv[1])) == NULL) { /* get the host info */
    herror("gethostbyname");
    exit(1);
  }

  if (argc > 2) {
    port = atoi(argv[2]);
  } else port = PORT;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  their_addr.sin_family = AF_INET; /* host byte order */
  their_addr.sin_port = htons(port); /* short, network byte order */
  their_addr.sin_addr = * ((struct in_addr * ) he->h_addr);
  bzero( & (their_addr.sin_zero), 8); /* zero the rest of the struct */

  if (connect(sockfd, (struct sockaddr * ) & their_addr, \
      sizeof(struct sockaddr)) == -1) {
    perror("connect");
    exit(1);
  }

  // Display login page
  display_login(sockfd);
  play_game(sockfds);

  close(sockfd);
  return 0;

}
