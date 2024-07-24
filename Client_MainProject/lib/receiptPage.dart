import 'package:flutter/material.dart';
import 'package:test_flutter_lso/main.dart';
import 'dart:io';

import 'package:test_flutter_lso/model/Prodotto.dart';
import 'package:restart_app/restart_app.dart';

class ReceiptPage extends StatefulWidget {
  final String idCarrello;
  const ReceiptPage({super.key, required this.idCarrello});

  @override
  _ReceiptPageState createState() => _ReceiptPageState();
}

class _ReceiptPageState extends State<ReceiptPage> {
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
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
          automaticallyImplyLeading: false, title: const Text('Receipt')),
      body: Center(
          child: Scaffold(
        body: Column(
          children: [
            Expanded(
              child: ListView.builder(
                itemCount: productList.length,
                itemBuilder: (context, index) {
                  final item = productList[index];
                  return ListTile(
                    title: Text(item.name),
                    subtitle: Text('\$${item.price.toStringAsFixed(2)}'),
                  );
                },
              ),
            ),
            Image.asset(
              'assets/robotgoodbye.png',
              height: 200,
            ),
            if (productList.isNotEmpty)
              Text("Thank you for shopping with us!")
            else
              Text(''),
            Text("Hope to see you soon!"),
            Padding(
                padding: EdgeInsets.all(16.0),
                child: Column(
                  children: [
                    if (productList.isEmpty)
                      const Text('No products in the cart')
                    else
                      const Text(''),
                    ElevatedButton(
                      onPressed: () {
                        Navigator.of(context).pushAndRemoveUntil(
                          MaterialPageRoute(
                              builder: (context) => const MainApp()),
                          (Route<dynamic> route) => false,
                        );
                      },
                      child: const Text('Exit Supermaket'),
                    ),
                  ],
                )),
          ],
        ),
      )),
    );
  }
}
