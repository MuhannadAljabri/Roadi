import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:firebase_messaging/firebase_messaging.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();
  final FirebaseMessaging _firebaseMessaging = FirebaseMessaging.instance;
  final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
      FlutterLocalNotificationsPlugin();

  void getTokenAndHandleMessages() async {
    String? token = await _firebaseMessaging.getToken();
    print("Firebase Messaging Token: $token");
  }

  @override
  void initState() {
    getTokenAndHandleMessages();
    super.initState();
    FirebaseMessaging.instance.requestPermission();
    FirebaseMessaging.onMessage.listen((RemoteMessage message) {
      print('Got a message whilst in the foreground!');
      print("Received message: ${message.notification?.title}, ${message.notification?.body}");
      if (message.notification != null) {
        showDialog(
          context: _scaffoldKey.currentContext!,
          builder: (BuildContext context) {
            return AlertDialog(
              title: Text(message.notification!.title!),
              content: Text(message.notification!.body!),
              actions: <Widget>[
                TextButton(
                  child: const Text("Yes"),
                  onPressed: () {
                    // Handle Yes response
                    Navigator.of(context).pop();
                  },
                ),
                TextButton(
                  child: const Text("No"),
                  onPressed: () {
                    // Handle No response
                    Navigator.of(context).pop();
                  },
                ),
              ],
            );
          },
        );
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        key: _scaffoldKey,
        appBar: AppBar(title: const Text('Firebase Messaging Demo')),
        body: const Center(child: Text('Listening for messages...')),
      ),
    );
  }
}
