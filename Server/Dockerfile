# Usa un'immagine di base di Linux
FROM gcc:latest

# Imposta la directory di lavoro nel container
WORKDIR /app

# Copia i file sorgente nel container
COPY main.c checkoutQueue.c cart.c cashier.c entranceQueue.c client.c checkoutQueue.h cart.h cashier.h entranceQueue.h client.h director.h director.c config.h catalog.json /app/

# Compila il programma
RUN gcc -Wall -Wextra -fsanitize=address -pthread -o server main.c checkoutQueue.c cart.c cashier.c entranceQueue.c client.c director.c

# Espone la porta su cui il server è in ascolto (se necessario)
EXPOSE 5050

# Avvia il server
CMD ["./server", "UI"]