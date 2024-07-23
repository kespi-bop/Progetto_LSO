import 'package:flutter/material.dart';
import 'dart:io';

class StampaCarrelloPage extends StatefulWidget {
  final String idCarrello;
  const StampaCarrelloPage({super.key, required this.idCarrello});

  @override
  _StampaCarrelloPageState createState() => _StampaCarrelloPageState();
}

class _StampaCarrelloPageState extends State<StampaCarrelloPage> {
  final String serverAddress = '192.168.1.182'; // Modifica l'indirizzo IP del tuo server
  final int serverPort = 5050; // La porta su cui il server sta ascoltando
  String carrelloResponse = '';
  bool isLoading = true;

  @override
  void initState() {
    super.initState();
    stampaCarrello();
  }

  Future<void> stampaCarrello() async {
    try {
      // Connessione al server
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print('Connected to: ${socket.remoteAddress.address}:${socket.remotePort}');
      
      // Invia la richiesta di stampa del carrello
      String request = 'cliente:${widget.idCarrello}:stampa';
      socket.write(request);
      
      // Ricevi la risposta dal server
      socket.listen((List<int> event) {
        String response = String.fromCharCodes(event);
        setState(() {
          carrelloResponse = response;
          isLoading = false;
        });
        print(response);
        socket.close();
      });
    } catch (e) {
      print('Error: $e');
      setState(() {
        carrelloResponse = 'Errore nella connessione al server';
        isLoading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Carrello ${widget.idCarrello}')),
      body: isLoading
          ? const Center(child: CircularProgressIndicator())
          : Padding(
              padding: const EdgeInsets.all(16.0),
              child: Text(
                carrelloResponse.isNotEmpty ? carrelloResponse : 'Carrello vuoto',
                style: Theme.of(context).textTheme.bodyMedium,
              ),
            ),
    );
  }
}
