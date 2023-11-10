// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "webpage.h"

// Replace with your network credentials
const char* ssid = "UA-Alumnos";
const char* password = "41umn05WLC";

bool motorState = 0;
const int motorPin = 23;
const int switchPin = 22;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients() {
  ws.textAll(String(motorState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      motorState = true;
      notifyClients();
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
    if (motorState){
      return "ON";
    }
    else{
      return "OFF";
    }
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
  
  File file = SPIFFS.open("/index.html");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available() - (file.size()-20)){
    Serial.write(file.read());
  }
  file.close();
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    initWebSocket();

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });

    // Start server
    server.begin();
}

void loop() {
  ws.cleanupClients();
  if(motorState){
    digitalWrite(motorPin, motorState);
    if (verify_half_turn() == 1)
    {
      motorState = !motorState;
      digitalWrite(motorPin, motorState);
      notifyClients();
    }
  }
}
