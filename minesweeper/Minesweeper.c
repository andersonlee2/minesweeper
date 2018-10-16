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
int x_input, y_input, menu_option;

char option_input;
  bool valid_option, game_running, leaderboard_requested;

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

//SAFE//

void initialise_board(){
	char *space = " ";
	int i = 0, j = 0;
	for (i=0; i< NUM_TILES_X; i++){
	    for (j=0; j< NUM_TILES_Y ; j++){
	    	board[j][i]= *space;
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
      printf("%c",board[j][i]);
    }
    if(i<NUM_TILES_Y -1){
    printf("\n %d", i+1);
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
    board[x][y] = *mine;
  }
}

void display_login(){
  printf("%s","Sup cunt the socket connection shit will go here. For now just press any key\n");
  getchar();
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n");
  
}

void display_leaderboard(){
	printf("\nWe don't have a leaderboard yet soz.\n");
	leaderboard_requested = false;
}

int check_tile(int x, int y){
	int surrounding_tiles = 0;
	if (tile_contains_mine(x,y)){
		surrounding_tiles = -1;
	} else {
		if(tile_contains_mine(x+1,y)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x-1,y)) {
			surrounding_tiles++;
		} else if(tile_contains_mine(x,y-1)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x,y+1)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x+1,y+1)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x-1,y+1)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x+1,y-1)){
			surrounding_tiles++;
		} else if(tile_contains_mine(x-1,y-1)){
			surrounding_tiles++;
		}
	}
	return surrounding_tiles;
}

void play_game(){
	valid_option = false;
	while (!valid_option){
		drawboard();
		printf("Choose an option:\n");
		printf("<R> Reveal tile\n");
		printf("<F> Place flag\n");
		printf("<Q> Quit\n\n"); 			
		printf("Option(R,F,Q):\n\n");
		option_input = getchar();
		if(option_input == 'r' || option_input == 'f' ||option_input == 'q'){
		  valid_option = true;
	    	}
		}
		if (option_input == 'r' || option_input == 'f'){
			 printf("\nEnter x coordinate:");
			 scanf("%d", &x_input);
			 printf("\nEnter y coordinate:");
			 scanf("%d", &y_input);
			 if(option_input == 'r'){
				 int tile_no = check_tile(x_input, y_input);
				 if (tile_no == -1){
					 board[x_input][y_input] = *revealed;
					 drawboard();
					 printf("You have revealed a mine! Game over :(\n\n");
					 game_running = false;
					 getchar();
					 display_welcome();
				 }
				 char tile_no_c = '0' + tile_no;
					 board[x_input][y_input] = tile_no_c;
			 } else {
				 board[x_input][y_input] = *flag;
			 }
			 valid_option = false; //return to options
		} else if(option_input == 'q'){
			 game_running= true;
			 
		}
}

void print_menu(){
	//print menu options
	printf("\n<1> Play Minesweeper\n");
	printf("<2> Show Leaderboard\n");
	printf("<3> Quit\n");
	printf("Option (1,2,3): ");
};



void wait_for_key(){	
	printf("Enter any key to continue:");
	char enter = 0;
	scanf(" %c", &enter);
}



int main(int argc, char *argv[]) {
	
	srand(RANDOM_NUMBER_SEED);
	display_login();
	display_welcome();
	
	do {
		//display menu options
		print_menu();
		
		//get client input
		scanf("%d", &menu_option );

		//check if input is valid
		while ( menu_option > 3 || menu_option < 1 ){
			printf("You entered an invalid option. Option (1,2,3): \n");
			scanf("%d", &menu_option );
		} 
		
		if (menu_option == 1){
			// Play Game
			initialise_board();
			place_mines();
		  	
			play_game();
			
			wait_for_key();
			
			
		} else if (menu_option == 2) {
			// Show Leaderboard
			display_leaderboard();
			wait_for_key();

		} else {
			// Quit Main
			printf("\nAdios!\n");
			exit(1);
		}

	} while( 1 );	

  	return 0;
}
