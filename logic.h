#ifndef LOGIC_H
#define LOGIC_H
#include <stdbool.h>
#include "board.h"


enum turn {
    BLACKS_TURN,
    WHITES_TURN
};

typedef enum turn turn;


enum outcome {
    IN_PROGRESS,
    BLACK_WIN,
    WHITE_WIN,
    DRAW
};

typedef enum outcome outcome;


struct game {
    unsigned int run;
    board* b;
    posqueue *black_queue, *white_queue;
    turn player;
};

typedef struct game game;

/*creates and returns a pointer to a new game struct
*with given parameters run, width, height, and game type*/
game* new_game(unsigned int run, unsigned int width,
               unsigned int height, enum type type);

/*frees a passed-in game struct, including internal allocations*/
void game_free(game* g);

/*given a game and a specified column, drops a game piece onto board
*drops the piece into the furthest row within given column
*piece color depends on player turn, if piece is placed, changes turns
*returns true if placed, else, returns false and does not change turns*/
bool drop_piece(game* g, unsigned int column);

/*given a game, the disarray move re-orients the game board
*to be flipped over the horizontal axis, and then adjusts for gravity
*this also adjusts the game queues to reflec the changes in position*/
void disarray(game* g);

void old_disarray(game* g);

/*given a game, offset removes the current player's oldest piece
*in exchange for removing the opponent's newest piece
*gravity is also adjusted, and returns a boolean if executed*/
bool offset(game* g);

/*given a game, determines the outcome of the game based on the current board
*returns WHITE_WINS or BLACK_WINS if a winner has been determined
*returns DRAW if both players have wins on the board or
*all spots in the board are covered and no winner exists
*returns IN_PROGRESS if the game is still ongoing without a winner*/
outcome game_outcome(game* g);

/*given two positions (an "oldest piece" and "newest piece") and a game,
*performs the offset move by removing the two given positions
*and adjusting for gravity - helper function used in offset*/
void offset_adjust(game* g, pos p1, pos p2);

#endif /* LOGIC_H */
