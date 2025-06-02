#include <criterion/criterion.h>
#include <stdbool.h>
#include <sys/time.h>
#include "pos.h"
#include "board.h"
#include "logic.h"


bool is_valid_queue(posqueue* q){
    pq_entry* posqueue_index = q->head;
    unsigned int counter = 0;

    if (q->len == 0){
        if (q->head == NULL && q->tail == NULL){
            return true;
        }
    }

    if (q->len == 1){
        if (q->head->next != NULL && q->head->prev != NULL){
            return false;
        }
        if (q->head->p.r != q->tail->p.r || q->head->p.c != q->tail->p.c){
            return false;
        }
        return true;
    } 
    if (q->head == q->tail){
        return false;
    }

    if(q->head->prev != NULL || q->tail->next != NULL){
        return false;
    }

    while (counter < q->len - 1){
        if (counter == 0){
            if (posqueue_index->prev != NULL){
                return false;
            }
            if (posqueue_index->next->prev != posqueue_index){
                return false;
            }
        } else {
            if (posqueue_index->next->prev != posqueue_index){
                return false;
            }
            if (posqueue_index->prev->next != posqueue_index){
                return false;
            }
        }
        posqueue_index = posqueue_index->next;
        counter++;
    }
    if (posqueue_index->next != NULL){
        return false;
    }
    if (posqueue_index->prev->next != posqueue_index){
        return false;
    }

    return true;
}

bool poscmp(pos p1, pos p2){
    if (p1.r == p2.r && p1.c == p2.c){
        return true;
    }
    return false;
}

/*pos.c tests - including is_valid_queue*/
Test(make_pos, new_pos_struct){
    pos test_pos = make_pos(10, 12);
    cr_assert(test_pos.r == 10 && test_pos.c == 12);
}

Test(poscmp, poscmp_true){
    pos p1 = make_pos(10, 3);
    pos p2 = make_pos(10, 3);
    cr_assert(poscmp(p1, p2));
}

Test(poscmp, poscmp_false){
    pos p1 = make_pos(3, 22);
    pos p2 = make_pos(10, 3);
    cr_assert(!poscmp(p1, p2));
}

Test(posqueue_free, no_errors){
    posqueue* test_queue = posqueue_new();
    posqueue_free(test_queue);
    cr_assert(1);
}

Test(posqueue_new, new_empty_queue){
    posqueue* test_queue = posqueue_new();
    cr_assert(test_queue->head == NULL && test_queue->tail == NULL);
    cr_assert(test_queue->len == 0);
    posqueue_free(test_queue);
}

Test(is_valid_queue, valid_queue_no_enqueue){
    posqueue* test_queue = posqueue_new();
    pos p1 = make_pos(8, 10);
    pos p2 = make_pos(9, 2);
    pq_entry* entry_1 = (pq_entry*)malloc(sizeof(pq_entry));
    if (entry_1 == NULL){
        fprintf(stderr, "pos_enqueue could not allocate memory for new entry");
        exit(1);
    }
    pq_entry* entry_2 = (pq_entry*)malloc(sizeof(pq_entry));
    if (entry_2 == NULL){
        fprintf(stderr, "pos_enqueue could not allocate memory for new entry");
        exit(1);
    }
    entry_1->p = p1;
    entry_1->prev = NULL;
    entry_2->p = p2;
    entry_2->next = NULL;

    entry_1->next = entry_2;
    entry_2->prev = entry_1;
    test_queue->head = entry_1;
    test_queue->tail = entry_2;
    test_queue->len = 2;
    cr_assert(is_valid_queue(test_queue));
    posqueue_free(test_queue);
}

Test(is_valid_queue, not_valid_queue){
    posqueue* test_queue = posqueue_new();
    pos p1 = make_pos(8, 10);
    pos p2 = make_pos(9, 2);
    pos p3 = make_pos(2, 2);
    pos_enqueue(test_queue, p1);
    pos_enqueue(test_queue, p2);
    pos_enqueue(test_queue, p3);
    test_queue->tail = test_queue->head;
    cr_assert(!is_valid_queue(test_queue));
    posqueue_free(test_queue);
}

