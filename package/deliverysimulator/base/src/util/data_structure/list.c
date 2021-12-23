#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "../errcode.h"

int list_initialize(struct List *l) {
    l->front = 0;
    l->back = 0;
    return 0;
}

int list_push_front(struct List *l, void *data) {
    if (!l) {
        return ERR_INPUT_NULL;
    }
    struct ListNode *added = malloc(sizeof(struct ListNode));
    if (!added) {
        return ERR_OUT_OF_MEM;
    }
    added->prev = 0;
    added->data = data;
    added->next = l->front;
    if (l->front) {
        l->front->prev = added;
    }
    l->front = added;
    if (!l->back) {
        l->back = added;
    }
    return ERR_OK;
}

int list_push_back(struct List *l, void *data) {
    if (!l) {
        return ERR_INPUT_NULL;
    }
    struct ListNode *added = malloc(sizeof(struct ListNode));
    if (!added) {
        return ERR_OUT_OF_MEM;
    }
    added->next = 0;
    added->data = data;
    added->prev = l->back;
    if (l->back) {
        l->back->next = added;
    }
    l->back = added;
    if (!l->front) {
        l->front = added;
    }
    return ERR_OK;
}

int list_pop_front(struct List *l) {
    if (!l) {
        return ERR_INPUT_NULL;
    }
    if (!l->front) {
        return ERR_LIST_POP_EMPTY;
    }
    struct ListNode *temp = l->front;
    l->front = temp->next;
    if (l->front) {
        l->front->prev = 0;
    } else {
        l->back = 0;
    }
    free(temp);
    return ERR_OK;
}

int list_pop_back(struct List *l) {
    if (!l) {
        return ERR_INPUT_NULL;
    }
    if (!l->back) {
        return ERR_LIST_POP_EMPTY;
    }
    struct ListNode *temp = l->back;
    l->back = temp->prev;
    if (l->back) {
        l->back->next = 0;
    } else {
        l->front = 0;
    }
    free(temp);
    return ERR_OK;
}

int list_finalize(struct List *l, char remove_data) {
    if (!l) {
        return ERR_INPUT_NULL;
    }
    for (; l->front; list_pop_front(l)) {
        if (remove_data) {
            free(l->front->data);
        }
    }
    return ERR_OK;
}

int list_print(struct List *l, void (*handler)(void*)) {
    if (!l || !handler) {
        return ERR_INPUT_NULL;
    }
    for (struct ListNode *iter = l->front; iter; iter = iter->next) {
        handler(iter->data);
    }
    return 0;
}
