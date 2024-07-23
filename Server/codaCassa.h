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
extern pthread_mutex_t mutex_coda_casse;

// Node for the queue
typedef struct nodo {
    int id_cliente;
    struct nodo* next;
} nodo_t;

// Structure for the queue
typedef struct {
    nodo_t* head;
    nodo_t* tail;
} coda_casse_t;

void add_client_to_cash_queue(int, coda_casse_t*);
int remove_client_from_cash_queue(coda_casse_t*);
int remove_client_from_cash_queue_by_id(int, coda_casse_t*);
int clients_number_cash_queue(coda_casse_t*);
int position_client_cash_queue(int, coda_casse_t*);

#endif