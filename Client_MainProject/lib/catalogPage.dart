import 'package:flutter/material.dart';
import 'package:test_flutter_lso/checkoutPage.dart';
import 'dart:io';
import 'dart:convert';

import 'package:test_flutter_lso/model/Prodotto.dart';

// The catalog page with items and a cart
class CatalogPage extends StatefulWidget {
  String id;
  CatalogPage({super.key, required this.id});

  @override
  _CatalogPageState createState() => _CatalogPageState();
}

class _CatalogPageState extends State<CatalogPage> {
  // Modifica l'indirizzo IP e la porta in base alle tue necessità
  final String serverAddress =
      '192.168.1.137'; // o l'indirizzo IP del tuo server
  final int serverPort = 5050; // la porta su cui il server sta ascoltando
  List<Prodotto> _products = [];
  String idCarrello = '-1';
  bool imInTheStore = false;

  @override
  initState() {
    // TODO: implement initState
    super.initState();
    possoEntrareNelSupermercato();
  }

  Future<void> possoEntrareNelSupermercato() async {
    bool entrato = false;
    try {
      // Connessione al server

      while (true) {
        print('Sono entrato?' + entrato.toString());
        Socket socket = await Socket.connect(serverAddress, serverPort);
        print('Socket APERTA');
        socket.write('cliente:${widget.id}:ingresso');
        print("CLIENTE:${widget.id}:INGRESSO");
        await for (var event in socket) {
          String response = String.fromCharCodes(event);
          print(response);

          if (response.contains("ID_cliente")) {
            String data = response.substring(
                response.indexOf("ID_cliente:") + "ID_cliente:".length);
            String posizione =
                data.split(':')[1].trim(); // Trim whitespace and newlines
            print('Il cliente ' + widget.id + ' è alla posizione ' + posizione);

            if (posizione == '0') {
              print('Il cliente ' + widget.id + ' è il prossimo a entrare');
              entrato = true;
            }
          }
        }
        await socket.close();
        if (entrato) {
          await entraNelSupermercato();
          print("STO PER ROMPERER IL CICLO");
          break;
        } else {
          print("SONO ENTRATO NEL ELSE, NON POSSO ENTRARE");
          sleep(Duration(seconds: 2));
        }
      }
      print("SONO ENTRATO PERCHE' IL WHILE E' TERMINATO");
    } catch (e) {
      print('NON POSSO ENTRARE');
      print('Error: $e');
    }
  }

  Future<void> entraNelSupermercato() async {
    bool assignedCart = false;
    try {
      while (!assignedCart) {
        print('ho il carrello assegnato?' + assignedCart.toString());
        Socket socket = await Socket.connect(serverAddress, serverPort);
        print('cliente:(id_carrello)$idCarrello:entra');
        socket.write('cliente:$idCarrello:entra');
        print("CLIENTE:${widget.id}:ENTRA");
        // Ricevi una risposta dal server
        await for (var event in socket) {
          String response = String.fromCharCodes(event);
          if (response.contains("ID_carrello")) {
            print(response);
            // Estrai la parte della stringa dopo "ID_carrello:"
            String idCarrelloResponse = response.substring(
                response.indexOf("ID_carrello:") + "ID_carrello:".length);
            if (idCarrelloResponse != '-1' && idCarrelloResponse != '-1\n') {
              print('al cliente è stato assegnato il carrello' +
                  idCarrelloResponse);
              assignedCart = true;
              setState(() {
                idCarrello = idCarrelloResponse.trim();
              });
            }
          }
          print('ho il carrello assegnato ORA?' + assignedCart.toString());
        }
        await socket.close();
        if (assignedCart) {
          await visualizzaCatalogo();
          setState(() {
            imInTheStore = true;
          });
          break;
        } else {
          sleep(Duration(seconds: 2));
        }
      }
    } catch (e) {
      print('Error: $e');
    }
  }

