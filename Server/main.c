#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/socket.h>

#include "carrello.h"
#include "cliente.h"
#include "codaCassa.h"
#include "config.h"
#include "cassiere.h"
#include "codaIngresso.h"
#include "direttore.h"

carrello_t carrelli[C_VARIABLE];
coda_casse_t coda_casse;
coda_ingresso_t coda_ingresso;
pthread_mutex_t mutex_coda_ingresso;
pthread_mutex_t mutex_coda_casse;
pthread_mutex_t mutex_n_clienti;
pthread_mutex_t mutex_chiocciola;
pthread_mutex_t mutex_carrelli;
pthread_mutex_t mutex_cassieri;

typedef struct {
    int socket;
    struct sockaddr_in address;
} connection_t;

void* process(void * ptr);
void send_response(int socket, char * response);
void read_request(int socket, char * request);
void sendCatalog(char* response);
void* reorderCarts();
void* ui();
void* bouncerAtEntrance();
void print_stickman(int num_stickman);

pthread_mutex_t mutex_cassieri = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv) {
    printf("[SERVER] Charts initialization\n");
    initialize_carts(carrelli);
    printf("[SERVER] Director is entering\n");
    pthread_t thread_direttore;
    if(pthread_create(&thread_direttore, NULL, checkExit, carrelli) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
    printf("[SERVER] Cashiers are entering\n");
    for(int i=0; i<CASHES_NUMBER; i++) cashierEnters(i, CASHIER_TIME+i, PRODUCT_TIME, carrelli, &coda_casse);
    int server_socket = 0;
    int client_socket = 0;
    struct sockaddr_in server_address;
    pthread_t thread_client;
    pthread_t thread_pulisci_carrelli;
    if(pthread_create(&thread_pulisci_carrelli, NULL, reorderCarts, NULL) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
    printf("[SERVER] Thread to clean up carts created\n");
    pthread_t thread_buttafuori;
    if(pthread_create(&thread_buttafuori, NULL, bouncerAtEntrance, NULL) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
    pthread_t thread_ui;
    printf("[SERVER] Thread for the bouncer created\n");
    if(argc == 2 && strcmp(argv[1], "UI") == 0){
        if(pthread_create(&thread_ui, NULL, ui, NULL) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
        printf("[SERVER] Thread for the UT created\n");
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror("Could not create socket"), exit(EXIT_FAILURE);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Binding the socket to the address and port
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) perror("Could not bind socket"), exit(EXIT_FAILURE);
    // Listening for incoming connections
    if (listen(server_socket, MAX_CONNECTIONS) < 0) perror("Could not listen on socket"), exit(EXIT_FAILURE);
    printf("[SERVER] Server initialized\n[SERVER] Waiting for connections...\n");
    while (1) {
        client_socket = accept(server_socket, NULL, NULL);

        // If the connection fails, print an error message and exit
        if (client_socket < 0) {
            perror("Could not establish new connection");
            exit(EXIT_FAILURE);
        }

        else{
            printf("[SERVER] New connection\n");
        }

        // Dinamic allocation to pass the socket descriptor to the thread
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        // Create a new thread to handle the new connection
        if (pthread_create(&thread_client, NULL, process, (void *)client_socket_ptr) < 0) perror("Could not create thread"), exit(EXIT_FAILURE);
    }

    return 0;
}

void* process(void * ptr) {
    int socket = *((int *) ptr);
    free(ptr); // Free the memory allocated for the socket descriptor

    char request[MAX_REQUEST_SIZE];
    char response[MAX_RESPONSE_SIZE];

    memset(request, 0, MAX_REQUEST_SIZE);
    memset(response, 0, MAX_RESPONSE_SIZE);

    // Reading the request from the client
    read_request(socket, request);

    // Processing the request
    if(strstr(request, "cliente") != NULL) {
        //printf("[CLIENT] Request: %s\n", request);
        clienteParser(request, response, carrelli, &coda_casse, &coda_ingresso);
        //printf("[SERVER] Response sent: %s", response);
    } else if(strstr(request, "cassiere") != NULL) {
        //printf("[CASHIER] Request: %s\n", request);
        strcpy(response, "cassiere ok");
        //printf("[SERVER] Response sent: %s", response);
    }
    else if(strstr(request, "catalogo") != NULL) {
        //printf("[ANONYMOUS] Request: %s\n", request);
        sendCatalog(response);
        //printf("[SERVER] Response sent: catalogo\n");
    }
    else printf("Invalid request: %s\n", request); 
    
    // Sending response to the client
    send_response(socket, response);

    close(socket);
    return NULL;
}

void send_response(int socket, char* response) {
    //printf("[TEST-SEND] Sending response: %s\n", response);
    if(send(socket, response, strlen(response), 0) == -1) perror("Send"), exit(1);
    //printf("[TEST-SEND] Response sent\n");
}

void read_request(int socket, char* request) {
    if(recv(socket, request, MAX_REQUEST_SIZE, 0) == -1) perror("Recv"), exit(1);
}

void sendCatalog(char* response) {
    FILE* catalogo = fopen("catalogo.json", "r");
    if(catalogo == NULL) perror("Error in opening the catalog"), exit(1);

    char buffer[MAX_RESPONSE_SIZE];
    while(fgets(buffer, MAX_RESPONSE_SIZE, catalogo) != NULL) {
        strcat(response, buffer);
    }
    
    fclose(catalogo);
    
}

void* reorderCarts() {
    while(ACTIVE_EMPLOYEE) {
        sleep(CARTS_CLEANUP_TIMER); 
        printf("[EMPLOYEE] Carts being cleaned up...\n");
        for(int i = 0; i < C_VARIABLE; i++) {
            if(carrelli[i].status != LIBERO && carrelli[i].ultima_operazione + CARTS_CLEANUP_TIMER < time(NULL) && carrelli[i].status != IN_CASSA) {
                printf("[EMPLOYEE] Cart number %d cleaned up\n", i);
                clear_cart(&carrelli[i]);
                if(carrelli[i].status == IN_CODA) remove_client_from_cash_queue_by_id(i, &coda_casse);
                carrelli[i].status = LIBERO;
                decrease_clients_number();
                pthread_mutex_unlock(&carrelli[i].mutex);
            }
        }
    }
    return NULL;
}

void* bouncerAtEntrance(){
    while(ACTIVE_BOUNCER) {
        sleep(BOUNCER_TIMER); 
        printf("[BOUNCER] Checking if everyone is still here\n");
        pthread_mutex_lock(&mutex_coda_ingresso);
        nodoIngresso_t* nodoIngresso_corrente = coda_ingresso.head;
        while(nodoIngresso_corrente != NULL) {
            if(nodoIngresso_corrente->ultima_operazione + BOUNCER_TIMER < time(NULL)) {
                printf("[BOUNCER] Throwing %d out!\n", nodoIngresso_corrente->id_cliente);
                remove_client_entrance_queue_by_id(nodoIngresso_corrente->id_cliente, &coda_ingresso);
            }
            nodoIngresso_corrente = nodoIngresso_corrente->next;
        }
        pthread_mutex_unlock(&mutex_coda_ingresso);
    }
    return NULL;
}

void* ui(){
    int update = 0;
    while(1){
        printf("\033[H\033[J");
        //system("clear");
        fflush(stdout);
        printf("Update: %d\n", ++update);
        fflush(stdout);
        int p_in_negozio = 0;
        int p_in_cassa = 0;
        int p_in_uscita = 0;
        
        for(int i = 0; i < C_VARIABLE; i++) {
            if (carrelli[i].status == IN_NEGOZIO) p_in_negozio++;
            if (carrelli[i].status == IN_CASSA || carrelli[i].status == PAGAMENTO) p_in_cassa++;
            if (carrelli[i].status == CONFERMA) p_in_uscita++;
        }
        
        printf("\nQUEUE OUTSIDE OF THE SUPERMARKET:\n");
        fflush(stdout);
        int p_in_coda = clients_number_entrance_queue(&coda_ingresso);
        print_stickman(p_in_coda);
        int p_in_coda_cassa = clients_number_cash_queue(&coda_casse);
        printf("\nINSIDE THE SUPERMARKET:\n");
        print_stickman(p_in_negozio);
        printf("\n");
        fflush(stdout);
        printf("QUEUE FOR THE CHECKOUT:\n");
        fflush(stdout);
        print_stickman(p_in_coda_cassa);
        printf("\n");
        fflush(stdout);
        printf("AT THE CHECKOUT:\n");
        print_stickman(p_in_cassa);
        printf("\n");
        fflush(stdout);
        printf("GOT OUT WITHOUT BUYING ANYTHING:\n");
        print_stickman(p_in_uscita);
        printf("\n");
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

void print_stickman(int num_stickman) {
    int i, j;
    
    for (i = 0; i < 5; i++) {
        for (j = 0; j < num_stickman; j++) {
            if (i == 0)
                printf(" O ");
            else if (i == 1)
                printf("/|\\");
            else if (i == 2)
                printf(" | ");
            else if (i == 3)
                printf("/ \\");
            else
                printf("   ");
            
            printf("   "); // space between stickmans
        }
        printf("\n");
    }
    fflush(stdout);
}
