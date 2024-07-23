#ifndef CASSIERE_H
#define CASSIERE_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "carrello.h"
#include "codaCassa.h"
#include "config.h"

// Structure for the cashier
typedef struct {
    int id;
    int cashierTime;
    int productProcessingTime;
    cart_t* carts;
    checkout_queue_t* checkout_queue;
} cashier_t;

extern pthread_mutex_t mutex_cashiers;

void cashierEnters(int, int, int, cart_t*, checkout_queue_t*);
void* waitQueue(void*);
void* processCart(void*);

#endif