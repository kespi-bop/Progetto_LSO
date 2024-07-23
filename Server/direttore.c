#include "direttore.h"

// Function to check if a client has the approval of the director to get out in case he doesn't have any items in the cart
void* checkExit(void* ptr){
    cart_t* carts = (cart_t*)ptr;
    while(1){
        sleep(DIRECTOR_TIMER);
        for(int i=0; i<C_VARIABLE; i++){
            if(carts[i].status == CONFIRM) {
                printf("[DIRETTORE] Cart %d has to get out\n", i);
                pthread_mutex_lock(&carts[i].mutex);
                carts[i].status = PAYED;
                pthread_mutex_unlock(&carts[i].mutex);
                printf("[DIRETTORE] Cart %d can get out\n", i);
            }
        }
    }
}