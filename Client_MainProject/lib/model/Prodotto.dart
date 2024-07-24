class Prodotto {
  final int id;
  final String name;
  final double price;

  Prodotto({
    required this.id,
    required this.name,
    required this.price,
  });

  //a factory constructor to create an instance from JSON
  factory Prodotto.fromJson(Map<String, dynamic> json) {
    return Prodotto(
      id: json['id'],
      name: json['name'],
      price: json['price'],
    );
  }
}
