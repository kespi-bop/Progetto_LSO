import 'package:flutter/material.dart';

// Simulate loading and then navigate to the catalog page
class LoadingPage extends StatelessWidget {
  final String s;
  const LoadingPage({super.key, required this.s});

  @override
  Widget build(BuildContext context) {
    // Simulate a network delay
    Future.delayed(const Duration(seconds: 10), () {
      // When loading is complete do nothing
    });

    return Scaffold(
      appBar: AppBar(
          automaticallyImplyLeading: false, title: const Text('Loading')),
      body: Column(
        children: [
          Center(
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: Text(s),
            ),
          ),
          const Center(
            child: CircularProgressIndicator(),
          ),
        ],
      ),
    );
  }
}
