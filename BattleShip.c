#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef struct Ship
{
    int hits;
    int size;
    int startrow;
    int startcolumn;
    char orientation;
} Ship;
typedef struct player
{
    char name[50];
    char grid[10][10];
    int radar;
    int shipsr;
    int smoke;
    int artillery;
    char smokegrid[10][10];
    Ship ships[4];
} player;
void Fire(player *opponent, int row, char column, int diff, player *self);
int RadarSweep(player *opponent, int row, int column, player *self);
void SmokeScreen(player *opponent, int row, char column, player *self);
void Artillery(int row, int col, char grid[10][10], player *opponent);
void Torpedo(player *opponent, char target, int diff);
void CreateGrid(char grid[10][10], char smokegrid[10][10])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            grid[i][j] = '~';
            smokegrid[i][j] = '~';
        }
    }
}
int compare_strings(const char *str1, const char *str2)
{
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
        {
            return 0;
        }
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0');
}
void handle_move(player *current_player, player *opponent, int diff)
{
    char command[10];
    char coor[3];
    printf("%s, Enter your move (e.g., Fire/Radar/Smoke/Artillery/Torpedo): ", current_player->name);
    scanf("%s %s", command, coor);
    char column = coor[0];
    int row = coor[1] - '1';
    if (compare_strings(command, "Fire"))
    {
        if ((column < 'A' || column > 'J') || (row < 0 || row > 9))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        Fire(opponent, row, column, diff, current_player);
    }
    else if (compare_strings(command, "Radar"))
    {
        if (current_player->radar <= 0)
        {
            printf("No radar sweeps remaining. Turn over.\n");
            return;
        }
        if ((column < 'A' || column > 'J') || (row < 0 || row - 1 > 9))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        RadarSweep(opponent, row, column, current_player);
    }
    else if (compare_strings(command, "Smoke"))
    {
        if (current_player->smoke <= 0)
        {
            printf("No smoke screens remaining. Turn over.\n");
            return;
        }
        if ((column < 'A' || column > 'J') || (row < 0 || row > 9))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        SmokeScreen(opponent, row, column, current_player);
    }
    else if (compare_strings(command, "Artillery"))
    {
        if ((column < 'A' || column > 'J') || (row < 0 || row > 9))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        Artillery(row, column - 'A', opponent->grid, opponent);
    }
    else if (compare_strings(command, "Torpedo"))
    {
        if ((coor[0] < 'A' || coor[0] > 'J') && (coor[1] < '1' || coor[1] > '9'))
        {
            printf("Invalid target. Please enter a valid move.\n");
            return;
        }
        if (coor[0] >= 'A' && coor[0] <= 'J')
        {
            column = coor[0] - 'A';
            Torpedo(opponent, coor[0], diff);
        }
        else if (coor[0] >= '1' && coor[0] <= '9' && coor[1] == '\0')
        {
            row = coor[0] - '1';
            Torpedo(opponent, coor[0], diff);
        }
        else if (coor[0] == '1' && coor[1] == '0')
        {
            row = 9;
            row = (char)row;
            Torpedo(opponent, row, diff);
        }
        else
        {
            printf("Invalid target. Please enter a valid row (1-10) or column (A-J).\n");
            return;
        }
        Torpedo(opponent, coor[0], diff);
    }
}
void displayGrid(char grid[10][10], int diff)
{
    char c = 'A';
    printf("   ");
    for (int i = 0; i < 10; i++)
    {
        printf("%2c ", c++);
    }
    printf("\n");
    for (int i = 0; i < 10; i++)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < 10; j++)
        {
            if (grid[i][j] == '*')
            {
                printf(" * ");
            }
            else if (diff == 0 && grid[i][j] == 'o')
            {
                printf(" o ");
            }
            else
            {
                printf(" ~ ");
            }
        }
        printf("\n");
    }
}

