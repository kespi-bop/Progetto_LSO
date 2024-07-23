import 'package:flutter/material.dart';
import 'package:test_flutter_lso/checkoutPage.dart';
import 'package:test_flutter_lso/loadingPage.dart';
import 'dart:io';

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

      while (!entrato) {
        Socket socket = await Socket.connect(serverAddress, serverPort);
        print('Sono entrato?' + entrato.toString());
        socket.write('cliente:${widget.id}:ingresso');
        socket.listen((List<int> event) async {
          String response = String.fromCharCodes(event);

          print(response);
          if (response.contains("ID_cliente")) {
            String data = response.substring(
                response.indexOf("ID_cliente:") + "ID_cliente:".length);
            String posizione = data.split(':')[1];
            print('il cliente ' + widget.id + ' è alla posizione ' + posizione);
            if (posizione == '0' || posizione == "0\n") {
              print('il cliente' + widget.id + 'è il prossimo a entrare');
              entrato = true;
            }
          }
          print('PossoEntrare ora?' + entrato.toString());
        });
        sleep(Duration(seconds: 5));
        socket.close();
        if (entrato) {
          await entraNelSupermercato();
          break;
        }
      }
    } catch (e) {
      print('NON POSSO ENTRARE');
      print('Error: $e');
    }
  }

  Future<void> entraNelSupermercato() async {
    try {
      // Connessione al server
/*       print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}'); */
      // Il cliente si mette in coda
      Socket socket = await Socket.connect(serverAddress, serverPort);
      print('cliente:${widget.id}:entra');
      socket.write('cliente:${widget.id}:entra');
      // Ricevi una risposta dal server
      socket.listen((List<int> event) {
        String response = String.fromCharCodes(event);
        if (response.contains("ID_carrello")) {
          print(response);
          // Estrai la parte della stringa dopo "ID_cliente:"
          String data = response.substring(
              response.indexOf("ID_carrello:") + "ID_carrello:".length);
          String idCarrello = data.split(':').first;
          print('al cliente è stato assegnato il carrello' + idCarrello);
        }
      });
      await visualizzaCatalogo();
      setState(() {
        imInTheStore = true;
      });
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
      socket.listen((List<int> event) {
        String response = String.fromCharCodes(event);
        print(response);
      });
    } catch (e) {
      print('Error: $e');
    }
  }

  final List<Prodotto> _items = List.generate(
    10,
    (index) =>
        Prodotto(id: index, nome: 'Item $index', prezzo: (index + 1) * 10.0),
  );
  final Map<Prodotto, int> _cart = {};

  void _toggleItem(Prodotto item, bool adding) {
    setState(() {
      if (adding) {
        _cart[item] = (_cart[item] ?? 0) + 1;
      } else {
        if (_cart.containsKey(item)) {
          if (_cart[item]! > 1) {
            _cart[item] = _cart[item]! - 1;
          } else {
            _cart.remove(item);
          }
        }
      }
    });
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
            appBar: AppBar(title: Text('Catalog ${widget.id}')),
            body: Column(
              children: [
                Expanded(
                  child: ListView.builder(
                    itemCount: _items.length,
                    itemBuilder: (context, index) {
                      final item = _items[index];
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
                            builder: (context) => const CheckoutPage()),
                      );
                    },
                    child: const Text('Join the checkout queue'),
                  ),
                ),
              ],
            ),
          )
        : const LoadingPage();
  }
}
