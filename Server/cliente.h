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

void clientEnters(int*, char*, carrello_t*, coda_ingresso_t*);
void clientExits(int, char*, carrello_t*);
void clienteParser(char*, char*, carrello_t*, coda_casse_t*, coda_ingresso_t*);
void clientAdds(int, char*, char*, carrello_t*);
void clientRemoves(int, char*, char*, carrello_t*);
void clientPrints(int, char*, carrello_t*);
void clientEntersCashQueue(int, char*, carrello_t*, coda_casse_t*);
void clientPays(int, char*, carrello_t*);
void clientEntersInEntranceQueue(int, char*, coda_ingresso_t*);
bool canEnter(coda_ingresso_t*);

#endif