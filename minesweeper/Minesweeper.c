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
int mine_positions[NUM_MINES][2];
int x_input[1], y_input[1];

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

void drawboard(){
  //game->running = true;
  int i, j;
  char *p = " ";
  printf("%s","  012345678\n 0");
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){
      if(board[i][j]!= *mine){
        board[i][j]= *p;
      }
      printf("%c",board[i][j]);
    }
    if(i<NUM_TILES_Y -1){
    printf("\n %d", i+1);
  }
  }
  printf("\n");
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n Press anykey for a board with mines\n");
  getchar();
}

void display_menu(){
  //game->running= false;
  printf("%s","Sup cunt the socket connection shit will go here. For now just press any key\n\n");
  getchar();
}

void display_game_instructions(){
  char option_input[1], x_input[1], y_input[1];
  printf("%s", "\nChoose an option (They aren't wokring properly yet):\n<R> Reveal tile\n<F> Place flag\n<Q>Quit\n\n Option(R,F,Q):");
  scanf("%c",option_input);
  if (option_input == "r"||"f"){
    printf("%s", "\nEnter x coordinate:");
    scanf("%c", x_input);
    getchar();
    printf("%s", "\nEnter y coordinate:");
    scanf("%c", y_input);
    getchar();
  } else if(option_input == "q"){
    //game->running = false;
    display_menu();
  } else {
    printf("%s", "Invalid entry\n\nChoose an option:\n<R> Reveal tile\n<F> Place flag\n<Q>Quit\n\n Option(R,F,Q):");
  }
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

int main(int argc, char *argv[]) {
  //game->running = false;
  srand(RANDOM_NUMBER_SEED);
  display_menu();
  display_welcome();
  place_mines();
  drawboard();
  display_game_instructions();



  return 0;
}
