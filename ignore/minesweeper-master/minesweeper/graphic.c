#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  
int main() {
    char board[10][3][100] = {
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" },
	{"x", "x", "x" }

	};
    int i=0;
    int j=0;



    for (i=0;i<10;i++){
        for (j=0;j<3;j++) {    
            printf("%s ",board[i][j]);
        }
        printf("\n");
    }
    printf("Done");
    scanf("%d",&i); //why this scanf here ??
    return 0;
}
