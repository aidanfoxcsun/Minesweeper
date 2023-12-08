//  -------------------------------
//  main.c
//  Minesweeper
//
//  Author: Aidan Fox
//
//  Date Created: 9/17/23.
//  Last Edited:  11/30/23.
//  -------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Global Constants

#define MAXTOKENCOUNT 20
#define MAXTOKENLENGTH 20
#define MAXLINELENGTH 400

// -------------------
// ### Cell Struct ###
// -------------------

struct cell{
    int position;       // value used to show location of cell in 2D board.
    int adjcount;       // value from 0-8 indicating how many adjacent cells are mines.
    int mined;          // 0/1 to indicate whether the cell has a mine(1) or not(0).
    int covered;        // 0/1 to indicate whether the cell is covered(1) or not(0).
    int flagged;        // 0/1 to indicate whether the cell has a flag(1) or not(0).
};
typedef struct cell cell;

// -------------------------------------------
// ### Input Processing Fuction Prototypes ###
// -------------------------------------------

int run_game(void);
void get_line(char[], int);
void get_tokens(char[], char[][MAXTOKENLENGTH], int*);
int process_command(char[][MAXTOKENLENGTH], int);

// ----------------
// Memory Functions
// ----------------
void free_space(void);

// -----------------------------------
// ### Command Function Prototypes ###
// -----------------------------------

void command_new(int, int, int);
void command_show(void);
int command_uncover(int, int);
int command_flag(int, int);

//----------------------------------------
// ### Cell Struct Function Prototypes ###
//----------------------------------------

void init_cell(cell*, int, int);
void adj_check(cell*, int, int);
void adj_mine(cell*, int, int);
void display_cell(cell*);
int get_random(int range);
void mine_random(void);
void mine_shuffle(int shuffle);

// -------------------------------------
// ### Gameplay Function Prototypes ####
// -------------------------------------

void recursive_uncover(int, int);
int win_game(void);
void game_over(void);


//global variables
cell **board;                        // 2D array, declared as pointer to a pointer to cell.
int rows, cols, mines, flags = 0;    // number of rows, columns, mines, and flags respectively.
int mineCounter;
const int neighborcount = 8;
const int row_neighbors[] = {-1, -1, 0, 1, 1,  1,  0, -1}; //offset arrays for neighbor cells
const int col_neighbors[] = { 0,  1, 1, 1, 0, -1, -1, -1};

// ---------------------
// ### Main Function ###
// ---------------------

int main(void) {
    run_game();
    return 0;
}

// --------------------------------
// Input Processing Implementations
// --------------------------------

void get_line(char line[], int maxlinelength){
    fgets(line, maxlinelength, stdin);
    int linelen = (int)strlen(line);
    if(line[linelen-1] == '\n')
        line[linelen-1] = '\0';
}

void get_tokens(char line[], char tokens[][MAXTOKENLENGTH], int *count){
    char linecopy[MAXLINELENGTH];
    strcpy(linecopy, line);
    char *p;
    p = strtok(linecopy, " ");
    int tokencount = 0;
    
    while(p != NULL){
        strcpy(tokens[tokencount],p);
        
        p = strtok(NULL, " ");
        tokencount++;
    }
    
    *count = tokencount;
}

int process_command(char tokens[][MAXTOKENLENGTH], int tokencount){
    if(strcmp(tokens[0], "new") == 0){
        command_new(atoi(tokens[1]), atoi(tokens[2]), atoi(tokens[3]));
        return 1;
    }else if (strcmp(tokens[0], "show") == 0){
        command_show();
        return 1;
    }else if (strcmp(tokens[0], "uncover") == 0){
        return command_uncover(atoi(tokens[1]), atoi(tokens[2]));
    }else if (strcmp(tokens[0], "flag") == 0){
        return command_flag(atoi(tokens[1]), atoi(tokens[2]));
    }else if (strcmp(tokens[0], "quit") == 0){
        return 0;
    }
    return -1;
}

