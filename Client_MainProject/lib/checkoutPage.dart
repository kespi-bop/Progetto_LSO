import 'package:flutter/material.dart';
import 'package:test_flutter_lso/loadingPage.dart';
import 'package:test_flutter_lso/main.dart';
import 'dart:io';

import 'package:test_flutter_lso/model/Prodotto.dart';
import 'package:restart_app/restart_app.dart';
import 'package:test_flutter_lso/receiptPage.dart';

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
  List<Prodotto> productList = [];
  bool paid = false;
  bool hasNoProducts = false;

  @override
  void initState() {
    super.initState();
    stampaCarrello();
  }

  Future<void> stampaCarrello() async {
    try {
      // Connessione al server
      print("STO PER CREARE LA SOCKET");
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print("SOCKET APERTA");
      print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');

      print('il cliente con carrello ' +
          widget.idCarrello +
          ' prints il suo carrello:\n');
      // Invia la richiesta di mettersi in queue alla cassa
      String request = 'client:${widget.idCarrello}:prints';
      socket.write(request);

      // Ricevi la risposta dal server
      await for (var event in socket) {
        String response = String.fromCharCodes(event);
        print('la risposta è' + response);
        await creaListadiProdotti(response);
        setState(() {});
        socket.close();
      }
    } catch (e) {
      print('Error: $e');
      setState(() {
        codaResponse = 'Errore nella connessione al server';
      });
    }
  }

  Future<void> creaListadiProdotti(String response) async {
    // Pulizia della stringa: rimuovere spazi bianchi
    response = response.trim();

    // Divisione della stringa nei productList individuali
    List<String> prodottiRaw = response.split('\n');

    // Rimuove gli spazi bianchi iniziali e finali per ogni elemento della lista
    prodottiRaw = prodottiRaw.map((p) => p.trim()).toList();

    // Parsing della stringa in oggetti Prodotto
    productList = prodottiRaw.map((prodottoString) {
      // Rimuove le parentesi graffe
      String cleanString = prodottoString.replaceAll(RegExp(r'[{}]'), '');

      // Divide le parti dell'elemento
      List<String> parts = cleanString.split(':');

      // Parsing delle parti
      int id = int.parse(parts[0]);
      String name = parts[1];
      double price = double.parse(parts[2]);

      return Prodotto(id: id, name: name, price: price);
    }).toList();

    // Stampa i risultati
    productList.forEach((prodotto) {
      print(
          'ID: ${prodotto.id}, Nome: ${prodotto.name}, Prezzo: ${prodotto.price}');
    });
  }

  Future<void> mettiInCoda() async {
    bool itsmyturn = false;
    try {
      while (true) {
        // Connessione al server
        if (productList.isNotEmpty) {
          Navigator.push(
            context,
            MaterialPageRoute(
                builder: (context) =>
                    const LoadingPage(s: 'wait for your turn')),
          );
        } else {
          Navigator.push(
            context,
            MaterialPageRoute(
                builder: (context) =>
                    const LoadingPage(s: 'wait for the Director')),
          );
        }
        print('è il mio turno?' + itsmyturn.toString());
        Socket socket = await Socket.connect(serverAddress, serverPort);
        print(
            'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');

        print('il cliente con carrello ' +
            widget.idCarrello +
            ' si mette in queue alla cassa');
        // Invia la richiesta di mettersi in queue alla cassa
        String request = 'client:${widget.idCarrello}:queue';
        socket.write(request);

        // Ricevi la risposta dal server
        await for (var event in socket) {
          String response = String.fromCharCodes(event);
          response = response.trim();
          print('sono in queue alla posizione ' + response);
          if (response == '0' || response == '0\n') {
            print('è il turno del cliente col carrello' + widget.idCarrello);
            print('la risposta è' + response);
            itsmyturn = true;
          }
          print('è il mio turno ORA' + itsmyturn.toString());
        }
        await socket.close();
        if (itsmyturn) {
          await effettuaPagamento();
          break;
        } else {
          sleep(Duration(seconds: 2));
        }
      }
    } catch (e) {
      print('Error: $e');
      setState(() {
        codaResponse = 'Errore nella connessione al server';
      });
    }
  }

  Future<void> effettuaPagamento() async {
    bool canipay = false;
    try {
      while (true) {
        // Connessione al server
        if (productList.isNotEmpty) {
          Navigator.push(
            context,
            MaterialPageRoute(
                builder: (context) =>
                    const LoadingPage(s: 'wait cashier to process your cart')),
          );
        }
        print('Posso pagare?' + canipay.toString());
        Socket socket = await Socket.connect(serverAddress, serverPort);
        print(
            'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');

        print(
            'il cliente con carrello ' + widget.idCarrello + ' prova a pagare');
        // Invia la richiesta di mettersi in queue alla cassa
        String request = 'client:${widget.idCarrello}:pay';
        socket.write(request);

        // Ricevi la risposta dal server
        await for (var event in socket) {
          String response = String.fromCharCodes(event);
          print('il carrello è in elaborazione??' + response);
          if (response != 'processing cart\n') {
            print('il carrello ' + widget.idCarrello + ' è stato elaborato');
            print('la risposta è' + response);
            canipay = true;
          }
          print('posso pagare?' + canipay.toString());
        }
        await socket.close();
        if (canipay) {
          setState(() {
            print("HO PAGATO");
            paid = true;
            print('Sono pronto a stampare il carrello ' + widget.idCarrello);
          });
          if (productList.length == 0) {
            setState(() {
              hasNoProducts = true;
            });
          }
          print("ORA RESTARTO L'app");
          Navigator.push(
            context,
            MaterialPageRoute(
                builder: (context) =>
                    ReceiptPage(idCarrello: widget.idCarrello)),
          );
          break;
        } else {
          sleep(Duration(seconds: 2));
        }
      }
    } catch (e) {
      print('Error: $e');
      setState(() {
        codaResponse = 'Errore nella connessione al server';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Checkout')),
      body: Center(
          child: paid || hasNoProducts
              ? const CircularProgressIndicator()
              : Scaffold(
                  body: Column(
                    children: [
                      Expanded(
                        child: ListView.builder(
                          itemCount: productList.length,
                          itemBuilder: (context, index) {
                            final item = productList[index];
                            return ListTile(
                              title: Text(item.name),
                              subtitle:
                                  Text('\$${item.price.toStringAsFixed(2)}'),
                            );
                          },
                        ),
                      ),
                      if (productList.isEmpty)
                        const Text('No products in the cart')
                      else
                        const Text(''),
                      Padding(
                          padding: EdgeInsets.all(16.0),
                          child: Column(
                            children: [
                              ElevatedButton(
                                onPressed: () async {
                                  print(
                                      "STO PER METTERMI IN CODA CON ID CARRELLO:" +
                                          widget.idCarrello);
                                  await mettiInCoda();
                                },
                                child: const Text('Go to the queue'),
                              ),
                            ],
                          )),
                    ],
                  ),
                )),
    );
  }
}
