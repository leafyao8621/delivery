#ifndef __LIST_H__
#define __LIST_H__

struct ListNode {
    void *data;
    struct ListNode *prev;
    struct ListNode *next;
};

struct List {
    struct ListNode *front;
    struct ListNode *back;
};

int list_initialize(struct List *l);
int list_push_front(struct List *l, void *data);
int list_push_back(struct List *l, void *data);
int list_pop_front(struct List *l);
int list_pop_back(struct List *l);
int list_finalize(struct List *l, char remove_data);
int list_print(struct List *l, void (*handler)(void*));

#endif