Test(is_valid_queue, not_valid_queue2){
    posqueue* test_queue = posqueue_new();
    pos p1 = make_pos(8, 10);
    pos p2 = make_pos(9, 2);
    pos p3 = make_pos(2, 2);
    pos_enqueue(test_queue, p1);
    pos_enqueue(test_queue, p2);
    pos_enqueue(test_queue, p3);
    test_queue->head->next = test_queue->tail;
    cr_assert(!is_valid_queue(test_queue));
    posqueue_free(test_queue);
}

Test(is_valid_queue, not_valid_queue3){
    posqueue* test_queue = posqueue_new();
    pos p1 = make_pos(8, 10);
    pos p2 = make_pos(9, 2);
    pos p3 = make_pos(2, 2);
    pos_enqueue(test_queue, p1);
    pos_enqueue(test_queue, p2);
    pos_enqueue(test_queue, p3);
    test_queue->head->next->prev = test_queue->head->next;
    cr_assert(!is_valid_queue(test_queue));
    posqueue_free(test_queue);
}

Test(pos_enqueue, enqueue_head_only){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 1);
    posqueue_free(test_queue);
}

Test(pos_enqueue, enqueue_2){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(3, 4));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(poscmp(test_queue->tail->p, make_pos(3, 4)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 2);
    posqueue_free(test_queue);
}

Test(pos_enqueue, enqueue_3){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(3, 4));
    pos_enqueue(test_queue, make_pos(7, 7));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(poscmp(test_queue->head->next->p, make_pos(3, 4)));
    cr_assert(poscmp(test_queue->tail->p, make_pos(7, 7)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 3);
    posqueue_free(test_queue);
}

Test(pos_dequeue, one_element_dequeue){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    cr_assert(poscmp(pos_dequeue(test_queue), make_pos(8, 10)));
    cr_assert(test_queue->head == NULL && test_queue->tail == NULL);
    cr_assert(test_queue->len == 0);
    posqueue_free(test_queue);
}

Test(pos_dequeue, two_elements_dequeue){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(9, 2));
    pos_enqueue(test_queue, make_pos(8, 10));
    cr_assert(poscmp(pos_dequeue(test_queue), make_pos(9, 2)));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 1);
    posqueue_free(test_queue);
}

Test(pos_dequeue, three_elements_dequeue){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(9, 2));
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(2, 2));
    cr_assert(poscmp(pos_dequeue(test_queue), make_pos(9, 2)));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 2);
    posqueue_free(test_queue);
}

Test(posqueue_remback, one_element_remback){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    cr_assert(poscmp(posqueue_remback(test_queue), make_pos(8, 10)));
    cr_assert(test_queue->tail == NULL && test_queue->head == NULL);
    cr_assert(test_queue->len == 0);
    posqueue_free(test_queue);
}

Test(posqueue_remback, two_element_remback){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(6, 6));
    cr_assert(poscmp(posqueue_remback(test_queue), make_pos(6, 6)));
    cr_assert(poscmp(test_queue->tail->p, test_queue->head->p));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 1);
    posqueue_free(test_queue);
}

Test(posqueue_remback, three_element_remback){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(6, 6));
    pos_enqueue(test_queue, make_pos(1, 4));
    cr_assert(poscmp(posqueue_remback(test_queue), make_pos(1, 4)));
    cr_assert(poscmp(test_queue->tail->p, make_pos(6, 6)));
    cr_assert(poscmp(test_queue->head->p, make_pos(8, 10)));
    cr_assert(is_valid_queue(test_queue));
    cr_assert(test_queue->len == 2);
    posqueue_free(test_queue);
}

Test(show_queue, show_queue_runs){
    posqueue* test_queue = posqueue_new();
    pos_enqueue(test_queue, make_pos(8, 10));
    pos_enqueue(test_queue, make_pos(6, 6));
    pos_enqueue(test_queue, make_pos(1, 4));
    show_queue(test_queue);
    printf("\n");
    cr_assert(1);
    posqueue_free(test_queue);
}

/*board.c matrix tests*/
Test(board_new, create_new_board_matrix){
    board* test_b = board_new(10, 10, MATRIX);
    cr_assert(test_b->width == 10 && test_b->height == 10 
                && test_b->type == MATRIX);
    board_free(test_b);
}

Test(board_free, no_errors_matrix){
    board* b = board_new(5, 5, MATRIX);
    board_free(b);
    cr_assert(1);
}

