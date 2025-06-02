#ifndef POS_H
#define POS_H
#include <stdbool.h>

struct pos {
    unsigned int r, c;
};

typedef struct pos pos;


typedef struct pq_entry pq_entry;

struct pq_entry {
    pos p;
    pq_entry *next, *prev;
};


struct posqueue {
    pq_entry *head, *tail;
    unsigned int len;
};

typedef struct posqueue posqueue;

/*creates a new pos value at row r and column c
*does not verify if given r and c values are valid
*returns new pos struct*/
pos make_pos(unsigned int r, unsigned int c);

/*allocates and returns a new, empty queue
*returns a pointer to newly allocated queue*/
posqueue* posqueue_new();

/*adds a new pos to the back of the queue
*updates queue accordingly, returns nothing*/
void pos_enqueue(posqueue* q, pos p);

/*removes and returns pos at the front of the queue
*updates queue accordingly*/
pos pos_dequeue(posqueue* q);

/*removes and returns pos at the back of the queue
*updates queue accordingly*/
pos posqueue_remback(posqueue* q);

/*deallocates given queue
*function is void*/
void posqueue_free(posqueue* q);

/*prints a representation of a given posqueue to terminal*/
void show_queue(posqueue* q);

/*checks a set of conditions to determine if a give queue is valid
*including head and tail validity, and that each element points to the
*correct "next" and "prev" values - returns bool*/
bool is_valid_queue(posqueue* q);

/*checks if two given pos structs are equal in both r and c
*returns true if both p1 and p2 have the same values, false if not*/
bool poscmp(pos p1, pos p2);

#endif /* POS_H */
