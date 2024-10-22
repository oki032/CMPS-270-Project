#include <stdio.h>
#include <stdlib.h>
typedef struct player {
    char name[50];
} player;
typedef struct Ships {
    char *name;
    int size;
} Ships;
void CreateGrid(void) {
    char grid[10][10]; 
    char c = 'A';
    printf("   ");
    for (int i = 0; i < 10; i++)
        printf("%c ", c++);
    printf("\n");
    for (int i = 0; i < 10; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < 10; j++) {
            grid[i][j] = '~'; 
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}
int main() {
    CreateGrid();
    printf("Difficulty: Easy(0) / Hard(1): ");
    int diff;
    scanf("%d", &diff);
    player player1, player2;
    printf("Enter first player's name: ");
    scanf("%s", player1.name);  
    printf("Enter second player's name: ");
    scanf("%s", player2.name);
    int percent = rand() % 2;  
    if (percent == 0) {
        printf("%s, place your ships\n", player1.name);
    } else {
        printf("%s, place your ships\n", player2.name);
    }
    return 0;
}