  Future<void> visualizzaCatalogo() async {
    try {
      // Connessione al server
/*       print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}'); */
      // Il cliente si mette in coda
      Socket socket = await Socket.connect(serverAddress, serverPort);
      socket.write('catalogo');
      // Ricevi una risposta dal server
      await for (var event in socket) {
        String response = String.fromCharCodes(event);
        // Decode the JSON string
        /* print(response); */
        final Map<String, dynamic> decodedJson = jsonDecode(response);

        // Extract the list of products from the decoded JSON
        final List<dynamic> productsJson = decodedJson['prodotti'];

        // Convert the JSON list into a List<Prodotto>
        _products = productsJson.map((productJson) {
          return Prodotto.fromJson(productJson);
        }).toList();
        // Print the products to verify
        _products.forEach((product) {
          print(
              'ID: ${product.id}, Nome: ${product.nome}, Prezzo: ${product.prezzo}');
        });
      }
      await socket.close();
    } catch (e) {
      print('Error: $e');
    }
  }

  final Map<Prodotto, int> _cart = {};

  void _toggleItem(Prodotto item, bool adding) {
    setState(() {
      if (adding) {
        _cart[item] = (_cart[item] ?? 0) + 1;
        aggiungiAlCarrello(item);
      } else {
        if (_cart.containsKey(item)) {
          if (_cart[item]! > 1) {
            rimuoviDalCarrello(item.id);
            _cart[item] = _cart[item]! - 1;
          } else {
            rimuoviDalCarrello(item.id);
            _cart.remove(item);
          }
        }
      }
    });
  }

  Future<void> aggiungiAlCarrello(Prodotto product) async {
    try {
      // Connessione al server
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print("l'id del carrello è: " +
          idCarrello +
          "e il prodotto è: " +
          product.id.toString());
      print(
          "cliente:$idCarrello:aggiungi\n:${product.id}:${product.nome}:${product.prezzo}");
      socket.write(
          "cliente:$idCarrello:aggiungi\n:${product.id}:${product.nome}:${product.prezzo}");
      // Ricevi una risposta dal server
      await for (var event in socket) {
        String response = String.fromCharCodes(event);
        print('ho aggiunto: ' + response);
      }
      socket.close();
    } catch (e) {
      print('Error: $e');
    }
  }

  Future<void> rimuoviDalCarrello(int idProdotto) async {
    try {
      // Connessione al server
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print("l'id del carrello da cui rimuovere è: " + idCarrello);
      print("cliente:$idCarrello:rimuovi\n:$idProdotto");
      socket.write("cliente:$idCarrello:rimuovi\n:$idProdotto");
      // Ricevi una risposta dal server
      await for (var event in socket) {
        String response = String.fromCharCodes(event);
        print('ho rimosso: ' + response);
      }
      socket.close();
    } catch (e) {
      print('Error: $e');
    }
  }

  double get _totalPrice {
    return _cart.entries
        .map((entry) => entry.key.prezzo * entry.value)
        .fold(0.0, (prev, amount) => prev + amount);
  }

  @override
  Widget build(BuildContext context) {
    return imInTheStore
        ? Scaffold(
            body: Column(
              children: [
                Expanded(
                  child: ListView.builder(
                    itemCount: _products.length,
                    itemBuilder: (context, index) {
                      final item = _products[index];
                      final quantity = _cart[item] ?? 0;

                      return ListTile(
                        title: Text(item.nome),
                        subtitle: Text('\$${item.prezzo.toStringAsFixed(2)}'),
                        trailing: Row(
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            IconButton(
                              icon: const Icon(Icons.remove),
                              onPressed: quantity > 0
                                  ? () => _toggleItem(item, false)
                                  : null,
                            ),
                            Text('$quantity'),
                            IconButton(
                              icon: const Icon(Icons.add),
                              onPressed: () => _toggleItem(item, true),
                            ),
                          ],
                        ),
                      );
                    },
                  ),
                ),
                Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Text('Total: \$${_totalPrice.toStringAsFixed(2)}',
                      style: Theme.of(context).textTheme.headlineMedium),
                ),
                Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: ElevatedButton(
                    onPressed: () async {
                      // Navigate to the checkout page after loading
                      await Navigator.push(
                        context,
                        MaterialPageRoute(
                            builder: (context) => CheckoutPage(
                                  idCarrello: idCarrello,
                                )),
                      );
                    },
                    child: const Text('Join the checkout queue'),
                  ),
                ),
              ],
            ),
          )
        : const Center(child: CircularProgressIndicator());
  }
}
