#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int xSize, ySize;
    char **mineField;
    char **gameField;
    int mineCount;
    int hiddenCount;
} Board;

Board createBoard(int xSize, int ySize);
void destroyBoard(Board *board);
void startBoard(Board *board, int x, int y, int mines);
void flagCell(Board *board, int x, int y);
int revealCell(Board *board, int x, int y);
void showBoard(Board *board);
int my_strtok(char ***tokens, char *string, char del);
bool my_strcmp(char *string1, char *string2);

int main() {
    srand(time(NULL));
    Board board;
    bool isRunning = true;
    bool isPlaying = true;
    int count;
    int x;
    int y;
    int result;
    char command[16];
    char **tokens = NULL;
    int tokcount;
    while (isRunning) {
        printf("Choose the dimensions: ");
        fgets(command, 16, stdin);
        my_strtok(&tokens, command, ' ');

        board = createBoard(atoi(tokens[0]), atoi(tokens[1]));

        printf("Choose a mine count: ");
        fgets(command, 16, stdin);
        count = atoi(command);
        printf("Choose a starting place: ");
        fgets(command, 16, stdin);
        my_strtok(&tokens, command, ' ');

        startBoard(&board, atoi(tokens[0]), atoi(tokens[1]), count);
        revealCell(&board, atoi(tokens[0]), atoi(tokens[1]));

        showBoard(&board);

        isPlaying = true;

        while (isPlaying) {
            printf("> ");
            fgets(command, 16, stdin);
            tokcount = my_strtok(&tokens, command, ' ');
            if (tokcount > 3) {
                printf("Error, Too many arguments!\n");
            } else if (tokcount < 3) {
                printf("Error, Too few arguments!\n");
            } else if (my_strcmp(tokens[0], "flag")) {
                flagCell(&board, atoi(tokens[1]), atoi(tokens[2]));
            } else if (my_strcmp(tokens[0], "reveal")) {
                revealCell(&board, atoi(tokens[1]), atoi(tokens[2]));
            } else {
                printf("Error, Invalid command!\n");
            }
            
            if (result == 1) {
                printf("You Lost!\n");
                isPlaying = false;
            }
            if (board.hiddenCount == 0) {
                printf("You Won!\n");
                isPlaying = false;
            }
            showBoard(&board);
        }
        destroyBoard(&board);
        for (int i = 0; i < tokcount; i++) {
            free(tokens[i]);
        }
        free(tokens);

        printf("Again?(Y/N) ");
        fgets(command, 16, stdin);

        if (my_strcmp(command, "N") == 0) {
            isRunning = false;
        }
    }

    return 0;
}

Board createBoard(int xSize, int ySize) {
    Board board;
    board.xSize = xSize;
    board.ySize = ySize;

    board.mineField = malloc(sizeof(char*) * xSize);
    for (int i = 0; i < xSize; i++) {
        board.mineField[i] = malloc(sizeof(char) * ySize);
    }

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            board.mineField[i][j] = 'N';
        }
    }

    board.gameField = malloc(sizeof(char*) * xSize);
    for (int i = 0; i < xSize; i++) {
        board.gameField[i] = malloc(sizeof(char) * ySize);
    }

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            board.gameField[i][j] = '#';
        }
    }

    return board;
}

void destroyBoard(Board *board) {
    for (int i = 0; i < board->xSize; i++) {
        free(board->mineField[i]);
    }
    free(board->mineField);
}

void setDigits(Board *board) {
    int neighbors[8][2] = {{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}};
    int count;
    for (int i = 0; i < board->xSize; i++) {
        for (int j = 0; j < board->ySize; j++) {
            if (board->mineField[i][j] != 'O') {
                count = 0;
                for (int k = 0; k < 8; k++) {
                    if (i+neighbors[k][0] >= 0 && i+neighbors[k][0] < board->xSize && j+neighbors[k][1] >= 0 && j+neighbors[k][1] < board->ySize) {
                        if (board->mineField[i+neighbors[k][0]][j+neighbors[k][1]] == 'O') {
                            count++;
                        }
                    }
                }
                board->mineField[i][j] = count + '0';
            }
        }
    }
}