Test(board_show, shows_correct_board_matrix){
    board* test_b = board_new(6, 6, MATRIX);
    board_show(test_b);
    cr_assert(1);
    board_free(test_b);
}

Test(board_get, empty_cell_matrix){
    board* test_b = board_new(10, 10, MATRIX);
    cr_assert(board_get(test_b, make_pos(5, 5)) == EMPTY);
    board_free(test_b);
}

Test(board_get, get_without_set_matrix){
    board* test_b = board_new(10, 10, MATRIX);
    test_b->u.matrix[5][5] = BLACK;
    cr_assert(board_get(test_b, make_pos(5, 5)) == BLACK);
    board_free(test_b);
}

Test(board_set, set_white_matrix){
    board* test_b = board_new(10, 10, MATRIX);
    pos set_pos = make_pos(5, 5);
    board_set(test_b, set_pos, WHITE);
    cr_assert(board_get(test_b, set_pos) == WHITE);
    board_free(test_b);
}

Test(board_set, set_black_matrix){
    board* test_b = board_new(10, 10, MATRIX);
    pos set_pos = make_pos(6, 8);
    board_set(test_b, set_pos, BLACK);
    cr_assert(board_get(test_b, set_pos) == BLACK);
    board_free(test_b);
}

/*logic.c tests*/
Test(new_game, create_new_game){
    game* g = new_game(5, 10, 10, MATRIX);
    cr_assert(g != NULL);
    cr_assert(g->b->width == 10 && g->b->height == 10);
    cr_assert(g->run == 5);
    cr_assert(g->black_queue->head == NULL && g->black_queue->tail == NULL);
    cr_assert(g->black_queue->len == 0);
    cr_assert(g->white_queue->head == NULL && g->white_queue->tail == NULL);
    cr_assert(g->white_queue->len == 0);
    cr_assert(g->b->type == MATRIX);
    game_free(g);
}

Test(game_free, no_errors){
    game* g = new_game(3, 5, 5, MATRIX);
    game_free(g);
    cr_assert(1);
}

Test(drop_piece, drop_one_piece){
    game* g = new_game(5, 10, 10, MATRIX);
    drop_piece(g, 2);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(g->white_queue->head == NULL);
    game_free(g);
}

Test(drop_piece, drop_two_pieces){
    game* g = new_game(5, 10, 10, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 8);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(board_get(g->b, make_pos(9, 8)) == WHITE);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(9, 8)));
    game_free(g);
}

Test(drop_piece, drop_three_pieces){
    game* g = new_game(5, 10, 10, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 8);
    drop_piece(g, 2);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(board_get(g->b, make_pos(9, 8)) == WHITE);
    cr_assert(board_get(g->b, make_pos(8, 2)) == BLACK);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(9, 8)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(8, 2)));
    game_free(g);
}

Test(drop_piece, returns_false){
    game* g = new_game(2, 2, 2, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    cr_assert(!drop_piece(g, 0));
    game_free(g);
}

Test(drop_piece, large_board){
    game* g = new_game(5, 100, 100, MATRIX);
    drop_piece(g, 50);
    cr_assert(board_get(g->b, make_pos(99, 50)) == BLACK);
    game_free(g);
}

Test(disarray, disarray_two){
    game* g = new_game(4, 5, 5, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 2);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 2)));
    game_free(g);
}

Test(disarray, disarray_four){
    game* g = new_game(3, 6, 4, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(3, 4)));
    game_free(g);
}

Test(disarray, disarray_five){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 3);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(4, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(5, 4)));
    game_free(g);
}

Test(disarray, disarray_eight){
    game* g = new_game(5, 10, 10, MATRIX);
    drop_piece(g, 8);
    drop_piece(g, 8);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 8);
    drop_piece(g, 8);
    disarray(g);  
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(6, 8)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(6, 2)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(8, 2)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(8, 8)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 8)));
    cr_assert(poscmp(g->white_queue->head->next->p, make_pos(7, 2)));
    cr_assert(poscmp(g->white_queue->tail->prev->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(9, 8)));

    game_free(g);
}

Test(disarray, board_covered_disarray){
    game* g = new_game(2, 2, 2, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(0, 0)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(0, 1)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(1, 0)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(1, 1)));
    game_free(g);
}

