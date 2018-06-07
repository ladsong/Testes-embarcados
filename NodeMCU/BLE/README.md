Bluetooth Low Energy test
=====
Using Do It ESP32
-----

Create a BLE server that, once we receive a connection, will receive a command and turn ON/OFF LED_BUILTIN or send notification.
On the client side, we use a smartphone with the nRF Connect app.

Connect to the server and send a message through the RX Characteristic:
* If you send a message containing 'A', the LED_BUILTIN will turn on.
* Else if you send a message containing 'B', the LED_BUILTIN will turn off.
* Else if you send a message containing 'C', the server sends a notification through the TX Characteristic.