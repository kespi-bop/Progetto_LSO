#include "cliente.h"

extern pthread_mutex_t mutex_clients_number;
extern pthread_mutex_t mutex_ticket_snail;
extern pthread_mutex_t mutex_carts;
int clients_number = 0;
int ticket_number = 0;
bool allow = true;

int get_clients_number(){
    int n;
    pthread_mutex_lock(&mutex_clients_number);
    n = clients_number;
    pthread_mutex_unlock(&mutex_clients_number);
    return n;
}

void increase_clients_number(){
    pthread_mutex_lock(&mutex_clients_number);
    clients_number++;
    pthread_mutex_unlock(&mutex_clients_number);
}

void decrease_clients_number(){
    pthread_mutex_lock(&mutex_clients_number);
    clients_number--;
    pthread_mutex_unlock(&mutex_clients_number);
}

// Function to parse the requests from the client
void clienteParser(char* request, char* response, cart_t* carts, checkout_queue_t* checkouts_queue, entrance_queue_t* entrance_queue){
    int id;
    char command[10];
    char data[100];
    sscanf(request, "cliente:%d:%s\n:%s", &id, command, data);
    if(strcmp(command, "entra") == 0) clientEnters(&id, response, carts, entrance_queue);
    else if(strcmp(command, "ingresso") == 0) clientEntersInEntranceQueue(id, response, entrance_queue);
    else if(carts[id].status == FREE) { strcpy(response, "Sessione scaduta\n\0"); return; }
    else if(strcmp(command, "esce") == 0) clientExits(id, response, carts);
    else if(strcmp(command, "aggiungi") == 0) clientAdds(id, request, response, carts);
    else if(strcmp(command, "rimuovi") == 0) clientRemoves(id, request, response, carts);
    else if(strcmp(command, "stampa") == 0) clientPrintCartContent(id, response, carts);
    else if(strcmp(command, "coda") == 0) clientEntersCheckoutQueue(id, response, carts, checkouts_queue);
    else if(strcmp(command, "paga") == 0) clientPays(id, response, carts);
    else strcpy(response, "Comando non riconosciuto\n\0");

    if(strcmp(command, "esce") != 0 && strcmp(command, "ingresso") != 0 && id >= 0) {
        carts[id].last_operation = time(NULL);
    }

}

// Function to manage the entrance of the client
void clientEnters(int* id, char* response, cart_t* carts, entrance_queue_t* entrance_queue){
    if(!canEnter(entrance_queue)) {
        sprintf(response, "Non puoi entrare\n");
    }else{
        printf("[BOUNCER] Client %d can enter\n", *id);
        remove_client_entrance_queue(entrance_queue);
        increase_clients_number();
        int i = 0;
        while(i < C_VARIABLE && carts[i].status != FREE) i++;
        pthread_mutex_lock(&carts[i].mutex);
        carts[i].status = INSIDE_SUPERMARKET;
        carts[i].last_operation = time(NULL);
        sprintf(response, "ID_carrello:%d\n", i);
        *id = i;
    }
}

// Function to check if the client can enter the supermarket
bool canEnter(entrance_queue_t* entrance_queue){
    int num = get_clients_number();
    int queue = clients_number_entrance_queue(entrance_queue);
    if (num==C_VARIABLE) allow = false;
    if (num<=C_VARIABLE-E_VARIABLE) allow = true;
    if (allow){
        printf("[BOUNCER] You can enter. Inside the supermarket: %d, In the queue: %d\n", num, queue);
        return true;
    } else {
        printf("[BOUNCER] You can NOT enter. Inside the supermarket: %d, In the queue: %d\n", num, queue);
        return false;
    }
}

// Function to manage the entrance of the client in the entrance queue
void clientEntersInEntranceQueue(int id, char* response, entrance_queue_t* entrance_queue){
    if(id < 0) {
        pthread_mutex_lock(&mutex_ticket_snail);
        id = ticket_number; ticket_number++;
        pthread_mutex_unlock(&mutex_ticket_snail);
        add_client_to_entrance_queue(id, entrance_queue);
    }
    int position = position_client_entrance_queue(id, entrance_queue);
    sprintf(response, "ID_cliente:%d:%d\n", id, position);
}

// Function to manage the exit of the client
void clientExits(int id, char* response, cart_t* carts){
    if ( carts[id].status == PAYED ) {
        clear_cart(&carts[id]);
        carts[id].status = FREE;
        decrease_clients_number();
        strcpy(response, "Sei uscito dal negozio\n\0");
    } else {
        strcpy(response, "Sessione scaduta\n\0");
    }
    
}

// Function to manage the addition of a product to the cart
void clientAdds(int id, char* request, char* response, cart_t* carts){
    int product_id;
    char product_name[50];
    float product_price;
    sscanf(request, "cliente:%d:aggiungi\n:%d:%[^:]:%f", &id, &product_id, product_name, &product_price);
    product_t product;
    product.id = product_id;
    strcpy(product.name, product_name);
    product.price = product_price;
    if(carts[id].status == INSIDE_SUPERMARKET) {
        add_product(&carts[id], product);
        strcpy(response, "ok\n\0");
    } else {
        strcpy(response, "Sessione scaduta\n\0");
    }
}

// Function to manage the removal of a product from the cart
void clientRemoves(int id, char* request, char* response, cart_t* carts){
    int product_id;
    sscanf(request, "cliente:%d:rimuovi\n:%d", &id, &product_id);
    if(remove_product(&carts[id], product_id)) strcpy(response, "ok\n\0");
    else strcpy(response, "Prodotto non trovato\n\0");
}

// Function to manage the print of the cart
void clientPrintCartContent(int id, char* response, cart_t* carts){
    print_cart(response, &carts[id]);
}

// Function to manage the entrance of the client in the checkout queue
void clientEntersCheckoutQueue(int id, char* response, cart_t* carts, checkout_queue_t* casse){
    if(carts[id].products_number == 0) {
        sprintf(response, "0\n");
        pthread_mutex_unlock(&carts[id].mutex);
        return;
    }
    if(carts[id].status == INSIDE_SUPERMARKET) {
        add_client_to_checkout_queue(id, casse);
        carts[id].status = IN_QUEUE;
    }
    int position = position_client_checkout_queue(id, casse);
    if(position <= 0 && carts[id].status == IN_QUEUE) {
        pthread_mutex_unlock(&carts[id].mutex);
    }
    sprintf(response, "%d\n", position);
}

// Function to manage the payment of the client
void clientPays(int id, char* response, cart_t* carts) {
    if(carts[id].status == PAYMENT || carts[id].status == PAYED) {
        sprintf(response, "ok\n");
        carts[id].status = PAYED;
    } else {
        if (carts[id].status == INSIDE_SUPERMARKET && carts[id].products_number == 0) carts[id].status = CONFIRM;
        sprintf(response, "Carrello in elaborazione\n");
    }
}