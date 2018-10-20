//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

char server_board[NUM_TILES_X][NUM_TILES_Y];
char client_board[NUM_TILES_X][NUM_TILES_Y];
char *mine = "*";
char *revealed = "x";
char *flag = "+";
int mine_positions[NUM_MINES][2], remaining_mines =10;
int x_input, y_input, menu_option;

char option_input;
bool valid_option, game_running;

typedef struct{
  int adjacent_mines;
  bool revealed;
  bool is_mine;
} Tile;

typedef struct{
  Tile tiles[NUM_TILES_X][NUM_TILES_Y];
  bool running;
} GameState;

bool tile_contains_mine(int x, int y){
  if(server_board[x][y]== *mine){ //change to == space
  return true;
} else {
  return false;
}
}

void initialise_board(){
	char *space = "-";
	int i = 0, j = 0;
	for (i=0; i< NUM_TILES_X; i++){
	    for (j=0; j< NUM_TILES_Y ; j++){
	    	client_board[j][i]= *space;
        server_board[j][i]= *space;
	    }
	}

}

void drawboard(){
  int i, j;
  char *p = ".";
  printf("%s","   012345678\n   =========\n 0|");
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){

      printf("%c",client_board[j][i]);
    }
    if(i<NUM_TILES_Y -1){
    printf("\n %d|", i+1);
  }
  }
  printf("\n");
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
  }
}

void display_menu(){
  printf("%s","The login stuff will go here. For now just press any key\n");
  getchar();
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose your option:");
  scanf("%d", &menu_option);
	if (menu_option == 1){
    initialise_board();
		place_mines();
	  game_running = true;
	} else if (menu_option == 2){
		printf("We don't have a leaderboard yet soz\n\n");
    display_welcome();
	} else if (menu_option ==3){
		exit(1);
	} else {
		printf("Enter a valid option. Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose:");
	}
}

int check_tile(int x, int y){
	int surrounding_tiles = 0;
	if (tile_contains_mine(x,y)){
		surrounding_tiles = -1;
	} else {
		if(tile_contains_mine(x+1,y)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y)) {
			surrounding_tiles++;
		} if(tile_contains_mine(x,y-1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x+1,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y+1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x+1,y-1)){
			surrounding_tiles++;
		} if(tile_contains_mine(x-1,y-1)){
			surrounding_tiles++;
		}
	}
	return surrounding_tiles;
}

void reveal_tile(int x ,int y){
  int tile_no = check_tile(x, y);
  char tile_no_c = '0' + tile_no;
  client_board[x][y] = tile_no_c;
}

void open_safe_tiles(int x, int y){
  //initialise linked list for 0 tiles
  int tile_value = check_tile(x,y);
  if (tile_value == 0){
  int adjacent_zeros = 0;
    reveal_tile(x-1,y);
    if (check_tile(x-1,y) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x+1,y);
    if (check_tile(x+1,y) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x,y-1);
    if (check_tile(x,y-1) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x,y+1);
    if (check_tile(x,y+1) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x-1,y+1);
    if (check_tile(x-1,y+1) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x-1,y-1);
    if (check_tile(x-1,y-1) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x+1,y+1);
    if (check_tile(x+1,y+1) == 0){
      adjacent_zeros++;
    }
    reveal_tile(x+1,y-1);
    if (check_tile(x+1,y-1) == 0){
      adjacent_zeros++;
    }
  }
}

void reveal_mines(){
  int x, y;
  for (int i=0; i< NUM_MINES; i++){
    client_board[mine_positions[i][0]][mine_positions[i][1]] = *mine;
  }
}

void play_game(){
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
			 //drawboard();
			 valid_option = false; //return to options
		} else if(option_input == 'q'){
			 game_running = false;
			 display_welcome();
		}
}

int main(int argc, char *argv[]) {
	srand(RANDOM_NUMBER_SEED);
	display_menu();
	display_welcome();
	while(game_running){
		play_game();
	}

  return 0;
}