void Fire(player *opponent, int row, char column, int diff, player *self)
{
    int col = column - 'A';
    row -= 1;
    if (opponent->grid[row][col] == 'S' || opponent->grid[row][col] == '*')
    {
        opponent->grid[row][col] = '*';
        printf("Hit!\n");
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            if ((ship->orientation == 'H' && row == ship->startrow && col >= ship->startcolumn && col < ship->startcolumn + ship->size) ||
                (ship->orientation == 'V' && col == ship->startcolumn && row >= ship->startrow && row < ship->startrow + ship->size))
            {
                ship->hits++;
                if (ship->hits == ship->size)
                {
                    if (ship->size == 5)
                    {
                        printf("You sunk a Carrier!\n");
                    }
                    else if (ship->size == 4)
                    {
                        printf("You sunk a Battleship!\n");
                    }
                    else if (ship->size == 3)
                    {
                        printf("You sunk a Destroyer!\n");
                    }
                    else if (ship->size == 2)
                    {
                        printf("You sunk a Submarine!\n");
                    }
                    opponent->shipsr--;
                    self->smoke++;
                }
                break;
            }
        }
    }
    else
    {
        if (diff == 1)
        {
            printf("Miss!\n");
        }
        else
        {
            opponent->grid[row][col] = 'o';
            printf("Miss!\n");
        }
    }
}
int RadarSweep(player *opponent, int row, int column, player *self)
{
    if (self->radar > 0)
    {
        self->radar--;
        int col = column - 'A';
        row -= 1;
        for (int i = row; i < row + 2 && i < 10; i++)
        {
            for (int j = col; j < col + 2 && j < 10; j++)
            {
                if (opponent->smokegrid[i][j] == 'S')
                {
                    printf("Enemy Ship(s) found");
                    return 1;
                }
            }
        }
        printf("No Enemy Ship(s) found");
        return 0;
    }
    else
    {
        printf("no more radar sweeps remaining. turn over");
        return 0;
    }
}
void SmokeScreen(player *opponent, int row, char column, player *self)
{
    int shipssunk = 4 - opponent->shipsr;
    if (self->smoke > 0)
    {
        self->smoke--;
        int col = column - 'A';
        row -= 1;
        for (int i = row; i < row + 2 && i < 10; i++)
        {
            for (int j = col; j < col + 2 && j < 10; j++)
            {
                self->smokegrid[i][j] = '~';
            }
        }
        system("clear");
        printf("Smoke screen deployed.\n");
    }
    else
    {
        printf("No smoke screens remaining. You lose your turn.\n");
    }
}
void Artillery(int row, int col, char grid[10][10], player *opponent)
{
    int hits = 0;
    row = row - 1;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int targetRow = row + i;
            int targetCol = col + j;
            if (targetRow >= 0 && targetRow < 10 && targetCol >= 0 && targetCol < 10)
            {
                if (grid[targetRow][targetCol] == 'S' || grid[targetRow][targetCol] == '*')
                {
                    grid[targetRow][targetCol] = '*';
                    hits++;
                }
                else
                {
                    grid[targetRow][targetCol] = 'o';
                }
            }
        }
    }
    if (hits > 0)
    {
        printf("Artillery Hit %d times\n", hits);
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            int ship_hit_count = 0;
            if (ship->orientation == 'H' && row == ship->startrow)
            {
                for (int j = ship->startcolumn; j < ship->startcolumn + ship->size; j++)
                {
                    if (opponent->grid[ship->startrow][j] == '*')
                    {
                        ship_hit_count++;
                    }
                }
            }
            else if (ship->orientation == 'V' && col == ship->startcolumn)
            {
                for (int j = ship->startrow; j < ship->startrow + ship->size; j++)
                {
                    if (opponent->grid[j][ship->startcolumn] == '*')
                    {
                        ship_hit_count++;
                    }
                }
            }
            if (ship_hit_count == ship->size)
            {
                if (ship->size == 5)
                {
                    printf("You sunk a Carrier!\n");
                }
                else if (ship->size == 4)
                {
                    printf("You sunk a Battleship!\n");
                }
                else if (ship->size == 3)
                {
                    printf("You sunk a Destroyer!\n");
                }
                else if (ship->size == 2)
                {
                    printf("You sunk a Submarine!\n");
                }
                opponent->shipsr--;
            }
        }
    }
    else
    {
        printf("Miss!\n");
    }
}
void Torpedo(player *opponent, char target, int diff)
{
    int hitcount = 0;
    int col = -1, row = -1;
    if ((target >= 'A' || target <= 'J'))
    {
        col = target - 'A';
    }
    else
    {
        row = target - '0';
    }
    if (col != -1)
    {
        for (int k = 0; k < 10; k++)
        {
            if (opponent->grid[k][col] == 'S')
            {
                opponent->grid[k][col] = '*';
                hitcount++;
            }
            else if (diff == 0 && opponent->grid[k][col] == '~')
            {
                opponent->grid[k][col] = 'o';
            }
        }
    }
    else if (row != -1)
    {
        for (int k = 0; k < 10; k++)
        {
            if (opponent->grid[row][k] == 'S')
            {
                opponent->grid[row][k] = '*';
                hitcount++;
            }
            else if (diff == 0 && opponent->grid[row][k] == '~')
            {
                opponent->grid[row][k] = 'o';
            }
        }
    }
    if (hitcount > 0)
    {
        printf("Torpedo Hit %d time(s)\n", hitcount);
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            int ship_hit_count = 0;
            if (ship->orientation == 'H' && row == ship->startrow)
            {
                for (int j = ship->startcolumn; j < ship->startcolumn + ship->size; j++)
                {
                    if (opponent->grid[ship->startrow][j] == '*')
                    {
                        ship_hit_count++;
                    }
                }
            }
            else if (ship->orientation == 'V' && col == ship->startcolumn)
            {
                for (int j = ship->startrow; j < ship->startrow + ship->size; j++)
                {
                    if (opponent->grid[j][ship->startcolumn] == '*')
                    {
                        ship_hit_count++;
                    }
                }
            }
            if (ship_hit_count == ship->size)
            {
                if (ship->size == 5)
                {
                    printf("You sunk a Carrier!\n");
                }
                else if (ship->size == 4)
                {
                    printf("You sunk a Battleship!\n");
                }
                else if (ship->size == 3)
                {
                    printf("You sunk a Destroyer!\n");
                }
                else if (ship->size == 2)
                {
                    printf("You sunk a Submarine!\n");
                }
                opponent->shipsr--;
            }
        }
    }
    else
    {
        printf("Miss!\n");
    }
}
int isValidPlacement(player *player, char column, int row, char orientation, int size)
{
    int ind = column - 'A';
    row -= 1;
    if ((orientation == 'H' && ind + size > 10) || (orientation == 'V' && row + size > 10))
    {
        return 0;
    }
    if (orientation == 'H')
    {
        for (int i = 0; i < size; i++)
        {
            if (player->grid[row][ind + i] != '~')
            {
                return 0;
            }
        }
    }
    else if (orientation == 'V')
    {
        for (int i = 0; i < size; i++)
        {
            if (player->grid[row + i][ind] != '~')
            {
                return 0;
            }
        }
    }
    return 1;
}
void PlaceShip(player *player)
{
    char ships[4][20] = {{"Carrier"}, {"Battleship"}, {"Destroyer"}, {"Submarine"}};
    int size = 5;
    char column;
    int row;
    char orientation;
    int shipSizes[4];
    for (int i = 0; i < 4; i++)
    {
        size = size - i;
        shipSizes[i] = size;
        int isValid = 0;
        while (!isValid)
        {
            printf("Enter the coordinates for the %s and orientation (H/V): ", ships[i]);
            scanf(" %c %d %c", &column, &row, &orientation);
            row -= 1;
            while (getchar() != '\n')
                ;
            isValid = isValidPlacement(player, column, row + 1, orientation, size);
            if (!isValid)
            {
                printf("Invalid placement. Try again.\n");
            }
        }
        int ind = column - 'A';
        player->ships[i].size = shipSizes[i];
        player->ships[i].hits = 0;
        player->ships[i].startrow = row;
        player->ships[i].startcolumn = ind;
        player->ships[i].orientation = orientation;
        if (orientation == 'H')
        {
            for (int j = 0; j < size; j++)
            {
                player->grid[row][ind + j] = 'S';
                player->smokegrid[row][ind + j] = 'S';
            }
        }
        else if (orientation == 'V')
        {
            for (int j = 0; j < size; j++)
            {
                player->grid[row + j][ind] = 'S';
                player->smokegrid[row + j][ind] = 'S';
            }
        }
    }
}
int main()
{
    player *player1, *player2;
    player1 = malloc(sizeof(player));
    player2 = malloc(sizeof(player));
    CreateGrid(player1->grid, player1->smokegrid);
    CreateGrid(player2->grid, player2->smokegrid);
    player1->shipsr = 4;
    player2->shipsr = 4;
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
    srand(time(NULL));
    /* succussful attempt at randomly choosing without seed
        int last;
        int percent;
        int arr[1];
        int random = rand()%2;
        if(random ==0){
            int last = ((int)(&arr) % 10)%10;
        }
        else{
            int last = (int)(&arr) % 10;
        }
        if(last%2==0){percent=0;}else{percent=1;}
    */

    int percent = rand() % 2;
    if (percent == 0)
    {
        printf("%s, place your ships you're starting\n", player1->name);
        PlaceShip(player1);
        system("clear");
        printf("%s, place your ships \n", player2->name);
        PlaceShip(player2);
        system("clear");
    }
    else
    {
        printf("%s, place your ships you're starting\n", player2->name);
        PlaceShip(player2);
        system("clear");
        printf("%s, place your ships \n", player1->name);
        PlaceShip(player1);
        system("clear");
    }
    int game_over = 0;
    while (!game_over)
    {
        printf("\n %s's current grid:\n", player2->name);
        displayGrid(player2->grid, diff);
        handle_move(player1, player2, diff);
        if (player2->shipsr == 0)
        {
            printf("%s wins!\n", player1->name);
            game_over = 1;
            break;
        }
        printf("\n %s's current grid:\n", player1->name);
        displayGrid(player1->grid, diff);
        handle_move(player2, player1, diff);
        if (player1->shipsr == 0)
        {
            printf("%s wins!\n", player2->name);
            game_over = 1;
            break;
        }
    }
    free(player1);
    free(player2);
    return 0;
}