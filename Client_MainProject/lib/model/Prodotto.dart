class Prodotto {
  final int id;
  final String nome;
  final double prezzo;

  Prodotto({
    required this.id,
    required this.nome,
    required this.prezzo,
  });

  //a factory constructor to create an instance from JSON
  factory Prodotto.fromJson(Map<String, dynamic> json) {
    return Prodotto(
      id: json['id'],
      nome: json['nome'],
      prezzo: json['prezzo'],
    );
  }
}
