/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/
#undef ESP32

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Ramli_wifi";
const char* password = "78933476";

#define SWITCH_ON   LOW
#define SWITCH_OFF  HIGH

// Initialize Telegram BOT
#define BOTtoken "1839545543:AAFr93e36zCde2dJkyCYL7uTTokKPhxPe_w"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "25945986"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// const int kipasPin = 2;
const int kipasPin = 4;
bool kipasState = SWITCH_OFF;

const int lampuPin = 5;
bool lamputState = SWITCH_OFF;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    // Inline buttons with callbacks when pressed will raise a callback_query message
    if (bot.messages[i].type == "callback_query")
    {
      Serial.print("Call back button pressed by: ");
      Serial.println(bot.messages[i].from_id);
      Serial.print("Data on the button: ");
      Serial.println(bot.messages[i].text);

      if (text == "Kipas ON")
      {
        kipasState = SWITCH_ON;
        digitalWrite(kipasPin, kipasState);
      }

      if (text == "Kipas OFF")
      {
        kipasState = SWITCH_OFF;
        digitalWrite(kipasPin, kipasState);
      }

      if (text == "Lampu ON")
      {
        lamputState = SWITCH_ON;
        digitalWrite(lampuPin, lamputState);
      }

      if (text == "Lampu OFF")
      {
        lamputState = SWITCH_OFF;
        digitalWrite(lampuPin, lamputState);
      }

      bot.sendMessage(bot.messages[i].from_id, bot.messages[i].text, "");
    }
    else
    {
      if (text == "/start") {
        String welcome = "Welcome, " + from_name + ".\n";
        welcome += "Use the following commands to control your outputs.\n\n";
        welcome += "/led_on to turn GPIO ON \n";
        welcome += "/led_off to turn GPIO OFF \n";
        welcome += "/state to request current GPIO state \n";
        bot.sendMessage(chat_id, welcome, "");
      }

      if (text == "/led_on")
      {
        bot.sendMessage(chat_id, "LED state set to ON", "");
        kipasState = SWITCH_OFF;
        digitalWrite(kipasPin, kipasState);
      }
      
      if (text == "/led_off")
      {
        bot.sendMessage(chat_id, "LED state set to OFF", "");
        kipasState = SWITCH_ON;
        digitalWrite(kipasPin, kipasState);
      }
      
      if (text == "/state")
      {
        if (digitalRead(kipasPin))
        {
          bot.sendMessage(chat_id, "LED is ON", "");
        }
        else
        {
          bot.sendMessage(chat_id, "LED is OFF", "");
        }
      }

      if (text == "/options")
      {
        String keyboardJson = "[[{ \"text\" : \"Kipas ON\", \"callback_data\" : \"Kipas ON\" }],[{ \"text\" : \"Kipas OFF\", \"callback_data\" : \"Kipas OFF\" }],[{ \"text\" : \"Lampu ON\", \"callback_data\" : \"Lampu ON\" }],[{ \"text\" : \"Lampu OFF\", \"callback_data\" : \"Lampu OFF\" }]]";
        bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(kipasPin, OUTPUT);
  digitalWrite(kipasPin, kipasState);
  pinMode(lampuPin, OUTPUT);
  digitalWrite(lampuPin, lamputState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}