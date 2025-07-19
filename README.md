# esp32-binary-clock

ESP32-based binary clock using a 4x4 RGB matrix. Super simple project that I wanted on my desk.

Time is displayed in 24-hour time as HHMM, with the following being the binary values in the matrix:

```
H H M M

1 1 1 1
2 2 2 2
4 4 4 4
8 8 8 8
```

So 6:38pm would be displayed as 1838 or:

![Front of the Mini Binary Clock](https://github.com/obsoletenerd/esp32-binary-clock/blob/main/Mini-Binary-Clock-Front.jpg?raw=true)

## Instructions


Wire 16 WS2812 RGB LEDs to pin 2 of an ESP32 (I'm using an ESP32-S3-Zero from Waveshare).

Create a file called "secrets.h" (or use the included example) in the same folder as the main .ino file, and inside it put your WiFI detals like this:

```
const char* ssid     = "SSID";
const char* password = "PASSWORD";
```

Now flash the sketch to your ESP32 using the Arduino IDE (or equivalent)

![Inside the Mini Binary Clock](https://github.com/obsoletenerd/esp32-binary-clock/blob/main/Mini-Binary-Clock-Wiring.jpg?raw=true)

![Back of the Mini Binary Clock](https://github.com/obsoletenerd/esp32-binary-clock/blob/main/Mini-Binary-Clock-Back.jpg?raw=true)