Test(disarray, disarray_twice){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 4);
    disarray(g);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(4, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(3, 4)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(4, 4)));
    game_free(g);
}

Test(offset, empty_board_offset){
    game* g = new_game(3, 8, 8, MATRIX);
    cr_assert(!offset(g));
    game_free(g);
}

Test(offset, white_no_pieces_offset){
    game* g = new_game(3, 8, 8, MATRIX);
    drop_piece(g, 5);
    cr_assert(!offset(g));
    cr_assert(g->player == WHITES_TURN);
    game_free(g);
}

Test(offset, offset_two){
    game* g = new_game(3, 8, 8, MATRIX);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(g->black_queue->head == NULL 
                && g->black_queue->tail == NULL);
    cr_assert(g->white_queue->head == NULL 
                && g->white_queue->tail == NULL);
    game_free(g);
}

Test(offset, offset_four){
    game* g = new_game(3, 8, 8, MATRIX);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(6, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 3)));
    game_free(g);
}

Test(offset, offset_twice){
    game* g = new_game(3, 8, 8, MATRIX);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 1);
    drop_piece(g, 2);
    offset(g);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(7, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(6, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 2)));
    game_free(g);
}

Test(offset, offset_with_disarray){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    disarray(g);
    drop_piece(g, 5);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(5, 1)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(4, 3)));
    game_free(g);
}

Test(offset, board_covered_offset){
    game* g = new_game(2, 2, 2, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(1, 1)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(1, 0)));
    game_free(g);
}

Test(offset, gravity_on_entire_column){
    game* g = new_game(2, 5, 5, MATRIX);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    offset(g);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(1, 3)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(3, 4)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(1, 4)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 3)));
    cr_assert(poscmp(g->white_queue->head->next->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->prev->p, make_pos(4, 4)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(2, 4)));
    game_free(g);
}

Test(offset, disarray_after_offset){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    drop_piece(g, 5);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(5, 1)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(4, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 5)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(5, 3)));
    game_free(g);
}

Test(offset, two_pieces_same_column_1){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    offset(g);
    cr_assert(board_get(g->b, make_pos(4, 1)) == EMPTY);
    cr_assert(board_get(g->b, make_pos(3, 1)) == EMPTY);
    cr_assert(board_get(g->b, make_pos(4, 2)) == WHITE);
    cr_assert(board_get(g->b, make_pos(3, 2)) == BLACK);
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 2)));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    game_free(g);
}

Test(game_outcome, draw_outcome){
    game* g = new_game(2, 2, 2, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == DRAW);
    game_free(g);
}

Test(game_outcome, in_progress){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == IN_PROGRESS);
    game_free(g);
}

Test(game_outcome, in_progress_2){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 5);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == IN_PROGRESS);
    game_free(g);
}

Test(game_outcome, white_wins_vertical){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_horizontal){
    game* g = new_game(4, 6, 6, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 5);
    drop_piece(g, 4);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_diagonal_left){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_diagonal_right){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 2);
    drop_piece(g, 4);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, both_win){
    game* g = new_game(4, 6, 6, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 4);
    cr_assert(game_outcome(g) == DRAW);
    game_free(g);
}

