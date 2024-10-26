#include <stdio.h>
#include <stdlib.h>
typedef struct player {
    char name[50];
    char grid[10][10];
    int radar;
    int ships;
    int smoke;
    int artilary;
    char smokegrid[10][10];
} player;
void CreateGrid(char grid[10][10], char smokegrid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = '~'; 
            smokegrid[i][j]='~';
        }
    }
}
void displayGrid(char grid[10][10], int diff){
    char c = 'A';
    printf("   ");
    for (int i = 0; i < 10; i++)
        printf("%c ", c++);
    printf("\n");
    for (int i = 0; i < 10; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < 10; j++) {
            if (diff == 1) {
                if (grid[i][j] == '~') {
                    printf(" ~ ");
                    }
                    else if (grid[i][j] == '*') {
                        printf(" * ");
                        }
                        else if (grid[i][j] == 'o') {
                            printf(" ~ ");
                            }
            }else{
                if (grid[i][j] == '~' || grid[i][j]=='o') {
                    printf(" ~ ");
                    }
                    else if (grid[i][j] == '*') {
                        printf(" ~ ");
                    }
            }
        }
        printf("\n");
    }
}
void Fire(player *opponent, int row, char column, int diff){
    int col= column-'A';
    row=row-1;
    if(opponent->grid[row][col]=='S'){
    opponent->grid[row][col]='*';
    printf("hit");
    }else{
        if(diff==1){
            print("miss");
        }else{
            opponent->grid[row][col]='o';
            printf("hit");
        }
    }
}
int RadarSweep(player *opponent, int row, int column, player *self){
    if(self->radar > 0){
    self->radar--;
    int col= column-'A';
    row-=1;
    for(int i=row; i<row+2; i++){
        for(int j=col; j<col+2; j++){
            if(opponent->smokegrid[i][j]=='S'){
                printf("Enemy Ship(s) found");
                return 1;
            }
        }
    }
    printf("No Enemy Ship(s) found");
    return 0;
    }else{
        printf("no more radar sweeps remaining. turn over");
        return 0;
    }
}
void SmokeScreen(player *opponent, int row, int column, player *self){
    int shipssunk= 4-opponent->ships;
    if(shipssunk>self->smoke){
        self->smoke++;
        int col= column-'A';
        row-=1;
        for(int i=row; i<row+2; i++){
        for(int j=col; j<col+2; j++){
                self->smokegrid[i][j]='~';
            }
        }
    }
    printf("smoke screen deployed");
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
    char orientation;
    for(int i=0; i<4; i++){
        size=size-i;
        int isValid=0;
        while(!isValid){
            printf("Enter the coordinates for the %s and orientation(Horizental/Vertical): \n", ships[i]);
            scanf(" %c %d %c",&column, &row, &orientation);
            while (getchar() != '\n');
            isValid=isValidPlacement(player, column, row, orientation, size);
             if (!isValid) {
                printf("Invalid placement. Try again.\n");
            }
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
        if(orientation=='H'){
            while(k<size){
                player->grid[j][k]='S';
                k++;
            }
        }else if(orientation=='V'){
            while(j<size){
                player->grid[j][k]='S';
                j++;
            }
        }
    }
}
int main() {
    player *player1, *player2;
    CreateGrid(player1->grid);
    CreateGrid(player2->grid);
    player1->ships = 4;
    player2->ships = 4;
    player1->radar = 3;
    player2->radar = 3;
    player1->smoke = 0;
    player2->smoke = 0;
    printf("Difficulty: Easy(0) / Hard(1): ");
    int diff;
    scanf("%d", &diff);
    printf("Enter first player's name: ");
    scanf("%s", player1->name);  
    printf("Enter second player's name: ");
    scanf("%s", player2->name);
    int percent = rand() % 2;  
    if (percent == 0) {
        printf("%s, place your ships you're starting\n", player1->name);
        PlaceShip(&player1);
    } else {
        printf("%s, place your ships you're starting\n", player2->name);
        PlaceShip(&player2);
    }
    return 0;
}
