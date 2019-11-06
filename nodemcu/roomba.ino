#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#define BAUD_PIN D3
#define SSID "ssid"
#define PSK "psk"

#define OI_DELAY 50
#define MAIN_LOOP_DELAY 10
#define WIFI_DELAY 500
#define WAKEUP_DELAY 1000

#define MDNS_NAME "nodemcu-roomba"
#define HTTP_PORT 80

#define OI_CLEAN 135
#define OI_STOP 133
#define OI_DOCK 143
#define OI_START 128
#define OI_SAFE_MODE 131

// Init HTTP server
ESP8266WebServer server(HTTP_PORT);

// Sends OI serial sequence command to roomba
void send_oi_serial_command(int command) {
  Serial.write(command);
  delay(OI_DELAY);
}

void wake_up_roomba() {
  // Toggle BAUD_PIN to wake up roomba in case it sleeps
  digitalWrite(BAUD_PIN, HIGH);
  delay(OI_DELAY);
  digitalWrite(BAUD_PIN, LOW);

  // Wait to let it wake up before sending commands
  delay(WAKEUP_DELAY);
}

// Sends "start" OI command to roomba
void start_oi() {
  send_oi_serial_command(OI_START)
}

// Sends "safe mode" OI command to roomba
void roomba_safe_mode() {
  send_oi_serial_command(OI_SAFE_MODE)
}

// Sends "clean" OI command to roomba
void startCleaning() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_CLEAN);
}

// Sends "stop" OI command to roomba
void stopCleaning() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_STOP);
}

// Sends "seek dock" OI command to roomba
void toDock() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_DOCK);
}

void setup() {
  // Init BAUD_PIN
  pinMode(BAUD_PIN, OUTPUT);
  digitalWrite(BAUD_PIN, LOW);

  // Start serial and baud rate to match roomba baud rate
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_DELAY);
  }

  // Start mDNS
  MDNS.begin(MDNS_NAME);

  // Not found route handler, just send 404 with some text
  server.onNotFound([]() {
    server.send(404, "text/html", "Not Found!");
  });

  // "clean" route handler, just send empty JSON back
  server.on("/clean", []() {
    server.send(200, "application/json", "{}");
  });

  // "dock" route handler, just send empty JSON back
  server.on("/dock", []() {
    server.send(200, "application/json", "{}");
  });

  // "status" route handler, send "power status", "charging status" and "battery level" JSON back
  server.on("/status", []() {
    server.send(200, "application/json", "{\"status\": 1, \"is_charging\": true, \"battery_level\": 100.00}");
  });

  // Start HTTP server
  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();

  delay(MAIN_LOOP_DELAY);
}