Test(game_outcome, black_wins_vertical){
    game* g = new_game(3, 5, 5, MATRIX);
    drop_piece(g, 4);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 2);
    drop_piece(g, 4);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_horizontal){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 5);
    drop_piece(g, 1);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_diagonal_left){
    game* g = new_game(3, 10, 10, MATRIX);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_diagonal_right){
    game* g = new_game(3, 6, 6, MATRIX);
    drop_piece(g, 5);
    drop_piece(g, 0);
    drop_piece(g, 4);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

//project 2 tests
Test(board_new, new_bit_rep){
    board* b = board_new(10, 10, BITS);
    cr_assert(b->width == 10 && b->height == 10 && b->type == BITS);
    board_free(b);
}

Test(board_get, bit_rep_get){
    board* b = board_new(3, 3, BITS);
    cr_assert(board_get(b, make_pos(2, 2)) == EMPTY);
    board_free(b);
}

Test(board_set, bit_rep_set_black){
    board* b = board_new(3, 3, BITS);
    board_set(b, make_pos(2, 2), BLACK);
    cr_assert(board_get(b, make_pos(2, 2)) == BLACK);
    board_free(b);
}

Test(board_set, bit_rep_set_white){
    board* b = board_new(5, 5, BITS);
    board_set(b, make_pos(4, 4), WHITE);
    cr_assert(board_get(b, make_pos(4, 4)) == WHITE);
    board_free(b);
}

Test(board_set, bit_rep_large_board){
    board* b = board_new(20, 20, BITS);
    board_set(b, make_pos(18, 3), WHITE);
    cr_assert(board_get(b, make_pos(18, 3)) == WHITE);
    board_free(b);
}

Test(board_set, uneven_board){
    board* b = board_new(10, 20, BITS);
    board_set(b, make_pos(2, 2), WHITE);
    cr_assert(board_get(b, make_pos(2, 2)) == WHITE);
    board_free(b);
}

Test(board_get, uneven_board){
    board* b = board_new(30, 25, BITS);
    board_set(b, make_pos(20, 20), WHITE);
    cr_assert(board_get(b, make_pos(20, 20)) == WHITE);
    cr_assert(board_get(b, make_pos(0, 0)) == EMPTY);
    board_free(b);
}

Test(board_set, bit_rep_set_multiple){
    board* b = board_new(10, 6, BITS);
    board_set(b, make_pos(2, 9), WHITE);
    cr_assert(board_get(b, make_pos(2, 9)) == WHITE);
    board_set(b, make_pos(2, 2), WHITE);
    cr_assert(board_get(b, make_pos(2, 2)) == WHITE);
    board_set(b, make_pos(4, 6), BLACK);
    cr_assert(board_get(b, make_pos(4, 6)) == BLACK);
    board_set(b, make_pos(2, 9), EMPTY);
    cr_assert(board_get(b, make_pos(2, 9)) == EMPTY);
    board_free(b);
}

Test(board_set, really_large_board_bits){
    board* b = board_new(100, 100, BITS);
    board_set(b, make_pos(95, 88), WHITE);
    cr_assert(board_get(b, make_pos(95, 88)) == WHITE);
    cr_assert(board_get(b, make_pos(94, 87)) == EMPTY);
    board_free(b);
}

Test(board_show, bit_rep_show){
    board* b = board_new(6, 6, BITS);
    board_show(b);
    board_free(b);
}

//old board.c tests with bits instead of matrix
Test(board_new, create_new_board_bits){
    board* test_b = board_new(10, 10, BITS);
    cr_assert(test_b->width == 10 && test_b->height == 10 
                && test_b->type == BITS);
    board_free(test_b);
}

Test(board_free, no_errors_bits){
    board* b = board_new(5, 5, BITS);
    board_free(b);
    cr_assert(1);
}

Test(board_show, shows_correct_board_bits){
    board* test_b = board_new(6, 6, BITS);
    board_show(test_b);
    cr_assert(1);
    board_free(test_b);
}

Test(board_get, empty_cell_bits){
    board* test_b = board_new(10, 10, BITS);
    cr_assert(board_get(test_b, make_pos(5, 5)) == EMPTY);
    board_free(test_b);
}

Test(board_set, set_white_bits){
    board* test_b = board_new(10, 10, BITS);
    pos set_pos = make_pos(5, 5);
    board_set(test_b, set_pos, WHITE);
    cr_assert(board_get(test_b, set_pos) == WHITE);
    board_free(test_b);
}

Test(board_set, set_black_bits){
    board* test_b = board_new(10, 10, BITS);
    pos set_pos = make_pos(6, 8);
    board_set(test_b, set_pos, BLACK);
    cr_assert(board_get(test_b, set_pos) == BLACK);
    board_free(test_b);
}

//logic.c tests with bits
Test(new_game, create_new_game_bits){
    game* g = new_game(5, 10, 10, BITS);
    cr_assert(g != NULL);
    cr_assert(g->b->width == 10 && g->b->height == 10);
    cr_assert(g->run == 5);
    cr_assert(g->black_queue->head == NULL && g->black_queue->tail == NULL);
    cr_assert(g->black_queue->len == 0);
    cr_assert(g->white_queue->head == NULL && g->white_queue->tail == NULL);
    cr_assert(g->white_queue->len == 0);
    cr_assert(g->b->type == BITS);
    game_free(g);
}

Test(game_free, no_errors_bits){
    game* g = new_game(3, 5, 5, BITS);
    game_free(g);
    cr_assert(1);
}

Test(drop_piece, drop_one_piece_bits){
    game* g = new_game(5, 10, 10, BITS);
    drop_piece(g, 2);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(g->white_queue->head == NULL);
    game_free(g);
}

Test(drop_piece, drop_two_pieces_bits){
    game* g = new_game(5, 10, 10, BITS);
    drop_piece(g, 2);
    drop_piece(g, 8);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(board_get(g->b, make_pos(9, 8)) == WHITE);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(9, 8)));
    game_free(g);
}

