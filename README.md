# esp32-binary-clock

ESP32-based binary clock using a 4x4 RGB matrix. Super simple project that I wanted on my desk.

Wire 16 WS2812 RGB LEDs to pin 2 of an ESP32 (I'm using an ESP32-S3-Zero from Waveshare).

Create a file called "secrets.h" (or use the included example) in the same folder as the main .ino file, and inside it put your WiFI detals like this:

```
const char* ssid     = "SSID";
const char* password = "PASSWORD";
```

Now flash the sketch to your ESP32 using the Arduino IDE (or equivalent)

![Front of the Mini Binary Clock](https://raw.githubusercontent.com/obsoletenerd/esp32-binary-clock/refs/heads/main/Mini-Binary-Clock-Back.jpg)

![Back of the Mini Binary Clock](https://raw.githubusercontent.com/obsoletenerd/esp32-binary-clock/refs/heads/main/Mini-Binary-Clock-Back.jpg)

![Inside the Mini Binary Clock](https://raw.githubusercontent.com/obsoletenerd/esp32-binary-clock/refs/heads/main/Mini-Binary-Clock-Wiring.jpg)
