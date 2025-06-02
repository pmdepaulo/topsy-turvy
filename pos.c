#include <stdio.h>
#include <stdlib.h>
#include "pos.h"

pos make_pos(unsigned int r, unsigned int c){
    pos new_pos = {r, c};
    return new_pos;
}

posqueue* posqueue_new(){
    posqueue* empty_queue = (posqueue*)malloc(sizeof(posqueue));
    if (empty_queue == NULL){
        fprintf(stderr, "posqueue_new could not allocate memory for new queue");
        exit(1);
    }

    empty_queue->head = NULL;
    empty_queue->tail = NULL;
    empty_queue->len = 0;

    return empty_queue;
}

void pos_enqueue(posqueue* q, pos p){
    if (q == NULL){
        fprintf(stderr, "passed in posqueue is NULL, cannot enqueue");
        exit(1);
    }

    pq_entry* new_entry = (pq_entry*)malloc(sizeof(pq_entry));
    if (new_entry == NULL){
        fprintf(stderr, "pos_enqueue could not allocate memory for new entry");
        exit(1);
    }
    new_entry->p = p;
    new_entry->next = NULL;
    
    if (q->head == NULL){
        new_entry->prev = NULL;
        new_entry->next = NULL;
        q->head = new_entry;
        q->tail = new_entry;
    } else{
        new_entry->prev = q->tail;
        q->tail->next = new_entry;
    }
    q->tail = new_entry;
    q->len++;
}

pos pos_dequeue(posqueue* q){
    if (!q->len){
        fprintf(stderr, "q is empty, cannot dequeue first element");
        exit(1);
    }

    pos first_pos = q->head->p;
    pq_entry* first_entry = q->head;
    if (q->len > 1){
        q->head->next->prev = NULL;
        q->head = q->head->next;
    } else{
        q->head = NULL;
        q->tail = NULL;
    }
    q->len--;
    free(first_entry);
    return first_pos;
}

pos posqueue_remback(posqueue* q){
    if (!q->len){
        fprintf(stderr, "q is empty, cannot remove back element");
        exit(1);
    }

    pos last_pos = q->tail->p;
    pq_entry* last_entry = q->tail;
    if (q->len > 1){
        q->tail->prev->next = NULL;
        q->tail = q->tail->prev;
    } else{
        q->head = NULL;
        q->tail = NULL;
    }
    q->len--;
    free(last_entry);
    return last_pos;
}

void posqueue_free(posqueue* q){
    pq_entry* posqueue_index = q->head;
    while (posqueue_index != NULL){
        pq_entry* index_next = posqueue_index->next;
        free(posqueue_index);
        posqueue_index = index_next;
    }
    free(q);
}

void show_queue(posqueue* q){
    printf("\n");
    pq_entry* posqueue_index = q->head;
    unsigned int counter = 0;
    while (counter < q->len - 1 && posqueue_index != NULL){
        printf(" (%d, %d) ->", posqueue_index->p.r, posqueue_index->p.c);
        posqueue_index = posqueue_index->next;
        counter++;
    }
    if (posqueue_index != NULL){
        printf(" (%d, %d) *END*", q->tail->p.r, q->tail->p.c);
    }
}