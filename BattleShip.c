#include <stdio.h>
#include <stdlib.h>
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
    int artilary;
    char smokegrid[10][10];
    Ship ships[4];
} player;
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
    char column;
    int row;
    printf("%s, enter your move (e.g., Fire B3): ", current_player->name);
    scanf("%s %c%d", command, &column, &row);
    if (compare_strings(command, "Fire"))
    {
        if (column < 'A' || column > 'J' || row < 1 || row > 10)
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        Fire(opponent, row, column, diff);
    }
    else if (compare_strings(command, "Radar"))
    {
        if (current_player->radar <= 0)
        {
            printf("No radar sweeps remaining. Turn over.\n");
            return;
        }
        if (column < 'A' || column > 'J' || row < 1 || row > 10)
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
        if (column < 'A' || column > 'J' || row < 1 || row > 10)
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        SmokeScreen(opponent, row, column, current_player);
    }
    else if (compare_strings(command, "Artillery"))
    {
        if (column < 'A' || column > 'J' || row < 1 || row > 10)
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        Artillary(opponent, row, column, diff);
    }
    else
    {
        printf("Invalid command. Please use Fire, Radar, Smoke, or Artillery.\n");
    }
}
void displayGrid(char grid[10][10], int diff)
{
    char c = 'A';
    printf("   ");
    for (int i = 0; i < 10; i++)
        printf("%c ", c++);
    printf("\n");
    for (int i = 0; i < 10; i++)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < 10; j++)
        {
            if (diff == 1)
            {
                if (grid[i][j] == '~')
                {
                    printf(" ~ ");
                }
                else if (grid[i][j] == '*')
                {
                    printf(" * ");
                }
                else if (grid[i][j] == 'o')
                {
                    printf(" ~ ");
                }
            }
            else
            {
                if (grid[i][j] == '~' || grid[i][j] == 'o')
                {
                    printf(" ~ ");
                }
                else if (grid[i][j] == '*')
                {
                    printf(" ~ ");
                }
            }
        }
        printf("\n");
    }
}
void Fire(player *opponent, int row, char column, int diff)
{
    int col = column - 'A';
    row -= 1;
    if (opponent->grid[row][col] == 'S')
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
                    printf("You sunk a ship!\n");
                    opponent->shipsr--;
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
void SmokeScreen(player *opponent, int row, int column, player *self)
{
    int shipssunk = 4 - opponent->shipsr;
    if (shipssunk > self->smoke)
    {
        self->smoke++;
        int col = column - 'A';
        row -= 1;
        for (int i = row; i < row + 2 && i < 10; i++)
        {
            for (int j = col; j < col + 2 && j < 10; j++)
            {
                self->smokegrid[i][j] = '~';
            }
        }
    }
    printf("smoke screen deployed");
}
void Artillary(player *opponent, int row, char column, int diff)
{
    int col = column - 'A';
    row -= 1;
    int hitcount = 0;
    for (int k = row; k < row + 2 && k < 10; k++)
    {
        for (int j = col; j < col + 2 && j < 10; j++)
        {
            if (opponent->grid[k][j] == 'S')
            {
                opponent->grid[k][j] = '*';
                hitcount++;
            }
        }
    }
    if (hitcount > 0)
    {
        printf("Artillary Hit %d times", hitcount);
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            if ((ship->orientation == 'H' && row == ship->startrow && col >= ship->startcolumn && col < ship->startcolumn + ship->size) ||
                (ship->orientation == 'V' && col == ship->startcolumn && row >= ship->startrow && row < ship->startrow + ship->size))
            {
                ship->hits++;
                if (ship->hits == ship->size)
                {
                    printf("You sunk a ship!\n");
                    opponent->shipsr--;
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
void Torpedo(player *opponent, char target, int diff)
{
    int hitcount = 0;
    int col = -1, row = -1;
    if (target >= 'A' && target <= 'J')
    {
        col = target - 'A';
    }
    else if (target >= '1' && target <= '9')
    {
        row = target - '1';
    }
    else
    {
        printf("Invalid target. Please enter a valid row or column.\n");
        return;
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
    else
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
        printf("Torpedo Hit %d times\n", hitcount);
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
                printf("You sunk a %s!\n", (ship->size == 5) ? "Carrier" : (ship->size == 4) ? "Battleship"
                                                                       : (ship->size == 3)   ? "Destroyer"
                                                                                             : "Submarine");
                opponent->shipsr--;
            }
        }
    }
    else
    {
        printf("Miss!\n");
    }
}

/*int isValidPlacement(player *player, char column, int row, char orientation, int size)
{
    int ind = column - 'A';
    if (orientation == 'H')
    {
        if (ind + size > 10)
            return 0;
        for (int i = 0; i < size; i++)
        {
            if (player->grid[row - 1][ind + i] != '~')
                return 0;
        }
    }
    else if (orientation == 'V')
    {
        if (row - 1 + size > 10)
            return 0;
        for (int i = 0; i < size; i++)
        {
            if (player->grid[row - 1 + i][ind] != '~')
                return 0;
        }
    }
    return 1;
}
*/
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
    char ships[4][20] = {{"Carrier"}, {"Battleship"}, {"Destroyer"}, {"Subarine"}};
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
            printf("Enter the coordinates for the %s and orientation(Horizental/Vertical): \n", ships[i]);
            scanf(" %c %d %c", &column, &row, &orientation);
            while (getchar() != '\n')
                ;
            isValid = isValidPlacement(player, column, row, orientation, size);
            if (!isValid)
            {
                printf("Invalid placement. Try again.\n");
            }
        }
        int ind = column - 'A';
        player->ships[i].size = shipSizes[i];
        player->ships[i].hits = 0;
        player->ships[i].startrow = row - 1;
        player->ships[i].startcolumn = column - 'A';
        player->ships[i].orientation = orientation;
        int j = 0;
        while (j < 10)
        {
            if (j == row)
                break;
            j++;
        }
        int k = 0;
        while (k < 10)
        {
            if (k == column - 'A')
                break;
            k++;
        }

        if (orientation == 'H')
        {
            for (int k = 0; k < size; k++)
            {
                player->grid[row - 1][ind + k] = 'S';
            }
        }
        else if (orientation == 'V')
        {
            for (int k = 0; k < size; k++)
            {
                player->grid[row - 1 + k][ind] = 'S';
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
    printf("%s, place your ships.\n", player1->name);
    PlaceShip(player1);
    system("cls");
    printf("%s, place your ships.\n", player2->name);
    PlaceShip(player2);
    system("cls");
    while (player1->shipsr > 0 && player2->shipsr > 0) {
        printf("\nCurrent Grid for %s:\n", player1->name);
        displayGrid(player1->grid, diff); 
        handle_move(player1, player2, diff); 
        if (player2->shipsr == 0) {
            printf("%s wins!\n", player1->name);
            break;
        }
        printf("\nCurrent Grid for %s:\n", player2->name);
        displayGrid(player2->grid, diff);
        handle_move(player2, player1, diff); 
        if (player1->shipsr == 0) {
            printf("%s wins!\n", player2->name);
            break;
        }
    }
    free(player1);
    free(player2);
    return 0;
}
