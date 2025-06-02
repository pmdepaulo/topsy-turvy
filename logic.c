#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "logic.h"
#include "board.h"
#include "pos.h"

game* new_game(unsigned int run, unsigned int width, 
                unsigned int height, enum type type){
    if (!(run <= width || run <= height)){
        fprintf(stderr, "run size is too large, impossible to win game");
        exit(1);
    }

    board* game_board = board_new(width, height, type);
    posqueue* black_queue = posqueue_new();
    posqueue* white_queue = posqueue_new();

    game* res_game = (game*)malloc(sizeof(game));
    if (res_game == NULL){
        fprintf(stderr, "new_game could not allocate memory for new game");
        exit(1);
    }

    res_game->run = run;
    res_game->b = game_board;
    res_game->black_queue = black_queue;
    res_game->white_queue = white_queue;
    res_game->player = BLACKS_TURN;
                
    return res_game;
}

void game_free(game* g){
    posqueue_free(g->black_queue);
    posqueue_free(g->white_queue);
    board_free(g->b);
    free(g);
}

bool drop_piece(game* g, unsigned int column){
    cell curr_piece;
    if (g->player == BLACKS_TURN){
        curr_piece = BLACK;
    } else{
        curr_piece = WHITE;
    }

    if (board_get(g->b, make_pos(0, column)) == EMPTY){
        pos temp_pos = make_pos(0, column);
        unsigned int i;
        for (i = g->b->height - 1; i >= 0; i--){
            temp_pos.r = i;
            if (board_get(g->b, temp_pos) == EMPTY){
                board_set(g->b, temp_pos, curr_piece);
                if (curr_piece == WHITE){
                    pos_enqueue(g->white_queue, temp_pos);
                    g->player = BLACKS_TURN;
                } else{
                    pos_enqueue(g->black_queue, temp_pos);
                    g->player = WHITES_TURN;
                }
                return true;
            }
        }
    }
    return false;
}

int update_queue(posqueue* q, pos new_pos, pos old_pos){
    pq_entry* posqueue_index = q->head;
    unsigned int counter = 1;
    bool index;
    while (counter < q->len){
        index = (posqueue_index->p.r == old_pos.r 
                && posqueue_index->p.c == old_pos.c);
        if (index){
            posqueue_index->p.r = new_pos.r;
            posqueue_index->p.c = new_pos.c;
            return 0;
        }
        posqueue_index = posqueue_index->next;
        counter++;
    }
    if (posqueue_index != NULL){
        index = (posqueue_index->p.r == old_pos.r 
                && posqueue_index->p.c == old_pos.c);
        if (index){
            posqueue_index->p.r = new_pos.r;
            posqueue_index->p.c = new_pos.c;
            return 0;
        }
    }
    return 0;
}

//disarray functions
struct args{
    game* g;
    unsigned int col;
};

/*given a game and a single column, performs a disarray move on one column only
*used only in pthread implementation of the matrix representation*/
void disarray_single_col(game* g, unsigned int col){
    pos bottom_p = make_pos(g->b->height - 1, 0);
    pos moving_p = make_pos(g->b->height - 1, 0);

    bottom_p.c = col;
    moving_p.c = col;
    bottom_p.r = g->b->height - 1;
    moving_p.r = g->b->height - 1;
    cell bottom_val = board_get(g->b, bottom_p);
    cell moving_val = board_get(g->b, moving_p);
        
    if (moving_val != EMPTY){
        while (moving_val != EMPTY && moving_p.r > 0){
            moving_p.r--;
            moving_val = board_get(g->b, moving_p);
        }
        if (moving_val == EMPTY){
            moving_p.r++;
            moving_val = board_get(g->b, moving_p);
        }

        while (bottom_p.r > moving_p.r && bottom_p.r >= 0){
            cell temp = moving_val;
            board_set(g->b, moving_p, bottom_val);
            if (moving_val == BLACK){
                update_queue(g->black_queue, bottom_p, moving_p);
            } else if (moving_val == WHITE){
                update_queue(g->white_queue, bottom_p, moving_p);
            }
            board_set(g->b, bottom_p, temp);
            if (bottom_val == BLACK){
                update_queue(g->black_queue, moving_p, bottom_p);
            } else if (bottom_val == WHITE){
                update_queue(g->white_queue, moving_p, bottom_p);
            }
            bottom_p.r--;
            moving_p.r++;
            bottom_val = board_get(g->b, bottom_p);
            moving_val = board_get(g->b, moving_p);
        }
    }
}

