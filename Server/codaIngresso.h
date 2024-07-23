#ifndef CODAINGRESSO_H
#define CODAINGRESSO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "config.h"


// Mutex for the entrance queue
extern pthread_mutex_t mutex_coda_ingresso;

// Node for the entrance queue
typedef struct nodoIngresso {
    int id_cliente;
    time_t ultima_operazione;
    struct nodoIngresso* next;
} nodoIngresso_t;

typedef struct {
    nodoIngresso_t* head;
    nodoIngresso_t* tail;
} coda_ingresso_t;

void add_client_to_entrance_queue(int, coda_ingresso_t*);
int remove_client_entrance_queue(coda_ingresso_t*);
int remove_client_entrance_queue_by_id(int, coda_ingresso_t*);
int clients_number_entrance_queue(coda_ingresso_t*);
int position_client_entrance_queue(int, coda_ingresso_t*);


#endif