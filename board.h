#ifndef BOARD_H
#define BOARD_H
#include "pos.h"

enum cell {
    EMPTY,
    BLACK,
    WHITE
};

typedef enum cell cell;

union board_rep {
    enum cell** matrix;
    unsigned int* bits;
};

typedef union board_rep board_rep;

enum type {
    MATRIX, BITS
};

struct board {
    unsigned int width, height;
    enum type type;
    board_rep u;
};

typedef struct board board;

/*creates and returns a pointer to a new game board with width and height
*assigns all cells to empty as default
*also takes in type, currently only supports matrix board types*/
board* board_new(unsigned int width, unsigned int height, enum type type);

/*given a pointer to a board, fully frees the board and all cells within*/
void board_free(board* b);

/*prints a visual representation of a passed-in board b to the screen
*columns and rows are */
void board_show(board* b);

/*given a pointer to a board b and a pos p, returns the cell value at pos
*raises an error if p is out of bounds for b*/
cell board_get(board* b, pos p);

/*sets position p on board b to a given cell value c
*function is void and raises an error if pos p is out of bounds for b*/
void board_set(board* b, pos p, cell c);

#endif /* BOARD_H */