int run_game(){
    char line[MAXLINELENGTH];
    char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];
    
    srand((unsigned)time(0));
    
    printf("\n\n### Welcome to Minesweeper! ###\n\nCommand list:\n");
    printf("[new] r c m: Creates a new board with r rows, c columns, and m mines.\n");
    printf("[show]: Shows the current board.\n");
    printf("[uncover] r c: Uncovers the cell at r, c.\n");
    printf("[flag] r c: Flags or unflags the cell at r, c.\n");
    printf("[quit]: Ends the game.\n\n");
    
    while(1){
        int tokencount, status;
        printf(">> ");
        get_line(line, MAXLINELENGTH);
        get_tokens(line, tokens, &tokencount);
        
        status = process_command(tokens, tokencount);
        if(status == 0)
            break;
        else if (status == -1)
            printf("Invalid Command, please try again.\n");
        
    }
    return 1;
}

// ----------------
// Memory Functions
// ----------------
void free_space(){
    for(int i = 0; i < rows; i++){
        free(board[i]);
        board[i] = NULL;
    }
    free(board);
    board = NULL;
    rows = 0;
    cols = 0;
    mines = 0;
    //printf("Board Cleared Successfully!\n");
}

// -----------------
// command functions
// -----------------

void command_new(int r, int c, int m){
    free_space(); // free up previously allocated space to prevent memory leaks
    
    printf("Generating new board with dimensions : %d x %d and %d mines...\n", r, c, m);
    
    rows = r;
    cols = c;
    mines = m;
    mineCounter = mines;
    
    //allocate intial row vector
    board = (cell **) malloc(sizeof(cell*) * rows);
    
    // for each element of row vector, allocate row
    for(int i = 0; i < rows; i++){
        board[i] = (cell *) malloc(sizeof(cell) * cols);
    }
    
    // for each cell call init_cell() to set intial values
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            init_cell(&board[i][j], i, j);
        }
    }
    
    //mine_shuffle(rows*cols*mines);
    mine_random();
    
    /*
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            adj_check(&board[i][j], i, j);
        }
    }
    */
    
    // different adjacency method
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            if(board[r][c].mined == 1){
                adj_mine(&board[r][c], r, c);
            }
        }
    }
    //printf("Board Generated Successfuly!\n");
}

void command_show(){
    printf("\n   ");
    for(int c = 0; c < cols; c++){
        printf("%d ", c);
    }
    printf("\n\n");
    for(int i = 0; i < rows; i++){
        printf("%d ", i);
        for(int j = 0; j < cols; j++){
            display_cell(&board[i][j]);
        }
        printf("\n");
    }
    printf("Total Mines: %d\n", mines);
    printf("Total flags used: %d\n\n", flags);
    
}

int command_uncover(int r, int c){
    if(r > rows-1 || c > cols-1 || r < 0 || c < 0){
        printf("Please enter a valid cell\n");
        return 1;
    }
    if(board[r][c].mined == 1){
        game_over();
    }else if(board[r][c].flagged == 1){
        printf("Cell at %d, %d is flagged\n", r, c);
        return 1;
    }else{
        recursive_uncover(r, c);
    }
    command_show();
    if(win_game() == 1){
        printf("Congrats! You win!\n");
        return 0;
    }
    return 1;
}

int command_flag(int r, int c){
    if(r > rows-1 || c > cols-1 || r < 0 || c < 0){
        printf("Please enter a valid cell\n");
        return 1;
    }
    if(board[r][c].flagged == 0){
        board[r][c].flagged = 1;
        flags++;
    }else{
        board[r][c].flagged = 0;
        flags--;
    }
    command_show();
    if(win_game() == 1){
        printf("Congrats! You win!\n");
        return 0;
    }
    return 1;
}


//-----------------------
// Cell Struct Functions
//-----------------------

void init_cell(cell* cell, int r, int c){
    
    int pos = (r * cols) + c;
    cell->position = pos;
    cell->covered = 1;
    cell->mined = 0;
    cell->adjcount = 0;
    
    
}

