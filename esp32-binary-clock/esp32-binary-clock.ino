#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "time.h"

#include "secrets.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 36000;
const int   daylightOffset_sec = 0;

#define NUM_LEDS 16
#define DATA_PIN 2

// Define the data pin for the status LED on the MCU
#define STATUS_PIN 2

// Create a NeoPixel object
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// Mapping of LED indices to the physical layout
const int ledMap[4][4] = {
  {15, 14, 13, 12},
  {8, 9, 10, 11},
  {7, 6, 5, 4},
  {0, 1, 2, 3}
};

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Get current time and display it
  int timeValue = getCurrentTimeAsHHMM();
  if (timeValue != -1) {
    displayBinary(timeValue);

    // Print local time to console for debugging purposes
    Serial.print("Displaying time: ");
    Serial.println(timeValue);
  }
  
  delay(1000); // Update every second
}

int getCurrentTimeAsHHMM() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return -1;
  }
  
  // Extract hours and minutes
  int hours = timeinfo.tm_hour;
  int minutes = timeinfo.tm_min;
  
  // Convert to HHMM format as a single integer
  int timeValue = hours * 100 + minutes;
  
  return timeValue;
}

void displayBinary(int number) {
  for (int col = 0; col < 4; col++) {
    // Extract each digit from the number (rightmost digit first)
    int digit;
    if (col == 0) {
      digit = number % 10;           // Units of minutes
    } else if (col == 1) {
      digit = (number / 10) % 10;    // Tens of minutes
    } else if (col == 2) {
      digit = (number / 100) % 10;   // Units of hours
    } else {
      digit = (number / 1000) % 10;  // Tens of hours
    }

    for (int row = 0; row < 4; row++) {
      // Determine if the LED should be on or off
      int ledIndex = ledMap[row][col];
      bool isSet = (digit >> row) & 1;

      if (isSet) {
        // Set a random color for each LED that's on
        strip.setPixelColor(ledIndex, strip.Color(random(0, 32), random(0, 32), random(0, 32)));
      } else {
        // Turn off the LED if the bit is not set
        strip.setPixelColor(ledIndex, strip.Color(0, 0, 0));
      }
    }
  }
  strip.show(); // Update the LED strip
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}