#ifndef ENTRANCEQUEUE_H
#define ENTRANCEQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "config.h"


// Mutex for the entrance queue
extern pthread_mutex_t mutex_entrance_queue;

// Node for the entrance queue
typedef struct entranceNode {
    int client_id;
    time_t last_operation;
    struct entranceNode* next;
} entrance_node_t;

typedef struct {
    entrance_node_t* head;
    entrance_node_t* tail;
} entrance_queue_t;

void add_client_to_entrance_queue(int, entrance_queue_t*);
int remove_client_entrance_queue(entrance_queue_t*);
int remove_client_entrance_queue_by_id(int, entrance_queue_t*);
int clients_number_entrance_queue(entrance_queue_t*);
int position_client_entrance_queue(int, entrance_queue_t*);


#endif