Test(drop_piece, drop_three_pieces_bits){
    game* g = new_game(5, 10, 10, BITS);
    drop_piece(g, 2);
    drop_piece(g, 8);
    drop_piece(g, 2);
    cr_assert(board_get(g->b, make_pos(9, 2)) == BLACK);
    cr_assert(board_get(g->b, make_pos(9, 8)) == WHITE);
    cr_assert(board_get(g->b, make_pos(8, 2)) == BLACK);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(9, 8)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(8, 2)));
    game_free(g);
}

Test(drop_piece, returns_false_bits){
    game* g = new_game(2, 2, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    cr_assert(!drop_piece(g, 0));
    game_free(g);
}

Test(drop_piece, large_board_bits){
    game* g = new_game(5, 100, 100, BITS);
    drop_piece(g, 50);
    cr_assert(board_get(g->b, make_pos(99, 50)) == BLACK);
    game_free(g);
}

Test(disarray, disarray_two_bits){
    game* g = new_game(4, 5, 5, BITS);
    drop_piece(g, 2);
    drop_piece(g, 2);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 2)));
    game_free(g);
}

Test(disarray, disarray_four_bits){
    game* g = new_game(3, 6, 4, BITS);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(3, 4)));
    game_free(g);
}

Test(disarray, disarray_five_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 3);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(4, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(5, 4)));
    game_free(g);
}

Test(disarray, disarray_eight_bits){
    game* g = new_game(5, 10, 10, BITS);
    drop_piece(g, 8);
    drop_piece(g, 8);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 8);
    drop_piece(g, 8);
    disarray(g);  
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(6, 8)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(6, 2)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(8, 2)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(8, 8)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 8)));
    cr_assert(poscmp(g->white_queue->head->next->p, make_pos(7, 2)));
    cr_assert(poscmp(g->white_queue->tail->prev->p, make_pos(9, 2)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(9, 8)));
    game_free(g);
}

Test(disarray, board_covered_disarray_bits){
    game* g = new_game(2, 2, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(0, 0)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(0, 1)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(1, 0)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(1, 1)));
    game_free(g);
}

Test(disarray, disarray_twice_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 4);
    disarray(g);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(4, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(3, 4)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(4, 4)));
    game_free(g);
}

Test(offset, empty_board_offset_bits){
    game* g = new_game(3, 8, 8, BITS);
    cr_assert(!offset(g));
    game_free(g);
}

Test(offset, white_no_pieces_offset_bits){
    game* g = new_game(3, 8, 8, BITS);
    drop_piece(g, 5);
    cr_assert(!offset(g));
    cr_assert(g->player == WHITES_TURN);
    game_free(g);
}

Test(offset, offset_two_bits){
    game* g = new_game(3, 8, 8, BITS);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(g->black_queue->head == NULL 
                && g->black_queue->tail == NULL);
    cr_assert(g->white_queue->head == NULL 
                && g->white_queue->tail == NULL);
    game_free(g);
}

Test(offset, offset_four_bits){
    game* g = new_game(3, 8, 8, BITS);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(6, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 3)));
    game_free(g);
}

Test(offset, offset_twice_bits){
    game* g = new_game(3, 8, 8, BITS);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 1);
    drop_piece(g, 2);
    offset(g);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(7, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(6, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(7, 2)));
    game_free(g);
}

Test(offset, offset_with_disarray_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    disarray(g);
    drop_piece(g, 5);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(5, 1)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 3)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(4, 3)));
    game_free(g);
}

Test(offset, board_covered_offset_bits){
    game* g = new_game(2, 2, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    offset(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(1, 1)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(1, 0)));
    game_free(g);
}