void* disarray_wrapper(void* a){
    struct args* args = (struct args*)a;
    disarray_single_col(args->g, args->col);
    return NULL;
}

void disarray(game* g){
    if (g->b->type == BITS){
        unsigned int i;
        for (i = 0; i < g->b->width; i++){
            disarray_single_col(g, i);
        }
    } else{
        pthread_t threads[g->b->width];
        struct args args[g->b->width];

        unsigned int i;
        for(i = 0; i < g->b->width; i++){
            args[i].g = g;
            args[i].col = i;
            pthread_create(&threads[i], NULL, disarray_wrapper, &args[i]);
        }

        for(i = 0; i < g->b->width; i++){
            pthread_join(threads[i], NULL);
        }
    }

    if (g->player == WHITES_TURN){
       g->player = BLACKS_TURN;
    } else{
        g->player = WHITES_TURN;
    }
}

void old_disarray(game* g){
    pos bottom_p = make_pos(g->b->height - 1, 0);
    pos moving_p = make_pos(g->b->height - 1, 0);

    for (int c = 0; c < g->b->width; c++){
        bottom_p.c = c;
        moving_p.c = c;
        bottom_p.r = g->b->height - 1;
        moving_p.r = g->b->height - 1;
        cell bottom_val = board_get(g->b, bottom_p);
        cell moving_val = board_get(g->b, moving_p);
        
        if (moving_val != EMPTY){
            while (moving_val != EMPTY && moving_p.r > 0){
                moving_p.r--;
                moving_val = board_get(g->b, moving_p);
            }
            if (moving_val == EMPTY){
                moving_p.r++;
                moving_val = board_get(g->b, moving_p);
            }

            while (bottom_p.r > moving_p.r && bottom_p.r >= 0){
                cell temp = moving_val;
                board_set(g->b, moving_p, bottom_val);
                if (moving_val == BLACK){
                    update_queue(g->black_queue, bottom_p, moving_p);
                } else if (moving_val == WHITE){
                    update_queue(g->white_queue, bottom_p, moving_p);
                }
                board_set(g->b, bottom_p, temp);
                if (bottom_val == BLACK){
                    update_queue(g->black_queue, moving_p, bottom_p);
                } else if (bottom_val == WHITE){
                    update_queue(g->white_queue, moving_p, bottom_p);
                }
                bottom_p.r--;
                moving_p.r++;
                bottom_val = board_get(g->b, bottom_p);
                moving_val = board_get(g->b, moving_p);
            }
        }
    }
    if (g->player == WHITES_TURN){

       g->player = BLACKS_TURN;
    } else{
        g->player = WHITES_TURN;
    }
}
//end of disarray functions

void offset_adjust(game* g, pos p1, pos p2){
    if (board_get(g->b, p1) == EMPTY || board_get(g->b, p2) == EMPTY){
        fprintf(stderr, "pos p is already empty on board b");
        exit(1);
    }

    pos moving_p = p1;
    pos temp_pos;
    board_set(g->b, p1, EMPTY);
    board_set(g->b, p2, EMPTY);

    if (moving_p.r > 0){
        do {
            temp_pos = moving_p;
            moving_p.r--;
            cell temp_cell = board_get(g->b, moving_p);
            board_set(g->b, temp_pos, temp_cell);
            if (temp_cell == BLACK){
                update_queue(g->black_queue, temp_pos, moving_p);
            } else{
                update_queue(g->white_queue, temp_pos, moving_p);
            }
        } while (board_get(g->b, moving_p) != EMPTY && moving_p.r > 0);
    }

    moving_p = p2;
    if (moving_p.r > 0){
        do {
            temp_pos = moving_p;
            moving_p.r--;
            cell temp_cell = board_get(g->b, moving_p);
            board_set(g->b, temp_pos, temp_cell);
            if (temp_cell == BLACK){
                update_queue(g->black_queue, temp_pos, moving_p);
            } else{
                update_queue(g->white_queue, temp_pos, moving_p);
            }
        } while (board_get(g->b, moving_p) != EMPTY && moving_p.r > 0);
    }
}

