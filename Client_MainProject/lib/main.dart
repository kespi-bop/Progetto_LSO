import 'package:flutter/material.dart';
import 'dart:io';

void main() {
  runApp(const MainApp());
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      home: HomePage(),
    );
  }
}

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

// Simulate loading and then navigate to the catalog page
class LoadingPage extends StatelessWidget {
  const LoadingPage({super.key});

  @override
  Widget build(BuildContext context) {
    // Simulate a network delay
    Future.delayed(const Duration(seconds: 1), () {
      // When loading is complete do nothing
    });

    return Scaffold(
      appBar: AppBar(title: const Text('Loading')),
      body: const Center(
        child: CircularProgressIndicator(),
      ),
    );
  }
}

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
      Socket socket = await Socket.connect(serverAddress, serverPort);
      while (!entrato) {
        socket.write('cliente:${widget.id}:ingresso');
        socket.listen((List<int> event) {
          String response = String.fromCharCodes(event);

          print(response);
          if (response.contains("ID_cliente")) {
            String data = response.substring(
                response.indexOf("ID_cliente:") + "ID_cliente:".length);
            String posizione = data.split(':')[1];
            print('il cliente ' + widget.id + ' è alla posizione ' + posizione);
            if (posizione == '0' || posizione == "0\n") {
              print('il cliente' + widget.id + 'è il prossimo a entrare');
              socket.close();
              entrato = true;
              entraNelSupermercato();
            }
          }
        });
        sleep(Duration(seconds: 5));
      }
    } catch (e) {
      print('NON POSSO ENTRARE');
      print('Error: $e');
    }
  }

  Future<void> entraNelSupermercato() async {
    try {
      // Connessione al server
      Socket socket = await Socket.connect(serverAddress, serverPort);
/*       print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}'); */
      // Il cliente si mette in coda
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
      socket.close();
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
      Socket socket = await Socket.connect(serverAddress, serverPort);
/*       print(
          'Connected to: ${socket.remoteAddress.address}:${socket.remotePort}'); */
      // Il cliente si mette in coda
      socket.write('catalogo');
      // Ricevi una risposta dal server
      socket.listen((List<int> event) {
        String response = String.fromCharCodes(event);
        /* print(response); */
      });
      socket.close();
    } catch (e) {
      print('Error: $e');
    }
  }

  final List<Item> _items = List.generate(
    10,
    (index) => Item(name: 'Item $index', price: (index + 1) * 10.0),
  );
  final Map<Item, int> _cart = {};

  void _toggleItem(Item item, bool adding) {
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
        .map((entry) => entry.key.price * entry.value)
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
                        title: Text(item.name),
                        subtitle: Text('\$${item.price.toStringAsFixed(2)}'),
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

class Item {
  final String name;
  final double price;

  Item({required this.name, required this.price});
}

// The checkout page with a thank-you message
class CheckoutPage extends StatelessWidget {
  const CheckoutPage({super.key});

  @override
  Widget build(BuildContext context) {
    // Simulate a network delay
    Future.delayed(const Duration(seconds: 2), () {
      // When loading is complete, navigate back to the home page
      Navigator.popUntil(context, ModalRoute.withName('/'));
    });

    return Scaffold(
      appBar: AppBar(title: const Text('Checkout')),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Text(
                'Thanks for buying in our supermarket! Have a great day!',
                style: TextStyle(fontSize: 18)),
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
