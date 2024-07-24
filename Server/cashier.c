#include "cashier.h"

// Function to create a cashier
void cashierEnters(int id, int cashierTime, int productProcessingTime, cart_t* carts, checkout_queue_t* checkout_queue) {
    pthread_t thread_cassiere;
    cashier_t* cashier = (cashier_t*)malloc(sizeof(cashier_t));
    cashier->id = id;
    cashier->cashierTime = cashierTime;
    cashier->productProcessingTime = productProcessingTime;
    cashier->carts = carts;
    cashier->checkout_queue = checkout_queue;
    if(pthread_create(&thread_cassiere, NULL, waitQueue, (void*)cashier) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
} 

// Function to wait for the queue
void* waitQueue(void* ptr) {
    cashier_t* cashier = (cashier_t*)ptr;
    checkout_queue_t* checkout_queue = cashier->checkout_queue;
    while(1) {
        pthread_t thread_process_cart;
        pthread_mutex_lock(&mutex_cashiers);
        if(checkout_queue->head != NULL) { 
            printf("[CASHIER %d] There's someone in the queue\n", cashier->id);
            if(pthread_create(&thread_process_cart, NULL, processCart, (void*)cashier) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
            pthread_join(thread_process_cart, NULL);
            printf("[CASHIER %d] Cart processed\n", cashier->id);
        } else {
            pthread_mutex_unlock(&mutex_cashiers);
        }
        sleep(cashier->cashierTime);
    }
    pthread_exit(NULL);
}

// Function to process the items of the cart
void* processCart(void* ptr) {
    cashier_t* cashier = (cashier_t*)ptr;
    cart_t* carts = cashier->carts;
    checkout_queue_t* checkout_queue = cashier->checkout_queue;
    int id = checkout_queue->head->client_id;
    remove_client_from_cash_queue_by_id(id, checkout_queue);
    pthread_mutex_unlock(&mutex_cashiers);
    pthread_mutex_lock(&carts[id].mutex);
    carts[id].status = CHECKING_OUT;
    for(int i = 0; i < carts[id].products_number; i++) sleep(cashier->productProcessingTime);
    sleep(cashier->cashierTime);
    carts[id].status = PAYMENT;
    pthread_mutex_unlock(&carts[id].mutex);
    pthread_exit(NULL);
}

