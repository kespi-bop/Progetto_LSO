import 'package:flutter/material.dart';
import 'dart:io';

import 'package:test_flutter_lso/catalogPage.dart';

// The initial page with a button to enter the supermarket
class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  // Modifica l'indirizzo IP e la porta in base alle tue necessità
  final String serverAddress =
      '192.168.1.137'; // o l'indirizzo IP del tuo server
  final int serverPort = 5050; // la porta su cui il server sta ascoltando
  String id = '-1';
  bool iWannaEnter = false;

  @override
  void initState() {
    super.initState();
    connectToServer();
  }

  Future<void> connectToServer() async {
    try {
      // Connessione al server
      Socket socket = await Socket.connect(serverAddress, serverPort);
/*       print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}'); */
      // Il cliente si mette in coda
      socket.write('cliente:-1:ingresso');
      // Ricevi una risposta dal server
      socket.listen((List<int> event) {
        String response = String.fromCharCodes(event);
        if (response.contains("ID_cliente")) {
          // Estrai la parte della stringa dopo "ID_cliente:"
          String data = response.substring(
              response.indexOf("ID_cliente:") + "ID_cliente:".length);
          id = data.split(':').first;
          print('il cliente' + id + 'si mette in coda');
        }
      });
      socket.close();
    } catch (e) {
      print('Error: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Supermarket App')),
      body: iWannaEnter
          ? CatalogPage(id: id)
          : Center(
              child: ElevatedButton(
                onPressed: () {
                  setState(() {
                    iWannaEnter = true;
                  });
                },
                child: const Text('Enter the supermarket'),
              ),
            ),
    );
  }
}
