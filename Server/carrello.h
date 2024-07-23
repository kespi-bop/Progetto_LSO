#ifndef CARRELLO_H
#define CARRELLO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "config.h"

// Data structure for the product
typedef struct {
    int id;
    char nome[50];
    float prezzo;
} prodotto_t;

// cart_node for the queue
typedef struct nodo_carr {
    prodotto_t prodotto;
    struct nodo_carr* next;
} nodo_t_carr;

typedef enum {
    LIBERO,
    IN_NEGOZIO,
    IN_CODA,
    IN_CASSA,
    PAGAMENTO,
    PAGATO,
    CONFERMA
} stato_carrello_t;

// Structure for the cart
typedef struct {
    stato_carrello_t status; // Status of the cart
    time_t ultima_operazione; // Last operation on the cart
    int n_prodotti; // Products in the cart
    nodo_t_carr* head; // Pointer to the head of the queue
    nodo_t_carr* tail; // Pointer to the tail of the queue
    pthread_mutex_t mutex; // Mutex for the cart
} carrello_t;


void print_cart(char*, carrello_t*);
void add_product(carrello_t*, prodotto_t);
float calculate_total(carrello_t*);
bool remove_product(carrello_t*, int);
void initialize_carts(carrello_t*);
void clear_cart(carrello_t*);

#endif