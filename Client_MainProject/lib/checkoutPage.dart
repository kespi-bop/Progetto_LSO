import 'package:flutter/material.dart';
import 'dart:io';

class CheckoutPage extends StatefulWidget {
  final String idCarrello;
  const CheckoutPage({super.key, required this.idCarrello});

  @override
  _CheckoutPageState createState() => _CheckoutPageState();
}

class _CheckoutPageState extends State<CheckoutPage> {
  final String serverAddress =
      '192.168.1.137'; // Modifica l'indirizzo IP del tuo server
  final int serverPort = 5050; // La porta su cui il server sta ascoltando
  String codaResponse = '';
  bool isLoading = true;

  @override
  void initState() {
    super.initState();
    print("STO PER METTERMI IN CODA CON ID CARRELLO:" + widget.idCarrello);
    mettiInCoda();
  }

  Future<void> mettiInCoda() async {
    try {
      // Connessione al server
      print("STO PER CREARE LA SOCKET");
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print("SOCKET APERTA");
      print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');

      print('il cliente con carrello ' +
          widget.idCarrello +
          ' si mette in coda alla cassa');
      // Invia la richiesta di mettersi in coda alla cassa
      String request = 'cliente:${widget.idCarrello}:coda';
      socket.write(request);

      // Ricevi la risposta dal server
      await for (var event in socket) {
        String response = String.fromCharCodes(event);
        setState(() {
          codaResponse = response;
          isLoading = false;
        });
        print(response);
        socket.close();
      }
    } catch (e) {
      print('Error: $e');
      setState(() {
        codaResponse = 'Errore nella connessione al server';
        isLoading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Checkout')),
      body: Center(
        child: isLoading
            ? const CircularProgressIndicator()
            : Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Text(
                    codaResponse.isNotEmpty
                        ? codaResponse
                        : 'Grazie per aver comprato nel nostro supermercato! Buona giornata!',
                    style: const TextStyle(fontSize: 18),
                    textAlign: TextAlign.center,
                  ),
                  const SizedBox(height: 20),
                  ElevatedButton(
                    onPressed: () {
                      // Navigate back to the home page
                      Navigator.popUntil(context, ModalRoute.withName('/'));
                    },
                    child: const Text('Exit from the supermarket'),
                  ),
                ],
              ),
      ),
    );
  }
}


// // ******** CONSIGLI ********

// // Per navigare verso questa nuova pagina da un'altra parte della tua app (ad esempio, dalla CatalogPage), puoi usare il seguente codice:

// Navigator.push(
//   context,
//   MaterialPageRoute(
//     builder: (context) => CheckoutPage(idCarrello: '12345'), // Sostituisci '12345' con l'ID del carrello appropriato
//   ),
// );


// // Se vuoi aggiungere un pulsante nella CatalogPage che permette di navigare verso la CheckoutPage, puoi farlo in questo modo:

// Padding(
//   padding: const EdgeInsets.all(16.0),
//   child: ElevatedButton(
//     onPressed: () {
//       Navigator.push(
//         context,
//         MaterialPageRoute(
//           builder: (context) => CheckoutPage(idCarrello: idCarrello), // Passa l'ID del carrello appropriato
//         ),
//       );
//     },
//     child: const Text('Join the checkout queue'),
//   ),
// )

// // Assicurati di avere l'ID del carrello disponibile in modo che possa essere passato alla nuova pagina.

