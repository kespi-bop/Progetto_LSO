import 'package:flutter/material.dart';
import 'dart:io';

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
