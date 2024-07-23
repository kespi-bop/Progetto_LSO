#include "direttore.h"

void* checkExit(void* ptr){
    carrello_t* carrelli = (carrello_t*)ptr;
    while(1){
        sleep(DIRECTOR_TIMER);
        for(int i=0; i<C_VARIABLE; i++){
            if(carrelli[i].status == CONFERMA) {
                printf("[DIRETTORE] Cart %d has to get out\n", i);
                pthread_mutex_lock(&carrelli[i].mutex);
                carrelli[i].status = PAGATO;
                pthread_mutex_unlock(&carrelli[i].mutex);
                printf("[DIRETTORE] Cart %d can get out\n", i);
            }
        }
    }
}