bool offset(game* g){
    if (g->black_queue->head == NULL || g->white_queue->head == NULL){
        return false;
    }
    pos oldest_pos = make_pos(0, 0);
    pos newest_pos = make_pos (0, 0);

    switch (g->player){
        case (BLACKS_TURN):
            oldest_pos = pos_dequeue(g->black_queue);
            newest_pos = posqueue_remback(g->white_queue);
            offset_adjust(g, oldest_pos, newest_pos);
            g->player = WHITES_TURN;
            break;
        case (WHITES_TURN):
            oldest_pos = pos_dequeue(g->white_queue);
            newest_pos = posqueue_remback(g->black_queue);
            offset_adjust(g, oldest_pos, newest_pos);
            g->player = BLACKS_TURN;
            break;
    }
    return true;
}

outcome game_outcome(game* g){
    unsigned int HEIGHT = g->b->height;
    unsigned int WIDTH = g->b->width;
    unsigned int run_counter = 0;
    cell curr_val;
    cell moving_val;
    pos start_pos = make_pos(0, 0);
    pos moving_pos = make_pos(0, 0);
    bool can_draw = true;
    bool black_win = false;
    bool white_win = false;

    unsigned int i, j;
    for (i = 0; i < HEIGHT; i++){
        for (j = 0; j < WIDTH; j++){
            start_pos.r = i;
            start_pos.c = j;
            curr_val = board_get(g->b, start_pos);
            if (curr_val == EMPTY){
                can_draw = false;
            } else{
                //horizontal pass
                if (start_pos.c < WIDTH){
                    moving_pos = start_pos;
                    run_counter = 0;
                    moving_val = board_get(g->b, moving_pos);
                    do {
                        moving_val = board_get(g->b, moving_pos);
                        run_counter++;
                        if (run_counter == g->run && moving_val == curr_val){
                            if (curr_val == BLACK){
                                black_win = true;
                            } else if (curr_val == WHITE){
                                white_win = true;
                            }
                        }
                        moving_pos.c++;
                    } while (moving_val == curr_val && moving_pos.c < WIDTH);
                }

                //vertical pass
                if (start_pos.r < HEIGHT){
                    moving_pos = start_pos;
                    run_counter = 0;
                    do {
                        moving_val = board_get(g->b, moving_pos);
                        run_counter++;
                        if (run_counter == g->run && moving_val == curr_val){
                            if (curr_val == BLACK){
                                black_win = true;
                            } else if (curr_val == WHITE){
                                white_win = true;
                            }
                        }
                        moving_pos.r++;
                    } while (moving_val == curr_val && moving_pos.r < HEIGHT);
                }

                //left diagonal pass
                if (start_pos.r < HEIGHT && start_pos.c > 0){
                    
                    moving_pos = start_pos;
                    run_counter = 0;
                    do{
                        moving_val = board_get(g->b, moving_pos);
                        run_counter++;
                        if (run_counter == g->run && moving_val == curr_val){
                            if (curr_val == BLACK){
                                black_win = true;
                            } else if (curr_val == WHITE){
                                white_win = true;
                            }
                        }
                        moving_pos.r++;
                        moving_pos.c--;
                    } while (moving_val == curr_val && moving_pos.r < HEIGHT &&
                            moving_pos.c >= 0);
                }

                //right diagonal pass
                if (start_pos.r < HEIGHT && start_pos.c < WIDTH){
                    moving_pos = start_pos;
                    run_counter = 0;
                    do {
                        moving_val = board_get(g->b, moving_pos);
                        run_counter++;
                        if (run_counter == g->run && moving_val == curr_val){
                            if (curr_val == BLACK){
                                black_win = true;
                            } else if (curr_val == WHITE){
                                white_win = true;
                            }
                        }
                        moving_pos.r++;
                        moving_pos.c++;
                    } while (moving_val == curr_val && moving_pos.r < HEIGHT &&
                            moving_pos.c < WIDTH);
                }
            }
        }
    }
    if (can_draw || (black_win && white_win)){
        return DRAW;
    } else if (black_win && !white_win){
        return BLACK_WIN;
    } else if (white_win && !black_win){
        return WHITE_WIN;
    }
    return IN_PROGRESS;
}