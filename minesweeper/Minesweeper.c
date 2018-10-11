//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

char board[NUM_TILES_X][NUM_TILES_Y], *mine = "*";
char *revealed = "x";
	char *flag = "+";
int mine_positions[NUM_MINES][2];
int x_input, y_input;

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

  GameState *game;

bool tile_contains_mine(int x, int y){
  if(board[x][y]!= *mine){
  return false;
} else {
  return true;
}
}

void initialise_board(){
	char *space = " ";
	int i = 0, j = 0;
	for (i=0; i< NUM_TILES_X; i++){
	    for (j=0; j< NUM_TILES_Y ; j++){
	    	board[i][j]= *space;
	    }
	}
	
}

void drawboard(){
  //game->running = true;
  int i, j;
  char *p = ".";
  printf("%s","  012345678\n 0");
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){
      /*if(board[i][j]!= *mine){
        board[i][j]= *p;
      }*/
      printf("%c",board[i][j]);
    }
    if(i<NUM_TILES_Y -1){
    printf("\n %d", i+1);
  }
  }
  printf("\n");
}



void display_menu(){
  //game->running= false;
  printf("%s","Sup cunt the socket connection shit will go here. For now just press any key\n\n");
  getchar();
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n Press anykey for a board with mines\n");
  
  getchar();
  game_running = true;
}

void place_mines(){
  for (int i=0; i < NUM_MINES; i++){

    int x,y;
    do {
      x = rand() % NUM_TILES_X;
      y = rand() % NUM_TILES_Y;

    } while (tile_contains_mine(x,y));
    // place mine and (x,y)
    board[x][y] = *mine;
  }
}

void play_game(){
	valid_option = false;
	
	while (!valid_option){
		
		drawboard();
			printf("%s", "\nChoose an option:\n<R> Reveal tile\n<F> Place flag\n <Q>Quit\n\n Option(R,F,Q):\n\n");
			option_input = getchar();
			if(option_input == 'r' || option_input == 'f' ||option_input == 'q'){
			  valid_option = true;
		    }
		}
		
		if (option_input == 'r' || option_input == 'f'){
			 printf("\nEnter x coordinate:");
			 scanf("%d", &x_input);
			 //getchar();
			 printf("\nEnter y coordinate:");
			 scanf("%d", &y_input);
			 if(option_input == 'r'){
				 board[x_input][y_input] = *revealed;
			 } else {
				 board[x_input][y_input] = *flag;
			 }
			 drawboard();
			 valid_option = false; //return to options
		} else if(option_input == 'q'){	
			 game_running = false;
			 display_welcome();
		}
}

int main(int argc, char *argv[]) {
	initialise_board();
	srand(RANDOM_NUMBER_SEED);
	display_menu();
	display_welcome();
	place_mines();
	//drawboard();
	while(game_running){
		play_game();	
	}
	
	
  return 0;
}
