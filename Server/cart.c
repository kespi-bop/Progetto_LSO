#include "cart.h"

// Function to add a product to the cart
void add_product(cart_t* cart, product_t product) {
    // Creazione del nuovo cart_node
    cart_node_t* new_cart_node = (cart_node_t*)malloc(sizeof(cart_node_t));
    new_cart_node->product = product;
    new_cart_node->next = NULL;

    if (cart->head == NULL) {
        // La coda è vuota, il nuovo cart_node diventa la testa e la coda
        cart->head = new_cart_node;
        cart->tail = new_cart_node;
    } else {
        // Aggiunta del nuovo cart_node in coda
        cart->tail->next = new_cart_node;
        cart->tail = new_cart_node;
    }

    cart->products_number++;
}

// Function to remove a product from the cart
bool remove_product(cart_t* cart, int product_id) {
    cart_node_t* current_cart_node = cart->head;
    cart_node_t* previous_cart_node = NULL;

    while (current_cart_node != NULL) {
        if (current_cart_node->product.id == product_id) {
            // Rimozione del cart_node corrente
            if (previous_cart_node == NULL) {
                // Il cart_node corrente è la testa
                cart->head = current_cart_node->next;
            } else {
                // Il cart_node corrente non è la testa
                previous_cart_node->next = current_cart_node->next;
            }

            // Se il cart_node corrente è la coda, aggiornamento della coda
            if (current_cart_node == cart->tail) {
                cart->tail = previous_cart_node;
            }

            // Deallocazione del cart_node corrente
            free(current_cart_node);

            cart->products_number--;
            return true;
        }

        previous_cart_node = current_cart_node;
        current_cart_node = current_cart_node->next;
    }
    return false;
}

// Function to print the content of the cart
void print_cart(char* cart_string, cart_t* cart) {
    cart_node_t* current_cart_node = cart->head;

    while (current_cart_node != NULL) {
        sprintf(cart_string, "%s{%d:%s:%f}\n", cart_string, current_cart_node->product.id, current_cart_node->product.name, current_cart_node->product.price);
        current_cart_node = current_cart_node->next;
    }
}

// Function to calculate the total price of the items in the cart
float calculate_total(cart_t* cart) {
    int total = 0;
    cart_node_t* current_cart_node = cart->head;

    while (current_cart_node != NULL) {
        total += current_cart_node->product.price;
        current_cart_node = current_cart_node->next;
    }

    return total;
}

// Function to initialize the carts
void initialize_carts(cart_t* carts) {
    for (int i = 0; i < C_VARIABLE; i++) {
        carts[i].status = FREE;
        carts[i].head = NULL;
        carts[i].tail = NULL;
        carts[i].products_number = 0;
        carts[i].last_operation = time(NULL);
    }
}

// Function to clear the cart
void clear_cart(cart_t* cart) {
    cart_node_t* current_cart_node = cart->head;
    cart_node_t* next_cart_node = NULL;

    while (current_cart_node != NULL) {
        // Save the pointer to the next node
        next_cart_node = current_cart_node->next;

        // Remove the current node
        remove_product(cart, current_cart_node->product.id);

        // Go to the next node
        current_cart_node = next_cart_node;
    }
}



