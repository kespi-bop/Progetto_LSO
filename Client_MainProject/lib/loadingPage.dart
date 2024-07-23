import 'package:flutter/material.dart';

// Simulate loading and then navigate to the catalog page
class LoadingPage extends StatelessWidget {
  const LoadingPage({super.key});

  @override
  Widget build(BuildContext context) {
    // Simulate a network delay
    Future.delayed(const Duration(seconds: 10), () {
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
