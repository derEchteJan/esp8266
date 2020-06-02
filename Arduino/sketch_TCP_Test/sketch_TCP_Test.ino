#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "http.h"
#include "webfiles.h"

// WIFI
const char *ssid = "BB4";
const char *wpa_key = "6151688957244792";
#define HTTP_PORT 80
#define TCP_PORT 69
#define HTTP_TIMEOUT_MS 3000
const IPAddress ip_addr(192, 168, 178, 60); // configured static ip address
const IPAddress subnet_mask(255, 255, 255, 0);
const IPAddress gateway_addr(192, 168, 178, 1);


// UART
#define BAUD_RATE 9600


// GPIO
#define LED_ON 0
#define LED_OFF 1
#define PIN_R 12
#define PIN_G 15
#define PIN_B 14


// Global Variables
WiFiServer http_server(HTTP_PORT);
WiFiServer tcp_server(TCP_PORT);



// MARK: - SETUP FUNCTIONS

void setup_gpio() {
  Serial.println("begin gpio setup..");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);
  analogWrite(PIN_R, 0);
  analogWrite(PIN_G, 0);
  analogWrite(PIN_B, 0);
  Serial.println("complete.\n");
}

void setup_serial() {
  Serial.println("begin uart setup..");
  Serial.begin(BAUD_RATE);
  Serial.println("complete.\n");
}

void setup_wifi() {
  Serial.println("begin wifi setup..");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,wpa_key);
  Serial.println("trying to connect to network...");
  while (WiFi.status() != WL_CONNECTED){
    digitalWrite(LED_BUILTIN, LED_ON); delay(100);
    digitalWrite(LED_BUILTIN, LED_OFF); delay(100);
  }
  Serial.println("connected successfully");
  WiFi.config(ip_addr, gateway_addr, subnet_mask);
  Serial.println("configured network settings");
  http_server.begin();
  tcp_server.begin();
  Serial.println("complete.\n");
}



// MARK: - SUBS

void ledSignalSetupComplete() {
  delay(500); digitalWrite(LED_BUILTIN, LED_ON); delay(800);
  digitalWrite(LED_BUILTIN, LED_OFF);
}

/*void sendHTTPResponse(WiFiClient client, char* message) {
  char* httpHeaders = "HTTP/1.1 200 OK\n"
  "Content-Type: text/html\n"
  "Connection: close\n"
  "Access-Control-Allow-Origin: *\n"
  "Content-Length: "
  ;
  char c = httpHeaders[0];
  for(int i = 0; c != '\0'; c = httpHeaders[++i]) {
    client.write(c);
  }
  int messageLength = strlen(message);
  int skipLeadingZeroes = true;
  for(int lenDivider = 100; lenDivider > 0; lenDivider /= 10) {
    char digit = (messageLength / lenDivider % 10) + '0';
    if(digit == '0' && skipLeadingZeroes) {
      continue; 
    } else {
      skipLeadingZeroes = false;
      client.write(digit);
    }
  }
  client.write('\n');
  client.write('\n');
  c = message[0];
  for(int i = 0; c != '\0'; c = message[++i]) {
    client.write(c);
  }
  client.write('\n');
}*/

/*
void readHTTPRequest(WiFiClient client) {
  Serial.println("Reading Request:");
  delay(500);
  while(client.available() ) {
    Serial.write(client.read());
  }
}*/

#define TCP_BUF_L 5
byte tcpBuffer[TCP_BUF_L];
int tcpBufferReadIndex = 0;

void readTCPRequest(WiFiClient client) {
  while(client.available() && tcpBufferReadIndex < TCP_BUF_L) {
    byte data = client.read();
    if(data == '\n' || data == '\r') continue;
    tcpBuffer[tcpBufferReadIndex++] = data;
  }
  if(tcpBufferReadIndex >= TCP_BUF_L) {
    tcpBufferReadIndex = 0;
    handleTCPRequest(client);
    for(int i = 0; i < TCP_BUF_L; i++) tcpBuffer[i] = 0;
  }
}

void handleTCPRequest(WiFiClient client) {
  byte opcode = tcpBuffer[0];
  int val = 0;
  switch(opcode) {
  // direct color
  case 'c':
    analogWrite(PIN_R, tcpBuffer[2]);
    analogWrite(PIN_G, tcpBuffer[3]);
    analogWrite(PIN_B, tcpBuffer[4]);
    break;
  // only red, ascii encoded
  case 'r': 
  case 'g':
  case 'b':
    val = (tcpBuffer[2] - '0') * 100 + (tcpBuffer[3] - '0') * 10 + (tcpBuffer[4] - '0');
    if(opcode == 'r') {
      analogWrite(PIN_R, val);
      Serial.printf("TCP: red value set to %d\n", val);
      writeTCPResponse(client, "red value set");
    }
    if(opcode == 'g') {
      analogWrite(PIN_G, val);
      Serial.printf("TCP: green value set to %d\n", val);
      writeTCPResponse(client, "green value set");
    }
    if(opcode == 'b') {
      analogWrite(PIN_B, val);
      Serial.printf("TCP: blue value set to %d\n", val);
      writeTCPResponse(client, "blue value set");
    }
    break;
  default:
    char* response = "unknown opcode 0";
    response[15] = opcode;
    writeTCPResponse(client, response);
  }
}

void writeTCPResponse(WiFiClient client, char* response) {
  char c = response[0];
  for(int i = 0; c != '\0'; c = response[++i]) client.write(c);
  client.write('\r'); client.write('\n');
  //client.stop();
}

void onRecievedRequest(WiFiClient client, HTTP::Request request) {
  HTTP::writeResponse(client, "200 OK", WEBFILES::buildIndexHtml());
  /*switch(request.method) {
  case HTTP_HELPER::get:
    if(request.hasPath("/") || request.hasPath("/index.html"))
      HTTP_HELPER::writeResponse(client, 200, NULL);
    else if(request.hasPath("/script.js"))
      HTTP_HELPER::writeResponse(client, 200, "some script");
    return;
  case HTTP_HELPER::post:
    return;
  }
  HTTP_HELPER::writeResponse(client, 404, NULL);*/
}


// MARK: - RTOS LIVE CYCLE

void setup() {
  setup_gpio();
  setup_serial();
  setup_wifi();
  ledSignalSetupComplete();
}

int board_led_state = 0;

void loop() {
  WiFiClient http_client = http_server.available();
  if (http_client) {
    Serial.print("HTTP: >> ");Serial.print(http_client.remoteIP());Serial.println(" connected");
    HTTP::readRequest(http_client, HTTP_TIMEOUT_MS, onRecievedRequest);
    Serial.println("HTTP: << client disconnected\n");
  }
  WiFiClient tcp_client = tcp_server.available();
  if(tcp_client) {
    while(tcp_client.connected()) {
      readTCPRequest(tcp_client);
    }
  }
}
