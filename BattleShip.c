#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

typedef struct Ship
{
    int hits;
    int size;
    int startrow;
    int startcolumn;
    char orientation;
    int sunk; 
} Ship;


typedef struct player
{
    char name[50];
    char grid[10][10];
    int radar;
    int shipsr; 
    int smoke;
    int artillery;
    int torpedo;
    char smokegrid[10][10];
    Ship ships[4];
    char opponentGrid[10][10];
    int probabilityGrid[10][10];
    int artilleryUnlocked;
    int torpedoUnlocked;
    int artilleryUsed;
    int torpedoUsed;
    int inTargetMode;
    int lastHitRow;
    int lastHitCol;
} player;

void deploySmokeScreenOverShip(player *bot);
void Fire(player *opponent, int row, char column, int diff, player *self);
int RadarSweep(player *opponent, int row, char column, player *self);
void SmokeScreen(player *self, int row, char column);
void Artillery(int row, int col, player *opponent, player *self, int diff);
void Torpedo(player *opponent, char target, int diff, player *self);
void CreateGrid(char grid[10][10]);
int compare_strings(const char *str1, const char *str2);
void handle_move(player *current_player, player *opponent, int diff);
void displayGrid(char grid[10][10], int diff);
int isValidPlacement(player *player, char column, int row, char orientation, int size);
void UpdateProbabilityMap(player *bot);
void BotMakeMove(player *bot, player *human, int diff);
void PlaceShip(player *player, int isBot);
void InitializePlayer(player *p);
void selectHighestProbabilityCell(player *bot, int *targetRow, int *targetCol);
void checkForSunkShips(player *bot, player *human);
void selectHighestProbabilityArea(player *bot, int *targetRow, int *targetCol);
int decideRowOrColumn(player *bot, char *target);

