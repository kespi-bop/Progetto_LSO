#include "codaIngresso.h"

void add_client_to_entrance_queue(int id_cliente, coda_ingresso_t* coda_ingresso){
    pthread_mutex_lock(&mutex_coda_ingresso); // Lock mutex before accessing the queue
    nodoIngresso_t* nuovo_nodoIngresso = (nodoIngresso_t*) malloc(sizeof(nodoIngresso_t));
    nuovo_nodoIngresso->id_cliente = id_cliente;
    nuovo_nodoIngresso->ultima_operazione = time(NULL);
    nuovo_nodoIngresso->next = NULL;
    if(coda_ingresso->head == NULL){
        coda_ingresso->head = nuovo_nodoIngresso;
        coda_ingresso->tail = nuovo_nodoIngresso;
    } else {
        coda_ingresso->tail->next = nuovo_nodoIngresso;
        coda_ingresso->tail = nuovo_nodoIngresso;
    }
    pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
}

int remove_client_entrance_queue(coda_ingresso_t* coda_ingresso){
    pthread_mutex_lock(&mutex_coda_ingresso); // Lock mutex before accessing the queue
    if(coda_ingresso->head == NULL){
        pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex if the queue is empty
        return -1;
    } else {
        nodoIngresso_t* nodoIngresso_da_rimuovere = coda_ingresso->head;
        int id_cliente = nodoIngresso_da_rimuovere->id_cliente;
        coda_ingresso->head = coda_ingresso->head->next;
        free(nodoIngresso_da_rimuovere);
        pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
        return id_cliente;
    }
}

int remove_client_entrance_queue_by_id(int id_cliente, coda_ingresso_t* coda_ingresso){
    pthread_mutex_lock(&mutex_coda_ingresso); // Lock mutex before accessing the queue
    if(coda_ingresso->head == NULL){
        pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex if the queue is empty
        return -1;
    } else {
        nodoIngresso_t* nodoIngresso_corrente = coda_ingresso->head;
        nodoIngresso_t* nodoIngresso_precedente = NULL;
        while(nodoIngresso_corrente != NULL){
            if(nodoIngresso_corrente->id_cliente == id_cliente){
                if(nodoIngresso_precedente == NULL){
                    coda_ingresso->head = nodoIngresso_corrente->next;
                } else {
                    nodoIngresso_precedente->next = nodoIngresso_corrente->next;
                }
                free(nodoIngresso_corrente);
                pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
                return id_cliente;
            }
            nodoIngresso_precedente = nodoIngresso_corrente;
            nodoIngresso_corrente = nodoIngresso_corrente->next;
        }
        pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
        return -1;
    }

}

int clients_number_entrance_queue(coda_ingresso_t* coda_ingresso){
    pthread_mutex_lock(&mutex_coda_ingresso); // Lock mutex before accessing the queue
    nodoIngresso_t* nodoIngresso_corrente = coda_ingresso->head;
    int count = 0;
    while(nodoIngresso_corrente != NULL){
        count++;
        nodoIngresso_corrente = nodoIngresso_corrente->next;
    }
    pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
    return count;
}

int position_client_entrance_queue(int id_cliente, coda_ingresso_t* coda_ingresso){
    pthread_mutex_lock(&mutex_coda_ingresso); // Lock mutex before accessing the queue
    nodoIngresso_t* nodoIngresso_corrente = coda_ingresso->head;
    int count = 0;
    while(nodoIngresso_corrente != NULL){
        if(nodoIngresso_corrente->id_cliente == id_cliente){
            nodoIngresso_corrente->ultima_operazione = time(NULL);
            pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
            return count;
        }
        count++;
        nodoIngresso_corrente = nodoIngresso_corrente->next;
    }
    pthread_mutex_unlock(&mutex_coda_ingresso); // Unlock mutex after accessing the queue
    return -1;
}