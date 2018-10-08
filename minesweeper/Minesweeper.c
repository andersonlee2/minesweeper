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

typedef struct{
  int adjacent_mines;
  bool revealed;
  bool is_mine;
} Tile;

typedef struct{
  bool won;
  Tile tiles[NUM_TILES_X][NUM_TILES_Y];
} GameState;

bool tile_contains_mine(int x, int y){
  if(board[x][y]!= *mine){
  return false;
} else {
  return true;
}
}

void drawboard(){
  int i, j;
  char *p = " ";
  printf("%s","  ABCDEFGHI\n 0");
  for (i=0; i< NUM_TILES_X; i++){
    for (j=0; j< NUM_TILES_Y ; j++){
      if(board[i][j]!= *mine){
        board[i][j]= *p;
      }
      printf("%c",board[i][j]);
    }
    if(i<NUM_TILES_X){
    printf("\n %d", i+1);
  }
  }
  printf("\n");
}

void display_welcome(){
  printf("%s","Welcome to the online Minesweeper game!\n");
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
  srand(RANDOM_NUMBER_SEED);
  display_welcome();
  place_mines();
  drawboard();

    //Tile.revealed = false;



  return 0;
}
