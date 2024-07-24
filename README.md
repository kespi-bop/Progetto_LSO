# Progetto_LSO

## Tecnologie utilizzate

Il progetto consiste nel produrre un applicativo client-server, usando Docker-compose, che simuli la gestione di un supermercato.
Il Server è stato sviluppato (come da requisiti) in linguaggio C, mentre il Client è stato sviluppato in linguaggio Dart, in particolare con l'utilizzo del framework Flutter.
Client e Server sono in locale e comunicano attraverso l'uso di Socket (come da requisiti).

## Come eseguire l’applicativo

Per eseguire l’applicativo è necessario:
- avviare il comando make nella cartella Server per effettuare la compilazione del Server in C,
- avviare il comando make nella cartella Client per effettuare la compilazione del Client in C,
- usare docker-compose up per avviare il server su docker (docker-compose down per fermarlo),
- se si vuole avviare il Client Flutter è necessario avviare prima un dispositivo android,
- se si vuole avviare il Client in C è necessario andare nella cartella Client e lanciare il comando ./client.