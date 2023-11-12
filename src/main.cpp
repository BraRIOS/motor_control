// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

// Replace with your network credentials
//const char* ssid = "UA-Alumnos";
//const char* password = "41umn05WLC";
const char* ssid = "Fibertel WiFi212 2.4GHz";
const char* password = "dorysmerlin0270";

bool dispensing = 0;
int candiesStock = 40;
int price = 5;
int buyed = 0;
const int motorPin = 23;
const int switchPin = 22;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyDispensing() {
  ws.textAll("Dispensing:"+String(dispensing));
}

void notifyCandies() {
  ws.textAll("Candies:"+String(candiesStock));
}

void notifyPrice() {
  ws.textAll("Price:"+String(price));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.printf("WebSocket client message: %s\n", (char*)data);
    if (strncmp((char*)data, "buy", 3) == 0) {
      int buy = atoi((char*)&data[4]);
      if (buy <= candiesStock && buy > 0){
        Serial.printf("Dispensing %d candies\n", buy);
        buyed = buy;
        candiesStock -= buy;
        dispensing = 1;
        notifyDispensing();
        notifyCandies();
      }    
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (dispensing){
      return "Dispensing...";
    }
    else{
      return "Ready";
    }
  }
  else if(var == "CANDIES"){
    return String(candiesStock);
  }
  else if(var == "PRICE"){
    return String(price);
  }
  return String();
}

static int verify_half_turn(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (digitalRead(switchPin) == 0)
        {
            return 1;
        }
        delay(100);
    }
    return 0;
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(SERIAL_BAUD);

  //innitialize pins
  pinMode(motorPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(motorPin, LOW);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 
  Serial.println("SPIFFS mounted successfully");
    
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      //request->send_P(200, "text/html", index_html, processor);
      request -> send(SPIFFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();
}

void loop() {
  if(dispensing){
    while (buyed){
      digitalWrite(motorPin, HIGH);
      if (verify_half_turn()){
        digitalWrite(motorPin, LOW);
        delay(1000);
        buyed--;
      }        
    }
    dispensing = 0;
    notifyDispensing();
  }
}
