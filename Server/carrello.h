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
} product_t;

// cart_node for the queue
typedef struct cart_node {
    product_t prodotto;
    struct cart_node* next;
} cart_node_t;

typedef enum {
    FREE,
    INSIDE_SUPERMARKET,
    IN_QUEUE,
    CHECKING_OUT,
    PAYMENT,
    PAYED,
    CONFIRM
} cart_status_t;

// Structure for the cart
typedef struct {
    cart_status_t status; // Status of the cart
    time_t last_operation; // Last operation on the cart
    int products_number; // Products in the cart
    cart_node_t* head; // Pointer to the head of the queue
    cart_node_t* tail; // Pointer to the tail of the queue
    pthread_mutex_t mutex; // Mutex for the cart
} cart_t;


void print_cart(char*, cart_t*);
void add_product(cart_t*, product_t);
float calculate_total(cart_t*);
bool remove_product(cart_t*, int);
void initialize_carts(cart_t*);
void clear_cart(cart_t*);

#endif