//  -------------------------------
//  main.c
//  Minesweeper
//
//  Author: Aidan Fox
//
//  Date Created: 9/17/23.
//  Last Edited:  9/22/23.
//  -------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// -----------------------------------
// ### Command Function Prototypes ###
// -----------------------------------

void command_new(int, int, int);
void command_show(void);

//----------------------------------------
// ### Cell Struct Function Prototypes ###
//----------------------------------------
void init_cell(cell*, int, int);
void adj_check(cell*, int, int);
void display_cell(cell*);

//global variables
cell **board;                        // 2D array, declared as pointer to a pointer to cell.
int rows, cols, mines, mineCounter;  // number of rows, columns mines respectively.


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
    }else if (strcmp(tokens[0], "quit") == 0){
        return 0;
    }
    return -1;
}

int run_game(){
    char line[MAXLINELENGTH];
    char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];
    
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

// -----------------
// command functions
// -----------------

void command_new(int r, int c, int m){
    rows = r;
    cols = c;
    mines = m;
    
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
    
    /* Implement at a Later Time
     
    // for each cell call adj_check() to set adjacency values
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            adj_check(&board[i][j], i, j);
        }
    }
     
    */
}

void command_show(){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            display_cell(&board[i][j]);
        }
        printf("\n");
    }
    
}


//-----------------------
// Cell Struct Functions
//-----------------------

void init_cell(cell* cell, int r, int c){
    int pos = (r * cols) + c;
    (*cell).position = pos;
    // other struct members tbi at a later time
}

void adj_check(cell* cell, int r, int c){
    // Loops through board positions from board[r - 1][c - 1] to board[r + 1][c + 1]
    // skipping itself, and incrementing adjcount for each mined cell found.
    // Visual Representation of cells being checked:
    //  [Cell r-1, c-1]     [Cell r-1, c ]     [Cell r-1, c+1]
    //  [Cell r  , c-1]     [You are here]     [Cell r,   c+1]
    //  [Cell r+1, c-1]     [Cell r+1, c ]     [Cell r+1, c+1]
    //
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            if(i == 0 && j == 0)
                continue;
            else if (r+i < 0 || c + j < 0)
                continue;
            else{
                if(board[r+i][c+j].mined == 1)
                    (*cell).adjcount++;
            }
        }
    }
}

void display_cell(cell *c){
    if(c->flagged == 1) printf("P ");
    else if(c->covered == 1) printf("/ ");
    else printf("%2d ", c->position);
}
