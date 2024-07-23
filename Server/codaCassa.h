#ifndef CODACASSA_H
#define CODACASSA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "config.h"

// Mutex for the cash queue
extern pthread_mutex_t mutex_checkout_queue;

// Node for the queue
typedef struct nodo {
    int client_id;
    struct nodo* next;
} node_t;

// Structure for the queue
typedef struct {
    node_t* head;
    node_t* tail;
} checkout_queue_t;

void add_client_to_checkout_queue(int, checkout_queue_t*);
int remove_client_from_cash_queue(checkout_queue_t*);
int remove_client_from_cash_queue_by_id(int, checkout_queue_t*);
int clients_number_cash_queue(checkout_queue_t*);
int position_client_checkout_queue(int, checkout_queue_t*);

#endif