void adj_check(cell* cell, int r, int c){
    // Loops through board positions from board[r - 1][c - 1] to board[r + 1][c + 1]
    // skipping itself, and incrementing adjcount for each mined cell found.
    // Visual Representation of cells being checked:
    //  [Cell r-1, c-1]     [Cell r-1, c ]     [Cell r-1, c+1]
    //  [Cell r  , c-1]     [You are here]     [Cell r,   c+1]
    //  [Cell r+1, c-1]     [Cell r+1, c ]     [Cell r+1, c+1]
    //
    int minecount = 0;
    
    for(int d = 0; d < neighborcount; d++){
        int rn = r + row_neighbors[d];
        int cn = c + col_neighbors[d];
        if(0 <= rn && rn < rows && 0 <= cn && cn < cols){ // check to make sure we aren't checking out of bounds
            if(board[rn][cn].mined == 1)
                minecount++;
        }
    }
    cell->adjcount = minecount;
}

void adj_mine(cell* this, int r, int c){
    for(int d = 0; d < neighborcount; d++){
        int rn = r + row_neighbors[d];
        int cn = c + col_neighbors[d];
        if(0 <= rn && rn < rows && 0 <= cn && cn < cols){ // check to make sure we aren't checking out of bounds
            board[rn][cn].adjcount++;
        }
    }
    
}

void display_cell(cell *c){
    if(c->flagged == 1) printf("%2s", "P");
    else if(c->covered == 1) printf("%2s", "#");
    else if(c->mined == 1) printf("%2s", "*");
    else if(c->adjcount == 0) printf("%2s", ".");
    else printf("%2d", c->adjcount);
    //else printf("%2d ", c->position);
}

int get_random(int range){
    return ((int)floor((float)range*rand()/RAND_MAX))%range;
}

void mine_random(){
    //printf("Total mines being generated: %d\n", mines);
    for(int m = 0; m < mines; m++){
        int r = get_random(rows);
        int c = get_random(cols);
        while(board[r][c].mined == 1){
            r = get_random(rows);
            c = get_random(cols);
        }
        board[r][c].mined = 1;
    }
}

// * NOT USED IN CURRENT VERSION * //
void mine_shuffle(int shuffle){
    // intialize all the first few rows with mined cells
    for(int p = 0; p < mines; p++){
        int r = p/(rows);
        int c = p%(cols);
        board[r][c].mined = 1;
    }
    
    // shuffling time
    for(int s = 0; s < shuffle; s++){
        int r1 = get_random(rows);      // pick two random cells
        int c1 = get_random(cols);
        int r2 = get_random(rows);
        int c2 = get_random(cols);
        int x = board[r1][c1].mined;    // swap their mined values
        int y = board[r2][c2].mined;
        board[r1][c1].mined = y;
        board[r2][c2].mined = x;
    }
}

// ---------------------------
// ### Gameplay Functions ###
// ---------------------------

// Scuffed DFS recursive algorithm.
void recursive_uncover(int r, int c){
    if(board[r][c].adjcount > 0){
        board[r][c].covered = 0;
        return;
    }
    board[r][c].covered = 0;
    
    for(int d = 0; d < neighborcount; d++){
        int rn = r + row_neighbors[d];
        int cn = c + col_neighbors[d];
        if(0 <= rn && rn < rows && 0 <= cn && cn < cols){ // check to make sure we aren't checking out of bounds
            if(board[rn][cn].covered != 0){
                board[rn][cn].covered = 0;
                recursive_uncover(rn, cn);
            }
        }
    }
    
}

int win_game(){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            if(board[i][j].mined == 1 && board[i][j].flagged == 0) return 0;
            else if (board[i][j].mined == 0 && board[i][j].covered == 1)return 0;
            // if a mine is not flagged, or an empty cell is still covered, then no win.
        }
    }
    return 1;
}

void game_over(){
    for(int r = 0; r < rows; r++){ // helper function to show all cells on lose.
        for(int c = 0; c < cols; c++){
            board[r][c].covered = 0;
        }
    }
    command_show();
    printf("You hit a mine! Game Over!\n");
    
    
}

