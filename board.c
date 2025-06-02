#include <stdio.h>
#include <stdlib.h>
#include "board.h"

board* board_new(unsigned int width, unsigned int height, enum type type){
    board* new_board = (board*)malloc(sizeof(board));
    if (new_board == NULL){
        fprintf(stderr, "board_new could not allocate memory for new board");
        exit(1);
    }
    new_board->width = width;
    new_board->height = height;
    new_board->type = type;

    if (type == MATRIX){
        new_board->u.matrix = (cell**)malloc(sizeof(cell*) * height);
        if (new_board->u.matrix == NULL){
            fprintf(stderr, "board_new could not allocate memory for matrix");
            exit(1);
        }

        unsigned int i, j;
        for (i = 0; i < height; i++){
            new_board->u.matrix[i] = (cell*)malloc(sizeof(cell) * width);
            if (new_board->u.matrix[i] == NULL){
                fprintf(stderr, "board_new could not allocate memory for row");
                exit(1);
            }
            for (j = 0; j < width; j++){
                new_board->u.matrix[i][j] = EMPTY;
            }
        }
    } else if (type == BITS){
        unsigned int bit_board_size = (width * height * 2) / 32;
        if ((width * height) % 32 != 0){
            bit_board_size++;
        }
        new_board->u.bits = (unsigned int*)calloc(sizeof(unsigned int) * \
        bit_board_size, 1);
        if (new_board->u.bits == NULL){
            fprintf(stderr, "board_new could not allocate memory for bit board");
        }
    }
    return new_board;
}

cell board_get(board* b, pos p){
    if (p.r >= b->height || p.c >= b->width){
        printf("\n %d, %d", p.r, p.c);
        fprintf(stderr, "given position for board_get is out of bounds");
        exit(1);
    }

    //cell_num uses zero-based indexing
    cell res_cell;
    if (b->type == BITS){
        unsigned int cell_num = (p.r * b->width) + p.c;
        unsigned int index = (cell_num * 2) / 32;
        res_cell = b->u.bits[index] & (0x03 << (2 * cell_num));
        res_cell >>= (2 * cell_num);
    } else{
        res_cell = b->u.matrix[p.r][p.c];
    }
    return res_cell;
}

void board_set(board* b, pos p, cell c){
    if (p.r >= b->height || p.c >= b->width){
        fprintf(stderr, "given position for board_set is out of bounds");
        exit(1);
    }

    //cell_num uses zero-based indexing
    if (b->type == BITS){
        unsigned int cell_num = (p.r * b->width) + p.c;
        unsigned int index = (cell_num * 2) / 32;
        unsigned int cleared_mask = b->u.bits[index] & ~(0x03 << (2 * cell_num));
        b->u.bits[index] = (c << (cell_num * 2)) | cleared_mask;
    } else{
        b->u.matrix[p.r][p.c] = c;
    }
}

void board_free(board* b){
    unsigned int i;
    if (b->type == MATRIX){
        for (i = 0; i < b->height; i++){
            free(b->u.matrix[i]);
        }
        free(b->u.matrix);
    } else{
        free(b->u.bits);
    }
    free(b);
}

void board_show(board* b){
    unsigned int i, j;
    unsigned char counter = 48;
    unsigned int question_marks = 0;
    pos temp_pos = make_pos(0, 0);
    
    for (i = 0; i < b->height + 2; i++){
        for (j = 0; j < b->width + 2; j++){
            if (i == 0){
                if (j == 0 || j == 1){
                    printf(" ");
                } else{
                    printf("%c", counter);
                    if (counter == 57 && !question_marks){
                        counter = counter + 8;
                    } else if (counter == 90 && !question_marks){
                        counter = counter + 7;
                    } else if (counter >= 122){
                        counter = 63;
                        question_marks = 1;
                    } else{
                        if (!question_marks){
                            counter++;
                        }
                    }
                }
            } else if (i == 1){
                question_marks = 0;
                counter = 48;
                printf(" ");
            } else{
                if (j == 0){
                    printf("%c", counter);
                    if (counter == 57 && !question_marks){
                        counter = counter + 8;
                    } else if (counter == 90 && !question_marks){
                        counter = counter + 7;
                    } else if (counter >= 122){
                        counter = 63;
                        question_marks = 1;
                    } else{
                        if (!question_marks){
                            counter++;
                        }
                    }
                } else if (j == 1){
                    printf(" ");
                } else{
                    unsigned char cell_contents;
                    temp_pos.r = i - 2;
                    temp_pos.c = j - 2;
                    switch(board_get(b, temp_pos)){
                        case(EMPTY):
                            cell_contents = '.';
                            break;
                        case(BLACK):
                            cell_contents = '*';
                            break;
                        case(WHITE):
                            cell_contents = 'o';
                            break;
                    }
                    printf("%c", cell_contents);
                }
            }
        }
        printf("\n");
    }
}