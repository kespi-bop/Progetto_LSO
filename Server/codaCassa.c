#include "codaCassa.h"

// Class to manage the queue for the checkout

// Function to add a client to the queue
void add_client_to_checkout_queue(int client_id, checkout_queue_t* checkout_queue){
    pthread_mutex_lock(&mutex_checkout_queue); // Lock mutex before accessing the queue
    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    new_node->client_id = client_id;
    new_node->next = NULL;
    if(checkout_queue->head == NULL){
        checkout_queue->head = new_node;
        checkout_queue->tail = new_node;
    } else {
        checkout_queue->tail->next = new_node;
        checkout_queue->tail = new_node;
    }
    pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
}

// Function to remove a client from the queue
int remove_client_from_cash_queue(checkout_queue_t* checkout_queue){
    pthread_mutex_lock(&mutex_checkout_queue); // Lock mutex before accessing the queue
    if(checkout_queue->head == NULL){
        pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex if the queue is empty
        return -1;
    } else {
        node_t* node_to_remove = checkout_queue->head;
        int client_id = node_to_remove->client_id;
        checkout_queue->head = checkout_queue->head->next;
        free(node_to_remove);
        pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
        return client_id;
    }
}

// Function to remove a client from the queue by id
int remove_client_from_cash_queue_by_id(int client_id, checkout_queue_t* checkout_queue){
    pthread_mutex_lock(&mutex_checkout_queue); // Lock mutex before accessing the queue
    if(checkout_queue->head == NULL){
        pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex if the queue is empty
        return -1;
    } else {
        node_t* current_node = checkout_queue->head;
        node_t* previous_node = NULL;
        while(current_node != NULL){
            if(current_node->client_id == client_id){
                if(previous_node == NULL){
                    checkout_queue->head = current_node->next;
                } else {
                    previous_node->next = current_node->next;
                }
                free(current_node);
                pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
                return client_id;
            }
            previous_node = current_node;
            current_node = current_node->next;
        }
        pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
        return -1;
    }

}

// Function to get the number of clients in the queue
int clients_number_cash_queue(checkout_queue_t* checkout_queue){
    pthread_mutex_lock(&mutex_checkout_queue); // Lock mutex before accessing the queue
    int clients_number = 0;
    node_t* current_node = checkout_queue->head;
    while(current_node != NULL){
        clients_number++;
        current_node = current_node->next;
    }
    pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
    return clients_number;
}

// Function to get the position of a client in the queue by id
int position_client_checkout_queue(int client_id, checkout_queue_t* checkout_queue){
    pthread_mutex_lock(&mutex_checkout_queue); // Lock mutex before accessing the queue
    int position = 0;
    node_t* current_node = checkout_queue->head;
    while(current_node != NULL){
        if(current_node->client_id == client_id){
            pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
            return position;
        }
        position++;
        current_node = current_node->next;
    }
    pthread_mutex_unlock(&mutex_checkout_queue); // Unlock mutex after accessing the queue
    return -1;
}