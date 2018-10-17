//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

char server_board[NUM_TILES_X][NUM_TILES_Y], client_board[NUM_TILES_X][NUM_TILES_Y];
char *revealed = "x", *mine = "*", *space = " ";;
char *flag = "+";
int mine_positions[NUM_MINES][2];
int x_input, y_input, menu_option;
int mines_remaining = NUM_MINES;
//int moves_left = NUM_TILES_X * NUM_TILES_Y - mines_remaining;

char option_input;
  bool valid_option, game_running;

bool valid_tile(int x, int y){
	if ((x >= 0 && x < NUM_TILES_X) && (y >= 0 && y < NUM_TILES_Y)){
		return true;
	} else {
		return false;
	}
}

bool tile_contains_mine(int x, int y, char board[][NUM_TILES_Y]){
  if(board[x][y]== *mine){ //change to == space
  return true;
} else {
  return false;
}
}

void initialise_board(){

	int i = 0, j = 0;
	for (i=0; i< NUM_TILES_X; i++){
	    for (j=0; j< NUM_TILES_Y ; j++){
	    	client_board[j][i] = server_board[j][i]= *space;
	    }
	}
}

void drawboard(char client_board[][NUM_TILES_Y]){
  int i, j;
  printf("%s","  012345678\n 0");
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){
      printf("%c",client_board[j][i]);
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

    } while (tile_contains_mine(x,y,server_board));
    // place mine and (x,y)
    server_board[x][y] = *mine;
  }
}

void display_menu(){
  printf("%s","Sup cunt the socket connection shit will go here. For now just press any key\n");
  getchar();
}

int check_tile(int x, int y,char server_board[][NUM_TILES_Y]){
	int surrounding_tiles = 0;
	if (tile_contains_mine(x,y,server_board)){
		surrounding_tiles = -1;
	} else {
    if (valid_tile(x-1,y)){
      if(tile_contains_mine(x-1,y,server_board)){
  			surrounding_tiles++;
  		}
    }
    if (valid_tile(x+1,y)){
      if(tile_contains_mine(x+1,y,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x,y+1)){
      if(tile_contains_mine(x,y+1,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x,y-1)){
      if(tile_contains_mine(x,y-1,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x-1,y+1)){
      if(tile_contains_mine(x-1,y+1,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x-1,y-1)){
      if(tile_contains_mine(x-1,y-1,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x+1,y+1)){
      if(tile_contains_mine(x+1,y+1,server_board)){
  		     surrounding_tiles++;
  		}
    }
    if (valid_tile(x+1,y-1)){
      if(tile_contains_mine(x+1,y-1,server_board)){
  		     surrounding_tiles++;
  		}
    }
	}
	return surrounding_tiles;
}

bool check_surrounding_tiles(char board[][NUM_TILES_Y],int x, int y){
  if (board[x][y] != *space){
    return false;
  }

  int i,j;

  //Mine opened
  if (server_board[x][y] == *mine){
    client_board[x][y] == *mine;

    for (i=0; i<NUM_MINES; i++);
      client_board[mines[i][0]][mines[i][1]]=*mine;
    drawboard(client_board);
    printf("/nYou have revealed a mine! Game over :(\n\n");
    return true;
  } else {
    int tile_value = check_tile(x,y, server_board);

    client_board[x][y] = tile_value + 0;

    if (!tile_value){
      if (valid_tile(x-1,y)){
        if(tile_contains_mine(x-1,y,server_board)){
    			check_surrounding_tiles(x-1,y);
    		}
      }
      if (valid_tile(x+1,y)){
        if(tile_contains_mine(x+1,y,server_board)){
    		     check_surrounding_tiles(x+1,y);
    		}
      }
      if (valid_tile(x,y+1)){
        if(tile_contains_mine(x,y+1,server_board)){
    		     check_surrounding_tiles(x,y+1);
    		}
      }
      if (valid_tile(x,y-1)){
        if(tile_contains_mine(x,y-1,server_board)){
    		     check_surrounding_tiles(x,y-1);
    		}
      }
      if (valid_tile(x-1,y+1)){
        if(tile_contains_mine(x-1,y+1,server_board)){
    		     check_surrounding_tiles(x-1,y+1);
    		}
      }
      if (valid_tile(x-1,y-1)){
        if(tile_contains_mine(x-1,y-1,server_board)){
    		     check_surrounding_tiles(x-1,y-1);
    		}
      }
      if (valid_tile(x+1,y+1)){
        if(tile_contains_mine(x+1,y+1,server_board)){
    		     check_surrounding_tiles(x+1,y+1);
    		}
      }
      if (valid_tile(x+1,y-1)){
        if(tile_contains_mine(x+1,y-1,server_board)){
    		     check_surrounding_tiles(x+1,y-1);
    		}
      }
      return false;
    }
  }
}

void play_game(){
  bool game_over = false;
  initialise_board();
  place_mines();
  drawboard(client_board);
	!valid_option;
	while(!valid_option){
			printf("%s", "\nChoose an option:\n<R> Reveal tile\n<F> Place flag\n<Q> Quit\n\n Option(R,F,Q):\n\n");
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
				 /*int tile_no = check_tile(x_input, y_input);
				 if (tile_no == -1){
					 client_board[x_input][y_input] = *revealed; //could change to client = server
					 drawboard();
					 printf("You have revealed a mine! Game over :(\n\n");
					 game_running = false;
					 getchar();
					 display_welcome();
				 }
         reveal_tile(x_input, y_input);*/
				 game_over = check_surrounding_tiles(client_board, server_board, mine, x_input, y_input);
         //drawboard();
			 } else {
				 client_board[x_input][y_input] = *flag;
         drawboard(client_board);
			 }
			 //drawboard();
			 valid_option = false; //return to options
		} else if(option_input == 'q'){
			 game_running = false;
			 display_welcome();
		}
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n\nSelect from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose option:");
  scanf("%d", &menu_option);
	if (menu_option == 1){
	  play_game();
	} else if (menu_option == 2){
		printf("We don't have a leaderboard yet soz\n");

	} else if (menu_option ==3){
		exit(1);
	} else {
		printf("Enter a valid option. Select from the following:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\nChoose an option:");
	}
}

int main(int argc, char *argv[]) {
	//initialise_board();
	srand(RANDOM_NUMBER_SEED);
	display_menu();
	display_welcome();
  return 0;
}