void startBoard(Board *board, int x, int y, int mines) {
    if (x >= board->xSize || y >= board->ySize) {
        printf("Error, Out of bounds!\n");
        return;
    }

    board->mineCount = mines;
    board->hiddenCount = mines;

    int neighbors[8][2] = {{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}};
    for (int i = 0; i < 8; i++) {
        if (x+neighbors[i][0] >= 0 && x+neighbors[i][0] < board->xSize && y+neighbors[i][1] >= 0 && y+neighbors[i][1] < board->ySize) {
            board->mineField[x+neighbors[i][0]][y+neighbors[i][1]] = 'T';
        }
    }
    board->mineField[x][y] = 'T';

    int xRand, yRand;
    for (int i = 0; i < mines; i++) {
        xRand = (int)(((float)rand() / RAND_MAX) * (board->xSize - 1));
        yRand = (int)(((float)rand() / RAND_MAX) * (board->ySize - 1));
        if (board->mineField[xRand][yRand] == 'T' || board->mineField[xRand][yRand] == 'O') {
            i--;
        } else {
            board->mineField[xRand][yRand] = 'O';
        }
    }

    for (int i = 0; i < 8; i++) {
        if (x+neighbors[i][0] >= 0 && x+neighbors[i][0] < board->xSize && y+neighbors[i][0] >= 0 && y+neighbors[i][0] < board->ySize) {
            board->mineField[x+neighbors[i][0]][y+neighbors[i][1]] = 'N';
        }
    }
    board->mineField[x][y] = 'N';

    setDigits(board);
}

void flagCell(Board *board, int x, int y) {
    if (x < 0 || x >= board->xSize || y < 0 || y >= board->ySize) {
        printf("Error, Out of bounds!\n");
        return;
    }
    if (board->gameField[x][y] == '#') {
        board->gameField[x][y] = 'F';
        board->mineCount--;
        if (board->mineField[x][y] == 'O') {
            board->hiddenCount--;
        }
    } else if (board->gameField[x][y] == 'F') {
        board->gameField[x][y] = '#';
        board->mineCount++;
        if (board->mineField[x][y] == 'O') {
            board->hiddenCount++;
        }
    }

    

}

int revealCell(Board *board, int x, int y) {
    if (x < 0 || x >= board->xSize || y < 0 || y >= board->ySize) {
        return -1;
    }
    if (board->gameField[x][y] != '#') return 0;
    if (board->mineField[x][y] == 'O') {
        board->mineField[x][y] = 'O';
        return 1;
    } 
    if (board->mineField[x][y] != '0') {
        board->gameField[x][y] = board->mineField[x][y];
        return 0;
    }
    board->gameField[x][y] = '.';
    int neighbors[8][2] = {{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}};
    for (int i = 0; i < 8; i++) {
        revealCell(board, x+neighbors[i][0], y+neighbors[i][1]);
    }
}

void showBoard(Board *board) {
    printf("\nMines: %d\n", board->mineCount);
    printf("  ");
    for (int i = 0; i < board->xSize; i++) {
        printf("%2d ", i);
    }
    for (int i = 0; i < board->ySize; i++) {
        for (int j = 0; j < board->xSize; j++) {
            if (j == 0) {
                printf("\n%2d %c", i, board->gameField[j][i]);
            } else {
                printf("  %c", board->gameField[j][i]);
            }
        }
    }
    printf("\n");
}

int my_strtok(char ***tokens, char *string, char del) {
    int ptr = 0;
    int strptr = 0;
    int tokptr = 0;
    (*tokens) = realloc((*tokens), sizeof(char*));
    (*tokens)[tokptr] = NULL;
    while (string[ptr] != 0) {
        if (string[ptr] == del) {
            (*tokens)[tokptr] = realloc((*tokens)[tokptr], (strptr+1)*sizeof(char));
            (*tokens)[tokptr][strptr] = 0;
            tokptr++;
            (*tokens) = realloc((*tokens), (tokptr+1)*sizeof(char*));
            (*tokens)[tokptr] = NULL;
            strptr = 0;
        } else {
            (*tokens)[tokptr] = realloc((*tokens)[tokptr], (strptr+1)*sizeof(char));
            (*tokens)[tokptr][strptr] = string[ptr];
            strptr++;
        }
        ptr++;
    }
    (*tokens)[tokptr] = realloc((*tokens)[tokptr], (strptr+1)*sizeof(char));
    (*tokens)[tokptr][strptr] = 0;
    return ++tokptr;
}

bool my_strcmp(char *string1, char *string2) {
    int ptr = 0;
    while (string1[ptr] != 0 || string2[ptr] != 0) {
        if (string1[ptr] != string2[ptr]) {
            return false;
        }
        ptr++;
    }
    return true;
}