Test(offset, gravity_on_entire_column_bits){
    game* g = new_game(2, 5, 5, BITS);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    drop_piece(g, 4);
    offset(g);
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(1, 3)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(3, 4)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(1, 4)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 3)));
    cr_assert(poscmp(g->white_queue->head->next->p, make_pos(2, 3)));
    cr_assert(poscmp(g->white_queue->tail->prev->p, make_pos(4, 4)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(2, 4)));
    game_free(g);
}

Test(offset, disarray_after_offset_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 3);
    offset(g);
    drop_piece(g, 5);
    disarray(g);
    cr_assert(is_valid_queue(g->white_queue) && is_valid_queue(g->black_queue));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(5, 2)));
    cr_assert(poscmp(g->black_queue->head->next->p, make_pos(5, 1)));
    cr_assert(poscmp(g->black_queue->tail->prev->p, make_pos(4, 3)));
    cr_assert(poscmp(g->black_queue->tail->p, make_pos(5, 5)));
    cr_assert(poscmp(g->white_queue->head->p, make_pos(3, 3)));
    cr_assert(poscmp(g->white_queue->tail->p, make_pos(5, 3)));
    game_free(g);
}

Test(offset, two_pieces_same_column_1_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 1);
    offset(g);
    cr_assert(board_get(g->b, make_pos(4, 1)) == EMPTY);
    cr_assert(board_get(g->b, make_pos(3, 1)) == EMPTY);
    cr_assert(board_get(g->b, make_pos(4, 2)) == WHITE);
    cr_assert(board_get(g->b, make_pos(3, 2)) == BLACK);
    cr_assert(poscmp(g->white_queue->head->p, make_pos(4, 2)));
    cr_assert(poscmp(g->black_queue->head->p, make_pos(3, 2)));
    game_free(g);
}

Test(game_outcome, draw_outcome_bits){
    game* g = new_game(2, 2, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == DRAW);
    game_free(g);
}

Test(game_outcome, in_progress_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == IN_PROGRESS);
    game_free(g);
}

Test(game_outcome, in_progress_2_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 5);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == IN_PROGRESS);
    game_free(g);
}

Test(game_outcome, white_wins_vertical_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_horizontal_bits){
    game* g = new_game(4, 6, 6, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 5);
    drop_piece(g, 4);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_diagonal_left_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, white_wins_diagonal_right_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 2);
    drop_piece(g, 4);
    drop_piece(g, 3);
    drop_piece(g, 3);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, both_win_bits){
    game* g = new_game(4, 6, 6, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 4);
    cr_assert(game_outcome(g) == DRAW);
    game_free(g);
}

Test(game_outcome, black_wins_vertical_bits){
    game* g = new_game(3, 5, 5, BITS);
    drop_piece(g, 4);
    drop_piece(g, 3);
    drop_piece(g, 4);
    drop_piece(g, 2);
    drop_piece(g, 4);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_horizontal_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 0);
    drop_piece(g, 5);
    drop_piece(g, 1);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 3);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_diagonal_left_bits){
    game* g = new_game(3, 10, 10, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 5);
    drop_piece(g, 2);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, black_wins_diagonal_right_bits){
    game* g = new_game(3, 6, 6, BITS);
    drop_piece(g, 5);
    drop_piece(g, 0);
    drop_piece(g, 4);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 3);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

//disarray optimization check
Test(disarray, disarray_optimization_test_million){
    game* g = new_game(3, 8, 1000000, MATRIX);
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 1000000; j++){
            board_set(g->b, make_pos(j, i), BLACK);
        }
    }

    struct timeval start;
    struct timeval stop;

    gettimeofday(&start, NULL);
    old_disarray(g);
    gettimeofday(&stop, NULL);

    unsigned long int old_t0 = start.tv_sec * 1000000 + start.tv_usec;
    unsigned long int old_t1 = stop.tv_sec * 1000000 + stop.tv_usec;

    gettimeofday(&start, NULL);
    disarray(g);
    gettimeofday(&stop, NULL);

    unsigned long int new_t0 = start.tv_sec * 1000000 + start.tv_usec;
    unsigned long int new_t1 = stop.tv_sec * 1000000 + stop.tv_usec;

    unsigned long int new_proc_time = new_t1-new_t0;
    unsigned long int old_proc_time = old_t1-old_t0;

    cr_assert(new_proc_time < old_proc_time);
    game_free(g);
}