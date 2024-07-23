#include "carrello.h"

void add_product(carrello_t* carrello, prodotto_t prodotto) {
    // Creazione del nuovo nodo_carr
    nodo_t_carr* nuovo_nodo_carr = (nodo_t_carr*)malloc(sizeof(nodo_t_carr));
    nuovo_nodo_carr->prodotto = prodotto;
    nuovo_nodo_carr->next = NULL;

    if (carrello->head == NULL) {
        // La coda è vuota, il nuovo nodo_carr diventa la testa e la coda
        carrello->head = nuovo_nodo_carr;
        carrello->tail = nuovo_nodo_carr;
    } else {
        // Aggiunta del nuovo nodo_carr in coda
        carrello->tail->next = nuovo_nodo_carr;
        carrello->tail = nuovo_nodo_carr;
    }

    carrello->n_prodotti++;
}

bool remove_product(carrello_t* carrello, int id_prodotto) {
    nodo_t_carr* nodo_carr_corrente = carrello->head;
    nodo_t_carr* nodo_carr_precedente = NULL;

    while (nodo_carr_corrente != NULL) {
        if (nodo_carr_corrente->prodotto.id == id_prodotto) {
            // Rimozione del nodo_carr corrente
            if (nodo_carr_precedente == NULL) {
                // Il nodo_carr corrente è la testa
                carrello->head = nodo_carr_corrente->next;
            } else {
                // Il nodo_carr corrente non è la testa
                nodo_carr_precedente->next = nodo_carr_corrente->next;
            }

            // Se il nodo_carr corrente è la coda, aggiornamento della coda
            if (nodo_carr_corrente == carrello->tail) {
                carrello->tail = nodo_carr_precedente;
            }

            // Deallocazione del nodo_carr corrente
            free(nodo_carr_corrente);

            carrello->n_prodotti--;
            return true;
        }

        nodo_carr_precedente = nodo_carr_corrente;
        nodo_carr_corrente = nodo_carr_corrente->next;
    }
    return false;
}

void print_cart(char* stringa, carrello_t* carrello) {
    nodo_t_carr* nodo_carr_corrente = carrello->head;

    while (nodo_carr_corrente != NULL) {
        sprintf(stringa, "%s{%d:%s:%f}\n", stringa, nodo_carr_corrente->prodotto.id, nodo_carr_corrente->prodotto.nome, nodo_carr_corrente->prodotto.prezzo);
        nodo_carr_corrente = nodo_carr_corrente->next;
    }
}


float calculate_total(carrello_t* carrello) {
    int totale = 0;
    nodo_t_carr* nodo_carr_corrente = carrello->head;

    while (nodo_carr_corrente != NULL) {
        totale += nodo_carr_corrente->prodotto.prezzo;
        nodo_carr_corrente = nodo_carr_corrente->next;
    }

    return totale;
}

void initialize_carts(carrello_t* carrelli) {
    for (int i = 0; i < C_VARIABLE; i++) {
        carrelli[i].status = LIBERO;
        carrelli[i].n_prodotti = 0;
        carrelli[i].head = NULL;
        carrelli[i].tail = NULL;
        carrelli[i].ultima_operazione = time(NULL);
    }
}


void clear_cart(carrello_t* carrello) {
    nodo_t_carr* nodo_carr_corrente = carrello->head;
    nodo_t_carr* nodo_carr_successivo = NULL;

    while (nodo_carr_corrente != NULL) {
        // Save the pointer to the next node
        nodo_carr_successivo = nodo_carr_corrente->next;

        // Remove the current node
        remove_product(carrello, nodo_carr_corrente->prodotto.id);

        // Go to the next node
        nodo_carr_corrente = nodo_carr_successivo;
    }
}



