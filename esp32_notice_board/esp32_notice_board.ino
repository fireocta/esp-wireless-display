#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Display configuration
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TEXT_SIZE_TITLE 3
#define TEXT_SIZE_TIME 2
#define LINE_HEIGHT 20
#define MARGIN 10

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase credentials
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_DATABASE_URL"

// Initialize objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
TFT_eSPI tft = TFT_eSPI();

String currentNotice = "";
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize display
  tft.init();
  tft.setRotation(0); // Portrait for 240x240
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Show bootup message
  showBootMessage("Connecting to WiFi...");
  
  // Connect WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }
  
  // Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  showBootMessage("Connecting to Firebase...");
  
  if (Firebase.signUp(&config, &auth, "", "")) {
    showBootMessage("Connected!");
    delay(1000);
  } else {
    showBootMessage("Firebase Failed!");
    while(1) delay(1000);
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && (millis() - lastUpdate > 1000 || lastUpdate == 0)) {
    lastUpdate = millis();
    
    if (Firebase.RTDB.getString(&fbdo, "/notices/current/title")) {
      String newNotice = fbdo.stringData();
      
      if (newNotice != currentNotice) {
        currentNotice = newNotice;
        displayNotice(currentNotice);
      }
    }
  }
}

void displayNotice(String notice) {
  tft.fillScreen(TFT_BLACK);
  
  // Display time at top
  tft.setTextSize(TEXT_SIZE_TIME);
  String timeStr = getTimeString();
  int timeWidth = tft.textWidth(timeStr);
  tft.drawString(timeStr, (SCREEN_WIDTH - timeWidth) / 2, MARGIN);
  
  // Display notice text
  tft.setTextSize(TEXT_SIZE_TITLE);
  int yPos = MARGIN + LINE_HEIGHT * 2;
  int maxCharsPerLine = (SCREEN_WIDTH - (MARGIN * 2)) / (6 * TEXT_SIZE_TITLE);
  
  // Word wrap implementation
  String words = notice;
  String line = "";
  
  while (words.length() > 0) {
    int spaceIndex = words.indexOf(' ');
    String word = (spaceIndex == -1) ? words : words.substring(0, spaceIndex);
    
    if (line.length() + word.length() <= maxCharsPerLine) {
      line += word + " ";
      if (spaceIndex == -1) words = "";
      else words = words.substring(spaceIndex + 1);
    } else {
      // Draw the line
      tft.drawString(line, MARGIN, yPos);
      yPos += LINE_HEIGHT * TEXT_SIZE_TITLE;
      line = word + " ";
      if (spaceIndex == -1) words = "";
      else words = words.substring(spaceIndex + 1);
    }
  }
  
  // Draw final line
  if (line.length() > 0) {
    tft.drawString(line, MARGIN, yPos);
  }
}

void showBootMessage(String message) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  int textWidth = tft.textWidth(message);
  int xPos = (SCREEN_WIDTH - textWidth) / 2;
  tft.drawString(message, xPos, SCREEN_HEIGHT / 2);
}

String getTimeString() {
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", hours % 24, minutes % 60);
  return String(timeStr);
}