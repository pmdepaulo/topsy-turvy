#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "logic.h"

/*Given a game and an outparameter m, prompts the player for their move
*Function is void, only updates the outparameter passed in*/
void get_player_move(game* g, char* m){
    if (g->player == BLACKS_TURN){
        printf("\n");
        printf("Black: ");
    } else{
        printf("\n");
        printf("White: ");
    }
    scanf("%c%*c", m);
}

/*Checks the validity of a passed-in char for the game specifications
*Returns a boolean indicating whether or not the char is acceptable*/
bool check_move_validity(game* g, char move){
    if (move == '!'){
        return true;
    } else if (move == '^'){
        return true;
    } else if (move >= '0' && move <= '9'){
        if (move - '0' <= g->b->width - 1){
            return true;
        }
        return false;
    } else if (move >= 'A' && move <= 'Z'){
        if ((move - 'A') + 10 <= g->b->width - 1){
            return true;
        }
        return false;
    } else if (move >= 'a' && move <= 'z'){
       if ((move - 'a') + 36 <= g->b->width - 1){
            return true;
        }
        return false;
    } 
    return false;
}

/*Acts as the main loop for the game
*Prompts each player for their move and executes corresponding actions
*Keeps the game running as long as there exists no winner or no moves left*/
void game_loop(game* g){
    outcome outcome = game_outcome(g);
    while(outcome == IN_PROGRESS){
        printf("\n");
        board_show(g->b);

        char move;
        get_player_move(g, &move);
        
        //gets it wrong - keep bugging them
        while(!check_move_validity(g, move)){
            printf("\n");
            printf("Invalid move, please try again.");
            get_player_move(g, &move);
        }

        //gets it correct
        if (move == '!'){
            bool move_made = offset(g);
            if (!move_made){
                printf("\n");
                printf("Invalid move! Too little pieces for offset, try again.");
            }
        } else if (move == '^'){
            disarray(g);
        } else if (move >= '0' && move <= '9'){
            unsigned int col = move - '0';
            bool move_made = drop_piece(g, col);
            if (!move_made){
                printf("\n");
                printf("Invalid move! Column is full, try again.");
            }
        } else if (move >= 'A' && move <= 'Z'){
            unsigned int col = (move - 'A') + 10;
            bool move_made = drop_piece(g, col);
            if (!move_made){
                printf("\n");
                printf("Invalid move! Column is full, try again.");
            }
        } else if (move >= 'a' && move <= 'z'){
            unsigned int col = (move - 'a') + 36;
            bool move_made = drop_piece(g, col);
            if (!move_made){
                printf("\n");
                printf("Invalid move! Column is full, try again.");
            }
        }
        outcome = game_outcome(g);
    }
    printf("\n");
    board_show(g->b);
    printf("\n");
    if (outcome == BLACK_WIN){
        printf("\nBLACK WINS - GAME OVER");
    } else if (outcome == WHITE_WIN){
        printf("\nWHITE WINS - GAME OVER");
    } else{
        printf("\nDRAW - GAME OVER");
    }
    game_free(g);
    exit(1);
}

int main(int argc, char* argv[]){
    if (argc != 8){
        fprintf(stderr, "Incorrect number of game specifications");
        exit(1);
    }

    unsigned int height;
    unsigned int width;
    unsigned int run;
    enum type type;

    unsigned int i = 0;
    for (i = 0; i < argc; i++){
        if (!strcmp(argv[i], "-h")){
            if (!atoi(argv[i + 1])){
                fprintf(stderr, "Incorrect parameters, please give integers");
                exit(1);
            }
            height = atoi(argv[i + 1]);
        } else if (!strcmp(argv[i], "-w")){
            if (!atoi(argv[i + 1])){
                fprintf(stderr, "Incorrect parameters, please give integers");
                exit(1);
            }

            if (atoi(argv[i + 2]) > 63){
                fprintf(stderr, "Unplayable width size, must be less than 63");
                exit(1);
            }

            width = atoi(argv[i + 1]);
        } else if (!strcmp(argv[i], "-r")){
            if (!atoi(argv[i + 1])){
                fprintf(stderr, "Incorrect parameters, please give integers");
                exit(1);
            }
            run = atoi(argv[i + 1]);
        } else if (!strcmp(argv[i], "-b")){
            type = BITS;
        } else if (!strcmp(argv[i], "-m")){
            type = MATRIX;
        }
    }

    game* g = new_game(run, width, height, type);
    game_loop(g);
    return 0;
}