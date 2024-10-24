#include <stdio.h>
#include <stdlib.h>
typedef struct player {
    char name[50];
    char grid[10][10];
} player;
void CreateGrid(char grid[10][10]) {
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
int isValidPlacement(player *player, char column, int row, char orientation, int size) {
    int ind = column - 'A';  
    if (orientation == 'H') {
        if (ind + size > 10) return 0;  
        for (int i = 0; i < size; i++) {
            if (player->grid[row - 1][ind + i] != '~') return 0;  
        }
    } else if (orientation == 'V') {
        if (row - 1 + size > 10) return 0;  
        for (int i = 0; i < size; i++) {
            if (player->grid[row - 1 + i][ind] != '~') return 0; 
        }
    }
    return 1;  
}
void PlaceShip(player* player) {
    char ships[4][20]={{"Carrier"}, {"Battleship"}, {"Destroyer"}, {"Subarine"}};
    int size=5;
    char column;
    int row;
    char orientation[20];
    for(int i=0; i<4; i++){
        size=size-i;
        int isValid=0;
        while(!isValid){
            printf("Enter the coordinates for the %s and orientation(Horizental/Vertical): \n", ships[i]);
            scanf("%c%d%s",&column, &row, &orientation);
            isValid=isValidPlacement(player, column, row, orientation[0], size);
        }
        int j=0;
        while(j<10){
            if(j==row)
            break;
            j++;
        }
        int k=0;
        while(k<10){
            if(k==column-'A')
            break;
            k++;
        }
        if(orientation[0]=='H'){
            while(k<size){
                player->grid[j][k]='S';
                k++;
            }
        }else{
            while(j<size){
                player->grid[j][k]='S';
                j++;
            }
        }
    }
}
int main() {
    player player1, player2;
    CreateGrid(player1.grid);
    CreateGrid(player2.grid);
    printf("Difficulty: Easy(0) / Hard(1): ");
    int diff;
    scanf("%d", &diff);
    printf("Enter first player's name: ");
    scanf("%s", player1.name);  
    printf("Enter second player's name: ");
    scanf("%s", player2.name);
    int percent = rand() % 2;  
    if (percent == 0) {
        printf("%s, place your ships\n", player1.name);
        PlaceShip(&player1);
    } else {
        printf("%s, place your ships\n", player2.name);
        PlaceShip(&player2);
    }

    return 0;
}
