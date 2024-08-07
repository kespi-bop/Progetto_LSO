#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <semaphore.h>

#define MAX_REQUEST_SIZE 1024
#define MAX_RESPONSE_SIZE 1024

void send_request(int socket, char *request);
void read_response(int socket, char *response);

int getChoice();
void printMenu();

void entrance(char *request, char *response);
void enter(char *request, char *response);
void exitFromStore(char *request, char *response);
void addItem(char *request, char *response);
void removeItem(char *request, char *response);
void printCart(char *request, char *response);
void joinCheckoutQueue(char *request, char *response);
void pay(char *request, char *response);
void printCatalog(char *request, char *response);
void addById(char *request, char *response, int id_prodotto);

int id_cart = -1;
int id_client = -1;

int main()
{
    srand(time(NULL));
    while (1)
    {
        char request[MAX_REQUEST_SIZE];
        char response[MAX_RESPONSE_SIZE];

        system("clear");
        int choice = getChoice();
        switch (choice)
        {
        case 1:
            entrance(request, response);
            break;
        case 2:
            enter(request, response);
            break;
        case 3:
            printCatalog(request, response);
            break;
        case 4:
            addItem(request, response);
            break;
        case 5:
            removeItem(request, response);
            break;
        case 6:
            printCart(request, response);
            break;
        case 7:
            joinCheckoutQueue(request, response);
            break;
        case 8:
            pay(request, response);
            break;
        case 9:
            exitFromStore(request, response);
            exit(0);
        }
        printf("Request: %s\n", request);
        printf("Responser: %s\n", response);
        memset(response, 0, MAX_RESPONSE_SIZE);
        printf("Press enter to continue...\n");
        getchar();
        getchar();
    }
    return 0;
}

int create_socket()
{
    int sockfd;
    struct sockaddr_in server_address;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5050);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection failed");
        exit(1);
    }

    return sockfd;
}

void addById(char *request, char *response, int id_product)
{
    int sockfd = create_socket();
    char product_name[50];
    float product_price;
    switch (id_product)
    {
    case 1:
        strcpy(product_name, "Bread");
        product_price = 1.99;
        break;
    case 2:
        strcpy(product_name, "Juice");
        product_price = 0.99;
        break;
    case 3:
        strcpy(product_name, "Chocolate");
        product_price = 2.49;
        break;
    case 4:
        strcpy(product_name, "Candies");
        product_price = 1.79;
        break;
    case 5:
        strcpy(product_name, "Chicken");
        product_price = 5.99;
        break;
    case 6:
        strcpy(product_name, "Hamburger");
        product_price = 2.29;
        break;
    case 7:
        strcpy(product_name, "Apple");
        product_price = 0.69;
        break;
    case 8:
        strcpy(product_name, "Deodorant");
        product_price = 1.49;
        break;
    case 9:
        strcpy(product_name, "Wine");
        product_price = 3.99;
        break;
    case 10:
        strcpy(product_name, "Water");
        product_price = 0.89;
        break;
    }
    sprintf(request, "client:%d:add\n:%d:%s:%f", id_cart, id_product, product_name, product_price);
    send_request(sockfd, request);
    read_response(sockfd, response);
    printf("%s\n", response);
    close(sockfd);
}

void send_request(int socket, char *request)
{
    if (write(socket, request, strlen(request)) == -1)
        perror("Write"), exit(1);
}

void read_response(int socket, char *response)
{
    memset(response, 0, MAX_RESPONSE_SIZE);
    if (read(socket, response, MAX_RESPONSE_SIZE) == -1)
        perror("Read"), exit(1);
}

int getChoice()
{
    int choice;
    while (1)
    {
        printMenu();
        printf("Insert your choice: ");
        scanf("%d", &choice);
        if (choice >= 1 && choice <= 9)
        {
            return choice;
        }
        else
        {
            system("clear");
            printf("Invalid choice!\n\n");
        }
    }
}

void printMenu()
{
    printf("1. Join the entrance queue\n");
    printf("2. Enter the supermarket\n");
    printf("3. Print catalog\n");
    printf("4. Add a product to cart\n");
    printf("5. Remove a product from cart\n");
    printf("6. Print the cart\n");
    printf("7. Join the checkout queue\n");
    printf("8. Pay\n");
    printf("9. Exit the supermarket\n");
}

void entrance(char *request, char *response)
{
    int position = -1;
    do
    {
        int sockfd = create_socket();
        sprintf(request, "client:%d:entrance\n", id_client);
        send_request(sockfd, request);
        read_response(sockfd, response);
        printf("%s\n", response);
        close(sockfd);
        if (strstr(response, "ID_client") != NULL)
            sscanf(response, "ID_client:%d:%d\n", &id_client, &position);
        sleep(2);
    } while (position > 0);
}

void enter(char *request, char *response)
{
    do
    {
        int sockfd = create_socket();
        sprintf(request, "client:%d:enter\n", id_cart);
        send_request(sockfd, request);
        read_response(sockfd, response);
        printf("%s\n", response);
        if (strstr(response, "ID_cart") != NULL)
            sscanf(response, "ID_cart:%d\n", &id_cart);
        close(sockfd);
        sleep(2);
    } while (id_cart == -1);
}

void exitFromStore(char *request, char *response)
{
    int sockfd = create_socket();
    sprintf(request, "client:%d:exits\n", id_cart);
    send_request(sockfd, request);
    read_response(sockfd, response);
    printf("%s\n", response);
    close(sockfd);
}

void printCatalog(char *request, char *response)
{
    int sockfd = create_socket();
    sprintf(request, "catalog\n");
    send_request(sockfd, request);
    read_response(sockfd, response);
    printf("%s\n", response);
    close(sockfd);
}

void addItem(char *request, char *response)
{
    int id_product;
    printf("1. Bread\n");
    printf("2. Juice\n");
    printf("3. Chocolate\n");
    printf("4. Candies\n");
    printf("5. Chicken\n");
    printf("6. Hamburger\n");
    printf("7. Apple\n");
    printf("8. Deodorant\n");
    printf("9. Wine\n");
    printf("10. Water\n");

    printf("Insert the ID of the product to add: ");
    scanf("%d", &id_product);
    addById(request, response, id_product);
}

void removeItem(char *request, char *response)
{
    int sockfd = create_socket();
    int id_product;
    printf("Insert the ID of the product to remove: ");
    scanf("%d", &id_product);
    sprintf(request, "client:%d:remove\n:%d", id_cart, id_product);
    send_request(sockfd, request);
    read_response(sockfd, response);
    printf("%s\n", response);
    close(sockfd);
}

void printCart(char *request, char *response)
{
    int sockfd = create_socket();
    sprintf(request, "client:%d:prints", id_cart);
    send_request(sockfd, request);
    read_response(sockfd, response);
    printf("%s\n", response);
    close(sockfd);
}

void joinCheckoutQueue(char *request, char *response)
{
    int position = -1;
    do
    {
        int sockfd = create_socket();
        sprintf(request, "client:%d:queue", id_cart);
        send_request(sockfd, request);
        read_response(sockfd, response);
        printf("%s\n", response);
        close(sockfd);
        sleep(2);
    } while (position > 0);
}

void pay(char *request, char *response)
{
    do
    {
        int sockfd = create_socket();
        sprintf(request, "client:%d:pay", id_cart);
        send_request(sockfd, request);
        read_response(sockfd, response);
        printf("%s\n", response);
        close(sockfd);
        sleep(2);
    } while ((strcmp(response, "processing cart\n")) == 0);
}
