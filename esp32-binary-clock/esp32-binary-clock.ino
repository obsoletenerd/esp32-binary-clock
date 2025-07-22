// Mini Binary Clock by ObsoleteNerd (https://www.obsoletenerd.com)
// Project details: https://github.com/obsoletenerd/esp32-binary-clock
// Connects to NTP server to get time, then displays it in binary on a 4x4 matrix

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "time.h"

// WiFi details
#include "secrets.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 36000;
const int   daylightOffset_sec = 0;

// Define the number of LEDs and the data pin for the binary matrix
#define NUM_LEDS 16
#define DATA_PIN 2

// Define the data pin for the status LED on the MCU
#define STATUS_PIN 2

// Create a NeoPixel object
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// Startup sequence colours (animates upwards in lines, bottom to top)
uint32_t startupColour1 = strip.Color(21, 0, 14);     // Bottom row
uint32_t startupColour2 = strip.Color(1, 17, 21);     // 2nd row
uint32_t startupColour3 = strip.Color(3, 27, 18);     // 3rd row
uint32_t startupColour4 = strip.Color(30, 29, 11);    // Top row

// Status indicator colours
uint32_t wifiConnectingColour = strip.Color(2, 32, 32);    // Blue
uint32_t timeSuccessColour = strip.Color(7, 32, 3);        // Green
uint32_t errorColour = strip.Color(41, 5, 11);             // Red

// Binary "on" colours (Swaps between these two as the seconds tick)
uint32_t binaryOnColour = strip.Color(7, 32, 3);     // Darker Green
uint32_t binaryOnColour2 = strip.Color(4, 16, 2);   // Lighter Green

// Binary "off" colour
uint32_t binaryOffColour = strip.Color(2, 8, 8);    // Blue

// System status flag
bool systemReady = false;

// Mapping of LED indices to the physical layout
const int ledMap[4][4] = {
  {0, 4, 8, 12},     // Top row
  {1, 5, 9, 13},     // 2nd row
  {2, 6, 10, 14},    // 3rd row
  {3, 7, 11, 15}     // Bottom row
};


void setup() {
  Serial.begin(115200);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Run startup sequence
  startupSequence();
  
  // WiFi connection with status indication
  bool wifiSuccess = connectToWiFi();
  if (!wifiSuccess) {
    Serial.println("WiFi connection failed - entering error mode");
    errorMode(); // This will loop forever since without WiFi we can't get the time in the first place
    return;
  }
  
  // Get time with status indication
  bool timeSuccess = initializeTime();
  if (!timeSuccess) {
    Serial.println("Time initialization failed - entering error mode");
    errorMode(); // This will loop forever as we couldn't get the time
    return;
  }
  
  // Success! Flash green 3 times
  successIndication();
  
  // Disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  systemReady = true;
  Serial.println("System ready - entering normal operation");
}

void loop() {
  if (systemReady) {
    // Get current time and display it
    int timeValue = getCurrentTimeAsHHMM();
    if (timeValue != -1) {
      displayBinary(timeValue);
      Serial.print("Displaying time: ");
      Serial.println(timeValue);
    }
    
    delay(1000); // Update every second
  } else {
    // Should never reach here, but just in case
    errorMode();
  }
}

void startupSequence() {
  Serial.println("Running startup sequence...");
  
  // Clear all LEDs first
  strip.clear();
  strip.show();
  delay(200);
  
  // Light up each row from bottom to top
  uint32_t colours[4] = {startupColour1, startupColour2, startupColour3, startupColour4};
  
  for (int row = 3; row >= 0; row--) { // Start from bottom row (index 3)
    for (int col = 0; col < 4; col++) {
      int ledIndex = ledMap[row][col];
      strip.setPixelColor(ledIndex, colours[3-row]); // Use appropriate colour
    }
    strip.show();
    delay(500); // Half second delay between rows
  }
  
  // Keep the pattern on for a moment
  delay(1000);
  
  // Clear all LEDs
  strip.clear();
  strip.show();
  delay(200);
  
  Serial.println("Startup sequence complete");
}

bool connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  unsigned long startTime = millis();
  unsigned long lastBlink = 0;
  bool ledsOn = false;
  
  while (WiFi.status() != WL_CONNECTED) {
    // Timeout after 30 seconds
    if (millis() - startTime > 30000) {
      Serial.println("\nWiFi connection timeout");
      return false;
    }
    
    // Blink blue every 500ms
    if (millis() - lastBlink > 500) {
      if (ledsOn) {
        strip.clear();
      } else {
        for (int i = 0; i < NUM_LEDS; i++) {
          strip.setPixelColor(i, wifiConnectingColour);
        }
      }
      strip.show();
      ledsOn = !ledsOn;
      lastBlink = millis();
      Serial.print(".");
    }
    
    delay(50);
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Clear LEDs after successful connection
  strip.clear();
  strip.show();
  
  return true;
}

bool initializeTime() {
  Serial.println("Initializing time...");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Wait for time to be set (with timeout)
  unsigned long startTime = millis();
  struct tm timeinfo;
  
  while (!getLocalTime(&timeinfo)) {
    // Timeout after 10 seconds
    if (millis() - startTime > 10000) {
      Serial.println("Failed to obtain time - timeout");
      return false;
    }
    delay(100);
  }
  
  Serial.println("Time synchronized successfully");
  printLocalTime();
  
  return true;
}

void successIndication() {
  Serial.println("Success! Flashing green 3 times...");
  
  for (int i = 0; i < 3; i++) {
    // Turn all LEDs green
    for (int led = 0; led < NUM_LEDS; led++) {
      strip.setPixelColor(led, timeSuccessColour);
    }
    strip.show();
    delay(300);
    
    // Turn off all LEDs
    strip.clear();
    strip.show();
    delay(300);
  }
}

void errorMode() {
  Serial.println("Entering error mode - flashing red indefinitely");
  
  while (true) {
    // Turn all LEDs red
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, errorColour);
    }
    strip.show();
    delay(500);
    
    // Turn off all LEDs
    strip.clear();
    strip.show();
    delay(500);
  }
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
  // Get current seconds to determine which "on" color to use
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  int seconds = timeinfo.tm_sec;
  
  // Alternate between the two "on" colours based on seconds
  uint32_t currentOnColour = (seconds % 2 == 0) ? binaryOnColour : binaryOnColour2;
  
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
        // Use the alternating "on" colour
        strip.setPixelColor(ledIndex, currentOnColour);
      } else {
        // Use the "off" colour instead of turning the LED completely off
        strip.setPixelColor(ledIndex, binaryOffColour);
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