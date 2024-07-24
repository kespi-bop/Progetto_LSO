#include "entranceQueue.h"

// Class to manage the queue for the entrance

// Function to add a client to the queue
void add_client_to_entrance_queue(int client_id, entrance_queue_t* entrance_queue){
    pthread_mutex_lock(&mutex_entrance_queue); // Lock mutex before accessing the queue
    entrance_node_t* new_entrance_node = (entrance_node_t*) malloc(sizeof(entrance_node_t));
    new_entrance_node->client_id = client_id;
    new_entrance_node->last_operation = time(NULL);
    new_entrance_node->next = NULL;
    if(entrance_queue->head == NULL){
        entrance_queue->head = new_entrance_node;
        entrance_queue->tail = new_entrance_node;
    } else {
        entrance_queue->tail->next = new_entrance_node;
        entrance_queue->tail = new_entrance_node;
    }
    pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
}

// Function to remove a client from the queue
int remove_client_entrance_queue(entrance_queue_t* entrance_queue){
    pthread_mutex_lock(&mutex_entrance_queue); // Lock mutex before accessing the queue
    if(entrance_queue->head == NULL){
        pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex if the queue is empty
        return -1;
    } else {
        entrance_node_t* entrance_node_to_remove = entrance_queue->head;
        int client_id = entrance_node_to_remove->client_id;
        entrance_queue->head = entrance_queue->head->next;
        free(entrance_node_to_remove);
        pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
        return client_id;
    }
}

// Function to remove a client from the queue by id
int remove_client_entrance_queue_by_id(int client_id, entrance_queue_t* entrance_queue){
    pthread_mutex_lock(&mutex_entrance_queue); // Lock mutex before accessing the queue
    if(entrance_queue->head == NULL){
        pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex if the queue is empty
        return -1;
    } else {
        entrance_node_t* current_entrance_node = entrance_queue->head;
        entrance_node_t* previous_entrance_node = NULL;
        while(current_entrance_node != NULL){
            if(current_entrance_node->client_id == client_id){
                if(previous_entrance_node == NULL){
                    entrance_queue->head = current_entrance_node->next;
                } else {
                    previous_entrance_node->next = current_entrance_node->next;
                }
                free(current_entrance_node);
                pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
                return client_id;
            }
            previous_entrance_node = current_entrance_node;
            current_entrance_node = current_entrance_node->next;
        }
        pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
        return -1;
    }

}

// Function to count the number of clients in the queue
int clients_number_entrance_queue(entrance_queue_t* entrance_queue){
    pthread_mutex_lock(&mutex_entrance_queue); // Lock mutex before accessing the queue
    entrance_node_t* current_entrance_node = entrance_queue->head;
    int count = 0;
    while(current_entrance_node != NULL){
        count++;
        current_entrance_node = current_entrance_node->next;
    }
    pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
    return count;
}

// Function to get the position of a client in the queue
int position_client_entrance_queue(int client_id, entrance_queue_t* entrance_queue){
    pthread_mutex_lock(&mutex_entrance_queue); // Lock mutex before accessing the queue
    entrance_node_t* current_entrance_node = entrance_queue->head;
    int count = 0;
    while(current_entrance_node != NULL){
        if(current_entrance_node->client_id == client_id){
            current_entrance_node->last_operation = time(NULL);
            pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
            return count;
        }
        count++;
        current_entrance_node = current_entrance_node->next;
    }
    pthread_mutex_unlock(&mutex_entrance_queue); // Unlock mutex after accessing the queue
    return -1;
}