#ifndef CLIENTECONTROLLER_H 
#define CLIENTECONTROLLER_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "carrello.h"
#include "codaCassa.h"
#include "config.h"
#include "codaIngresso.h"

void decrease_clients_number();
void increase_clients_number();
int get_clients_number();

void clientEnters(int*, char*, cart_t*, entrance_queue_t*);
void clientExits(int, char*, cart_t*);
void clientParser(char*, char*, cart_t*, checkout_queue_t*, entrance_queue_t*);
void clientAddItem(int, char*, char*, cart_t*);
void clientRemoveItem(int, char*, char*, cart_t*);
void clientPrintCartContent(int, char*, cart_t*);
void clientEntersCheckoutQueue(int, char*, cart_t*, checkout_queue_t*);
void clientPays(int, char*, cart_t*);
void clientEntersInEntranceQueue(int, char*, entrance_queue_t*);
bool canEnter(entrance_queue_t*);

#endif