int main()
{
    player *player1, *player2;
    player1 = malloc(sizeof(player));
    player2 = malloc(sizeof(player));

    InitializePlayer(player1);
    InitializePlayer(player2);

    int diff;
    printf("Difficulty Tracking Mode: Easy(0) / Hard(1): ");
    scanf("%d", &diff);

    printf("Enter Player 1's name: ");
    scanf("%s", player1->name);

    char botChoice;
    printf("Is Player 2 a bot? (Y/N): ");
    scanf(" %c", &botChoice);
    botChoice = toupper(botChoice);

    int isBot = (botChoice == 'Y') ? 1 : 0;

    if (isBot)
    {
        strcpy(player2->name, "Bot");
    }
    else
    {
        printf("Enter Player 2's name: ");
        scanf("%s", player2->name);
    }

    srand(time(NULL));
    int startingPlayer = rand() % 2; 
    /* successful attempt at randomly choosing without seed
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

    if (startingPlayer == 0)
    {
        printf("%s, place your ships.\n", player1->name);
        PlaceShip(player1, 0);
        system("clear");
        printf("%s, place your ships.\n", player2->name);
        PlaceShip(player2, isBot);
        system("clear");
    }
    else
    {
        printf("%s, place your ships.\n", player2->name);
        PlaceShip(player2, isBot);
        system("clear");
        printf("%s, place your ships.\n", player1->name);
        PlaceShip(player1, 0);
        system("clear");
    }

    int game_over = 0;
    while (!game_over)
    {
        if (startingPlayer == 0)
        {
            printf("\n%s's turn.\n", player1->name);
            printf("\n%s's grid:\n", player2->name);
            displayGrid(player2->grid, diff);
            handle_move(player1, player2, diff);

            if (player2->shipsr == 0)
            {
                printf("%s wins!\n", player1->name);
                game_over = 1;
                break;
            }

            printf("\n%s's turn.\n", player2->name);
            if (isBot)
            {
                BotMakeMove(player2, player1, diff);

                if (player1->shipsr == 0)
                {
                    printf("%s wins!\n", player2->name);
                    game_over = 1;
                    break;
                }
            }
            else
            {
                printf("\n%s's grid:\n", player1->name);
                displayGrid(player1->grid, diff);
                handle_move(player2, player1, diff);

                if (player1->shipsr == 0)
                {
                    printf("%s wins!\n", player2->name);
                    game_over = 1;
                    break;
                }
            }
        }
        else
        {
            printf("\n%s's turn.\n", player2->name);
            if (isBot)
            {
                BotMakeMove(player2, player1, diff);

                if (player1->shipsr == 0)
                {
                    printf("%s wins!\n", player2->name);
                    game_over = 1;
                    break;
                }
            }
            else
            {
                printf("\n%s's grid:\n", player1->name);
                displayGrid(player1->grid, diff);
                handle_move(player2, player1, diff);

                if (player1->shipsr == 0)
                {
                    printf("%s wins!\n", player2->name);
                    game_over = 1;
                    break;
                }
            }

            printf("\n%s's turn.\n", player1->name);
            printf("\n%s's grid:\n", player2->name);
            displayGrid(player2->grid, diff);
            handle_move(player1, player2, diff);

            if (player2->shipsr == 0)
            {
                printf("%s wins!\n", player1->name);
                game_over = 1;
                break;
            }
        }
    }

    free(player1);
    free(player2);

    return 0;
}


void InitializePlayer(player *p)
{
    CreateGrid(p->grid);
    CreateGrid(p->smokegrid);
    p->shipsr = 4;
    p->radar = 3;
    p->smoke = 0;
    p->artillery = 0;
    p->torpedo = 0;
    p->artilleryUnlocked = 0;
    p->torpedoUnlocked = 0;
    p->artilleryUsed = 0;
    p->torpedoUsed = 0;
    p->inTargetMode = 0;
    p->lastHitRow = -1;
    p->lastHitCol = -1;
    CreateGrid(p->opponentGrid);
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            p->probabilityGrid[i][j] = 0;

    for (int i = 0; i < 4; i++)
        p->ships[i].sunk = 0;
}

void CreateGrid(char grid[10][10])
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            grid[i][j] = '~';
        }
    }
}

int compare_strings(const char *str1, const char *str2)
{
    return strcmp(str1, str2) == 0;
}

void handle_move(player *current_player, player *opponent, int diff)
{
    char command[20];
    printf("%s, Enter your move (e.g., Fire B5 / Radar C3 / Smoke D4 / Artillery E6 / Torpedo A): ", current_player->name);
    scanf("%s", command);

    if (compare_strings(command, "Fire"))
    {
        char coord[3];
        scanf("%s", coord);
        char column = toupper(coord[0]);
        int row;
        if (coord[2] != '\0')
            row = (coord[1] - '0') * 10 + (coord[2] - '0');
        else
            row = coord[1] - '0';
        if ((column < 'A' || column > 'J') || (row < 1 || row > 10))
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
        char coord[3];
        scanf("%s", coord);
        char column = toupper(coord[0]);
        int row;
        if (coord[2] != '\0')
            row = (coord[1] - '0') * 10 + (coord[2] - '0');
        else
            row = coord[1] - '0';
        if ((column < 'A' || column > 'J') || (row < 1 || row > 10))
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
        char coord[3];
        scanf("%s", coord);
        char column = toupper(coord[0]);
        int row;
        if (coord[2] != '\0')
            row = (coord[1] - '0') * 10 + (coord[2] - '0');
        else
            row = coord[1] - '0';
        if ((column < 'A' || column > 'J') || (row < 1 || row > 10))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        SmokeScreen(current_player, row, column);
    }
    else if (compare_strings(command, "Artillery"))
    {
        if (!current_player->artilleryUnlocked || current_player->artilleryUsed)
        {
            printf("Artillery not available. Turn over.\n");
            return;
        }
        char coord[3];
        scanf("%s", coord);
        char column = toupper(coord[0]);
        int row;
        if (coord[2] != '\0')
            row = (coord[1] - '0') * 10 + (coord[2] - '0');
        else
            row = coord[1] - '0';
        if ((column < 'A' || column > 'J') || (row < 1 || row > 10))
        {
            printf("Invalid coordinates. Please enter a valid move.\n");
            return;
        }
        Artillery(row, column - 'A', opponent, current_player, diff);
        current_player->artillery--;
    }
    else if (compare_strings(command, "Torpedo"))
    {
        if (!current_player->torpedoUnlocked || current_player->torpedoUsed)
        {
            printf("Torpedo not available. Turn over.\n");
            return;
        }
        char target[3];
        scanf("%s", target);
        Torpedo(opponent, toupper(target[0]), diff, current_player);
        current_player->torpedo--;
    }
    else
    {
        printf("Invalid command. Turn over.\n");
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
            if (ship->sunk)
                continue;
            if ((ship->orientation == 'H' && row == ship->startrow && col >= ship->startcolumn && col < ship->startcolumn + ship->size) ||
                (ship->orientation == 'V' && col == ship->startcolumn && row >= ship->startrow && row < ship->startrow + ship->size))
            {
                ship->hits++;
                if (ship->hits == ship->size)
                {
                    ship->sunk = 1;
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
                    self->artilleryUnlocked = 1;
                    if (opponent->shipsr == 1)
                    {
                        self->torpedoUnlocked = 1; 
                    }
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


    if (strcmp(self->name, "Bot") == 0)
    {
        if (opponent->grid[row][col] == '*')
        {
            self->inTargetMode = 1;
            self->lastHitRow = row;
            self->lastHitCol = col;
        }
        else
        {
            self->inTargetMode = 0;
            self->lastHitRow = -1;
            self->lastHitCol = -1;
        }
    }
}

int RadarSweep(player *opponent, int row, char column, player *self)
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
                    printf("Enemy ships found in the area.\n");
                    return 1;
                }
            }
        }
        printf("No enemy ships found in the area.\n");
        return 0;
    }
    else
    {
        printf("No more radar sweeps remaining. Turn over.\n");
        return 0;
    }
}

void SmokeScreen(player *self, int row, char column)
{
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

void Artillery(int row, int col, player *opponent, player *self, int diff)
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
                if (opponent->grid[targetRow][targetCol] == 'S' || opponent->grid[targetRow][targetCol] == '*')
                {
                    opponent->grid[targetRow][targetCol] = '*';
                    hits++;
                }
                else if (diff == 0 && opponent->grid[targetRow][targetCol] == '~')
                {
                    opponent->grid[targetRow][targetCol] = 'o';
                }
            }
        }
    }
    if (hits > 0)
    {
        printf("Artillery hit %d time(s)\n", hits);
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            if (ship->sunk)
                continue;
            int ship_hit_count = 0;
            if (ship->orientation == 'H')
            {
                for (int j = 0; j < ship->size; j++)
                {
                    if (opponent->grid[ship->startrow][ship->startcolumn + j] == '*')
                        ship_hit_count++;
                }
            }
            else if (ship->orientation == 'V')
            {
                for (int j = 0; j < ship->size; j++)
                {
                    if (opponent->grid[ship->startrow + j][ship->startcolumn] == '*')
                        ship_hit_count++;
                }
            }
            if (ship_hit_count == ship->size)
            {
                ship->sunk = 1;
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
        }
    }
    else
    {
        printf("Miss!\n");
    }
}

void Torpedo(player *opponent, char target, int diff, player *self)
{
    int hitcount = 0;
    int col = -1, row = -1;
    if (target >= 'A' && target <= 'J')
    {
        col = target - 'A';
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
    else if (target >= '1' && target <= '9')
    {
        row = target - '1';
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
    else if (target == '0')
    {
        row = 9;
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
    else
    {
        printf("Invalid target. Turn over.\n");
        return;
    }

    if (hitcount > 0)
    {
        printf("Torpedo hit %d time(s)\n", hitcount);
        for (int i = 0; i < 4; i++)
        {
            Ship *ship = &opponent->ships[i];
            if (ship->sunk)
                continue;
            int ship_hit_count = 0;
            if (ship->orientation == 'H')
            {
                for (int j = 0; j < ship->size; j++)
                {
                    if (opponent->grid[ship->startrow][ship->startcolumn + j] == '*')
                        ship_hit_count++;
                }
            }
            else if (ship->orientation == 'V')
            {
                for (int j = 0; j < ship->size; j++)
                {
                    if (opponent->grid[ship->startrow + j][ship->startcolumn] == '*')
                        ship_hit_count++;
                }
            }
            if (ship_hit_count == ship->size)
            {
                ship->sunk = 1;
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
        }
    }
    else
    {
        printf("Miss!\n");
    }
}

int isValidPlacement(player *player, char column, int row, char orientation, int size)
{
    int colIndex = column - 'A';
    int rowIndex = row - 1;

    if (colIndex < 0 || colIndex >= 10 || rowIndex < 0 || rowIndex >= 10)
        return 0;

    if (orientation == 'H' && colIndex + size > 10)
        return 0;
    if (orientation == 'V' && rowIndex + size > 10)
        return 0;

    if (orientation == 'H')
    {
        for (int i = 0; i < size; i++)
        {
            if (player->grid[rowIndex][colIndex + i] != '~')
                return 0;
        }
    }
    else if (orientation == 'V')
    {
        for (int i = 0; i < size; i++)
        {
            if (player->grid[rowIndex + i][colIndex] != '~')
                return 0;
        }
    }
    else
    {
        return 0;
    }

    return 1; 
}

void UpdateProbabilityMap(player *bot)
{
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            bot->probabilityGrid[i][j] = 0;

    int shipSizes[4];
    int numShips = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!bot->ships[i].sunk)
            shipSizes[numShips++] = bot->ships[i].size;
    }

    for (int s = 0; s < numShips; s++)
    {
        int shipSize = shipSizes[s];

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j <= 10 - shipSize; j++)
            {
                int canPlace = 1;
                int prob = 1; 

                for (int k = 0; k < shipSize; k++)
                {
                    char cell = bot->opponentGrid[i][j + k];
                    if (cell == 'o') 
                    {
                        canPlace = 0;
                        break;
                    }
                    else if (cell == '*')
                    {
                        prob += 10;
                    }
                }
                if (canPlace)
                {
                    for (int k = 0; k < shipSize; k++)
                    {
                        bot->probabilityGrid[i][j + k] += prob;
                    }
                }
            }
        }

        for (int i = 0; i <= 10 - shipSize; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                int canPlace = 1;
                int prob = 1;

                for (int k = 0; k < shipSize; k++)
                {
                    char cell = bot->opponentGrid[i + k][j];
                    if (cell == 'o')
                    {
                        canPlace = 0;
                        break;
                    }
                    else if (cell == '*')
                    {
                        prob += 10; 
                    }
                }
                if (canPlace)
                {
                    for (int k = 0; k < shipSize; k++)
                    {
                        bot->probabilityGrid[i + k][j] += prob;
                    }
                }
            }
        }
    }
}

void BotMakeMove(player *bot, player *human, int diff)
{
    UpdateProbabilityMap(bot);

    if (bot->artilleryUnlocked && !bot->artilleryUsed)
    {
        printf("Bot uses Artillery.\n");
        int targetRow, targetCol;
        selectHighestProbabilityCell(bot, &targetRow, &targetCol);
        Artillery(targetRow + 1, targetCol, human, bot, diff);
        bot->artilleryUsed = 1;
    }
    else if (bot->torpedoUnlocked && !bot->torpedoUsed)
    {
        printf("Bot uses Torpedo.\n");
        char target;
        decideRowOrColumn(bot, &target);
        Torpedo(human, target, diff, bot);
        bot->torpedoUsed = 1;
    }
    else if (bot->inTargetMode)
    {
        int targetRow = bot->lastHitRow;
        int targetCol = bot->lastHitCol;
        int found = 0;

        int directions[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
        for (int d = 0; d < 4; d++)
        {
            int newRow = targetRow + directions[d][0];
            int newCol = targetCol + directions[d][1];
            if (newRow >= 0 && newRow < 10 && newCol >= 0 && newCol < 10)
            {
                if (bot->opponentGrid[newRow][newCol] == '~')
                {
                    printf("Bot fires at %c%d\n", 'A' + newCol, newRow + 1);
                    Fire(human, newRow + 1, 'A' + newCol, diff, bot);
                    found = 1;
                    break;
                }
            }
        }
        if (!found)
        {
            bot->inTargetMode = 0;
            BotMakeMove(bot, human, diff); 
        }
    }
    else if (bot->radar > 0)
    {
        printf("Bot uses Radar Sweep.\n");
        int targetRow, targetCol;
        selectHighestProbabilityArea(bot, &targetRow, &targetCol);
        RadarSweep(human, targetRow + 1, 'A' + targetCol, bot);
    }
    else if (bot->smoke > 0)
    {
        printf("Bot deploys Smoke Screen.\n");
        deploySmokeScreenOverShip(bot);}
    else
    {
        int targetRow, targetCol;
        selectHighestProbabilityCell(bot, &targetRow, &targetCol);
        char actualcol = 'A' + targetCol;
        int actualrow = targetRow + 1;

        printf("Bot fires at %c%d\n", actualcol, actualrow);

        Fire(human, actualrow, actualcol, diff, bot);
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (human->grid[i][j] == '*')
                bot->opponentGrid[i][j] = '*';
            else if (human->grid[i][j] == 'o')
                bot->opponentGrid[i][j] = 'o';
        }
    }

    checkForSunkShips(bot, human);
}

void deploySmokeScreenOverShip(player *bot)
{

    int unsunkShipsIndices[4];
    int unsunkShipsCount = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!bot->ships[i].sunk)
        {
            unsunkShipsIndices[unsunkShipsCount++] = i;
        }
    }

    int shipIndex = unsunkShipsIndices[rand() % unsunkShipsCount];
    Ship *ship = &bot->ships[shipIndex];

    int possiblePositions[10][2];
    int positionsCount = 0;

    int shipRow = ship->startrow;
    int shipCol = ship->startcolumn;
    int shipSize = ship->size;
    char orientation = ship->orientation;

    if (orientation == 'H')
    {
        for (int col = shipCol; col < shipCol + shipSize; col++)
        {
            int startRow = shipRow + 1;
            int startCol = col - 1;

            if (startRow < 1)
                startRow = 1;
            if (startRow > 9)
                startRow = 9;
            if (startCol < 0)
                startCol = 0;
            if (startCol > 8)
                startCol = 8;

            possiblePositions[positionsCount][0] = startRow;
            possiblePositions[positionsCount][1] = startCol;
            positionsCount++;
        }
    }
    else if (orientation == 'V')
    {
        for (int row = shipRow; row < shipRow + shipSize; row++)
        {
            int startRow = row;
            int startCol = shipCol - 1;

            if (startRow < 1)
                startRow = 1;
            if (startRow > 9)
                startRow = 9;
            if (startCol < 0)
                startCol = 0;
            if (startCol > 8)
                startCol = 8;

            possiblePositions[positionsCount][0] = startRow;
            possiblePositions[positionsCount][1] = startCol;
            positionsCount++;
        }
    }

    if (positionsCount == 0)
    {
        int row = rand() % 9 + 1;
        char column = 'A' + (rand() % 9);
        SmokeScreen(bot, row, column);
        return;
    }

    int choice = rand() % positionsCount;
    int smokeRow = possiblePositions[choice][0];
    int smokeCol = possiblePositions[choice][1];

    SmokeScreen(bot, smokeRow, 'A' + smokeCol);
}

void selectHighestProbabilityCell(player *bot, int *targetRow, int *targetCol)
{
    int maxProb = 0;
    int maxCells[100][2];
    int maxCellCount = 0;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (bot->opponentGrid[i][j] == '~')
            {
                int prob = bot->probabilityGrid[i][j];
                if (prob > maxProb)
                {
                    maxProb = prob;
                    maxCellCount = 0;
                    maxCells[maxCellCount][0] = i;
                    maxCells[maxCellCount][1] = j;
                    maxCellCount = 1;
                }
                else if (prob == maxProb)
                {
                    maxCells[maxCellCount][0] = i;
                    maxCells[maxCellCount][1] = j;
                    maxCellCount++;
                }
            }
        }
    }

    int choice = rand() % maxCellCount;
    *targetRow = maxCells[choice][0];
    *targetCol = maxCells[choice][1];
}

void checkForSunkShips(player *bot, player *human)
{
    static int shipsSunkLastTurn = 0;
    int shipsSunkNow = 4 - human->shipsr;

    if (shipsSunkNow > shipsSunkLastTurn)
    {
        int shipsSunkThisTurn = shipsSunkNow - shipsSunkLastTurn;
        shipsSunkLastTurn = shipsSunkNow;

        if (shipsSunkThisTurn >= 1)
        {
            bot->artilleryUnlocked = 1;
        }
        if (shipsSunkNow == 3 && !bot->torpedoUnlocked)
        {
            bot->torpedoUnlocked = 1;
        }
    }
}

void selectHighestProbabilityArea(player *bot, int *targetRow, int *targetCol)
{
    int maxProb = 0;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int prob = bot->probabilityGrid[i][j] + bot->probabilityGrid[i][j + 1] +
                       bot->probabilityGrid[i + 1][j] + bot->probabilityGrid[i + 1][j + 1];
            if (prob > maxProb)
            {
                maxProb = prob;
                *targetRow = i;
                *targetCol = j;
            }
        }
    }
}

int decideRowOrColumn(player *bot, char *target)
{
    int rowSums[10] = { 0 };
    int colSums[10] = { 0 };

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (bot->opponentGrid[i][j] == '~')
            {
                rowSums[i] += bot->probabilityGrid[i][j];
                colSums[j] += bot->probabilityGrid[i][j];
            }
        }
    }

    int maxRowSum = 0, maxColSum = 0;
    int maxRow = 0, maxCol = 0;

    for (int i = 0; i < 10; i++)
    {
        if (rowSums[i] > maxRowSum)
        {
            maxRowSum = rowSums[i];
            maxRow = i;
        }
        if (colSums[i] > maxColSum)
        {
            maxColSum = colSums[i];
            maxCol = i;
        }
    }

    if (maxRowSum >= maxColSum)
    {
        *target = '1' + maxRow;
        return 1; 
    }
    else
    {
        *target = 'A' + maxCol;
        return 0; 
    }
}

void PlaceShip(player *player, int isBot)
{
    char ships[4][20] = { "Carrier", "Battleship", "Destroyer", "Submarine" };
    int shipSizes[4] = { 5, 4, 3, 2 }; 

    for (int i = 0; i < 4; i++)
    {
        int size = shipSizes[i];
        int isValid = 0;
        char column;
        int row;
        char orientation;

        while (!isValid)
        {
            if (!isBot)
            {
                printf("Enter the starting coordinate (e.g., A5) and orientation (H/V) for your %s (size %d): ", ships[i], size);
                char coordInput[10];
                scanf("%s %c", coordInput, &orientation);
                while (getchar() != '\n');

                column = toupper(coordInput[0]);
                if (coordInput[2] != '\0')
                    row = (coordInput[1] - '0') * 10 + (coordInput[2] - '0');
                else
                    row = coordInput[1] - '0';

                orientation = toupper(orientation);
            }
            else
            {
                char orientations[2] = { 'H', 'V' };
                orientation = orientations[rand() % 2];
                row = rand() % 10 + 1;
                column = 'A' + (rand() % 10);
            }

            isValid = isValidPlacement(player, column, row, orientation, size);
            if (!isValid && !isBot)
            {
                printf("Invalid placement. Try again.\n");
            }
        }

        int colIndex = column - 'A';
        int rowIndex = row - 1;
        player->ships[i].size = size;
        player->ships[i].hits = 0;
        player->ships[i].startrow = rowIndex;
        player->ships[i].startcolumn = colIndex;
        player->ships[i].orientation = orientation;
        player->ships[i].sunk = 0;

        if (orientation == 'H')
        {
            for (int j = 0; j < size; j++)
            {
                player->grid[rowIndex][colIndex + j] = 'S';
                player->smokegrid[rowIndex][colIndex + j] = 'S';
            }
        }
        else if (orientation == 'V')
        {
            for (int j = 0; j < size; j++)
            {
                player->grid[rowIndex + j][colIndex] = 'S';
                player->smokegrid[rowIndex + j][colIndex] = 'S';
            }
        }
    }
}