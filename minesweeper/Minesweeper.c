//assignment
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RANDOM_NUMBER_SEED 42
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10

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
  /*for (i=0;i<10;i++){
    if board[i][j] == "x"
  }*/
  return false;
}

char board[10][10];

void drawboard(){
  int i, j;
  //char *p = ".";
  for (i=0; i< 10; i++){
    for (j=0; j< 10; j++){
    //board[i][j]= *p;
      printf("%c ",board[i][j]);
    }
    printf("\n");
  }
}

void place_mines(){
  for (int i=0; i < NUM_MINES; i++){
    char *mine = "o";
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
  place_mines();
    drawboard();

    //Tile.revealed = false;


  srand(RANDOM_NUMBER_SEED);
  return 0;
}
