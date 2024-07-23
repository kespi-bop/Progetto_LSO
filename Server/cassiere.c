#include "cassiere.h"

void cashierEnters(int id, int tempoCassiere, int tempoElaborazioneProdotto, carrello_t* carrelli, coda_casse_t* coda_casse) {
    pthread_t thread_cassiere;
    cassiere_t* cassiere = (cassiere_t*)malloc(sizeof(cassiere_t));
    cassiere->id = id;
    cassiere->tempoCassiere = tempoCassiere;
    cassiere->tempoElaborazioneProdotto = tempoElaborazioneProdotto;
    cassiere->carrelli = carrelli;
    cassiere->coda_casse = coda_casse;
    if(pthread_create(&thread_cassiere, NULL, waitQueue, (void*)cassiere) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
} 

void* waitQueue(void* ptr) {
    cassiere_t* cassiere = (cassiere_t*)ptr;
    coda_casse_t* coda_casse = cassiere->coda_casse;
    while(1) {
        pthread_t thread_elabora_carrello;
        pthread_mutex_lock(&mutex_cassieri);
        if(coda_casse->head != NULL) { 
            printf("[CASHIER %d] There's someone in the queue\n", cassiere->id);
            if(pthread_create(&thread_elabora_carrello, NULL, processCart, (void*)cassiere) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
            pthread_join(thread_elabora_carrello, NULL);
            printf("[CASHIER %d] Cart processed\n", cassiere->id);
        } else {
            pthread_mutex_unlock(&mutex_cassieri);
        }
        sleep(cassiere->tempoCassiere);
    }
    pthread_exit(NULL);
}

void* processCart(void* ptr) {
    cassiere_t* cassiere = (cassiere_t*)ptr;
    carrello_t* carrelli = cassiere->carrelli;
    coda_casse_t* coda_casse = cassiere->coda_casse;
    int id = coda_casse->head->id_cliente;
    remove_client_from_cash_queue_by_id(id, coda_casse);
    pthread_mutex_unlock(&mutex_cassieri);
    pthread_mutex_lock(&carrelli[id].mutex);
    carrelli[id].status = IN_CASSA;
    for(int i = 0; i < carrelli[id].n_prodotti; i++) sleep(cassiere->tempoElaborazioneProdotto);
    sleep(cassiere->tempoCassiere);
    carrelli[id].status = PAGAMENTO;
    pthread_mutex_unlock(&carrelli[id].mutex);
    pthread_exit(NULL);
}

