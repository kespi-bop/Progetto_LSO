#include "cliente.h"

extern pthread_mutex_t mutex_n_clienti;
extern pthread_mutex_t mutex_chiocciola;
extern pthread_mutex_t mutex_carrelli;
int n_clienti = 0;
int numero_chiocciola = 0;
bool allow=true;

int get_clients_number(){
    int n;
    pthread_mutex_lock(&mutex_n_clienti);
    n = n_clienti;
    pthread_mutex_unlock(&mutex_n_clienti);
    return n;
}

void increase_clients_number(){
    pthread_mutex_lock(&mutex_n_clienti);
    n_clienti++;
    pthread_mutex_unlock(&mutex_n_clienti);
}

void decrease_clients_number(){
    pthread_mutex_lock(&mutex_n_clienti);
    n_clienti--;
    pthread_mutex_unlock(&mutex_n_clienti);
}

void clienteParser(char* request, char* response, carrello_t* carrelli, coda_casse_t* coda_casse, coda_ingresso_t* coda_ingresso){
    int id;
    char comando[10];
    char dati[100];
    sscanf(request, "cliente:%d:%s\n:%s", &id, comando, dati);
    if(strcmp(comando, "entra") == 0) clientEnters(&id, response, carrelli, coda_ingresso);
    else if(strcmp(comando, "ingresso") == 0) clientEntersInEntranceQueue(id, response, coda_ingresso);
    else if(carrelli[id].status == LIBERO) { strcpy(response, "Sessione scaduta\n\0"); return; }
    else if(strcmp(comando, "esce") == 0) clientExits(id, response, carrelli);
    else if(strcmp(comando, "aggiungi") == 0) clientAdds(id, request, response, carrelli);
    else if(strcmp(comando, "rimuovi") == 0) clientRemoves(id, request, response, carrelli);
    else if(strcmp(comando, "stampa") == 0) clientPrints(id, response, carrelli);
    else if(strcmp(comando, "coda") == 0) clientEntersCashQueue(id, response, carrelli, coda_casse);
    else if(strcmp(comando, "paga") == 0) clientPays(id, response, carrelli);
    else strcpy(response, "Comando non riconosciuto\n\0");

    if(strcmp(comando, "esce") != 0 && strcmp(comando, "ingresso") != 0 && id >= 0) {
        carrelli[id].ultima_operazione = time(NULL);
    }

    //printf("[TEST-PARSER] Risposta: %s\n", response);
}

void clientEnters(int* id, char* response, carrello_t* carrelli, coda_ingresso_t* coda_ingresso){
    if(!canEnter(coda_ingresso)) {
        sprintf(response, "Non puoi entrare\n");
        //printf("[TEST-ENTRA] Cliente %d non può entrare\n", *id);
    }else{
        printf("[BOUNCER] Cliente %d può entrare\n", *id);
        remove_client_entrance_queue(coda_ingresso);
        //printf("[TEST] Cliente %d rimosso dalla coda di ingresso\n", *id);
        increase_clients_number();
        int i = 0;
        while(i < C_VARIABLE && carrelli[i].status != LIBERO) i++;
        pthread_mutex_lock(&carrelli[i].mutex);
        carrelli[i].status = IN_NEGOZIO;
        carrelli[i].ultima_operazione = time(NULL);
        sprintf(response, "ID_carrello:%d\n", i);
        *id = i;
    }
    //printf("[TEST-ENTRA] esce\n");
}

bool canEnter(coda_ingresso_t* coda_ingresso){
    int Num = get_clients_number();
    int fila = clients_number_entrance_queue(coda_ingresso);
    if (Num==C_VARIABLE) allow=false;
    if (Num<=C_VARIABLE-E_VARIABLE) allow=true;
    if (allow){
        printf("[BOUNCER] You can enter. Inside the supermarket: %d, In the queue: %d\n", Num, fila);
        return true;
    } else {
        printf("[BOUNCER] You can NOT enter. Inside the supermarket: %d, In the queue: %d\n", Num, fila);
        return false;
    }
}

void clientEntersInEntranceQueue(int id, char* response, coda_ingresso_t* coda_ingresso){
    if(id < 0) {
        pthread_mutex_lock(&mutex_chiocciola);
        id = numero_chiocciola; numero_chiocciola++;
        pthread_mutex_unlock(&mutex_chiocciola);
        add_client_to_entrance_queue(id, coda_ingresso);
    }
    int position = position_client_entrance_queue(id, coda_ingresso);
    sprintf(response, "ID_client:%d:%d\n", id, position);
}

void clientExits(int id, char* response, carrello_t* carrelli){
    if ( carrelli[id].status == PAGATO ) {
        clear_cart(&carrelli[id]);
        carrelli[id].status = LIBERO;
        decrease_clients_number();
        strcpy(response, "You got out from the supermarket\n\0");
    } else {
        strcpy(response, "Session timed out\n\0");
    }
    
}

void clientAdds(int id, char* request, char* response, carrello_t* carrelli){
    int id_prodotto;
    char nome_prodotto[50];
    float prezzo_prodotto;
    sscanf(request, "cliente:%d:aggiungi\n:%d:%[^:]:%f", &id, &id_prodotto, nome_prodotto, &prezzo_prodotto);
    prodotto_t prodotto;
    prodotto.id = id_prodotto;
    strcpy(prodotto.nome, nome_prodotto);
    prodotto.prezzo = prezzo_prodotto;
    if(carrelli[id].status == IN_NEGOZIO) {
        add_product(&carrelli[id], prodotto);
        //printf("Prodotto %d aggiunto al carrello %d\n", id_prodotto, id);
        strcpy(response, "ok\n\0");
    } else {
        //printf("Richiesta su carrello %d non in negozio\n", id);
        strcpy(response, "Sessione scaduta\n\0");
    }
}

void clientRemoves(int id, char* request, char* response, carrello_t* carrelli){
    int id_prodotto;
    sscanf(request, "cliente:%d:rimuovi\n:%d", &id, &id_prodotto);
    if(remove_product(&carrelli[id], id_prodotto)) strcpy(response, "ok\n\0");
    else strcpy(response, "Prodotto non trovato\n\0");
}

void clientPrints(int id, char* response, carrello_t* carrelli){
    print_cart(response, &carrelli[id]);
}

void clientEntersCashQueue(int id, char* response, carrello_t* carrelli, coda_casse_t* casse){
    if(carrelli[id].n_prodotti == 0) {
        sprintf(response, "0\n");
        pthread_mutex_unlock(&carrelli[id].mutex);
        return;
    }
    if(carrelli[id].status == IN_NEGOZIO) {
        add_client_to_cash_queue(id, casse);
        carrelli[id].status = IN_CODA;
        //printf("[TEST-CODACASSE] Cliente %d si è messo in coda. Clienti in coda: %d\n", id, numero_clienti_coda(casse));
    }
    int position = position_client_cash_queue(id, casse);
    if(position <= 0 && carrelli[id].status == IN_CODA) {
        pthread_mutex_unlock(&carrelli[id].mutex);
    }
    sprintf(response, "%d\n", position);
}

void clientPays(int id, char* response, carrello_t* carrelli) {
    if(carrelli[id].status == PAGAMENTO || carrelli[id].status == PAGATO) {
        sprintf(response, "ok\n");
        carrelli[id].status = PAGATO;
    } else {
        if (carrelli[id].status == IN_NEGOZIO && carrelli[id].n_prodotti == 0) carrelli[id].status = CONFERMA;
        sprintf(response, "Carrello in elaborazione\n");
    }
}