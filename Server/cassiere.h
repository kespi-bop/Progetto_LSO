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
    int tempoCassiere;
    int tempoElaborazioneProdotto;
    carrello_t* carrelli;
    coda_casse_t* coda_casse;
} cassiere_t;

extern pthread_mutex_t mutex_cassieri;

void cashierEnters(int, int, int, carrello_t*, coda_casse_t*);
void* waitQueue(void*);
void* processCart(void*);

#endif