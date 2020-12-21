#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Roomba.h>

#define BAUD_PIN D3
#define SSID "ssid"
#define PSK "psk"

#define OI_DELAY 50
#define MAIN_LOOP_DELAY 1
#define WIFI_DELAY 500
#define BAUD_TRIGGER_DELAY 50

#define MDNS_NAME "roomba"
#define HTTP_PORT 80

#define OI_CLEAN 135
#define OI_STOP 133
#define OI_DOCK 143
#define OI_STOP 173
#define OI_START 128
#define OI_SAFE_MODE 131
#define OI_CHARGING_STATE 21
#define OI_BATTERY_CHARGE 25
#define OI_BATTERY_CAPACITY 26

// Init HTTP server
ESP8266WebServer server(HTTP_PORT);

// Init roomba sensors object
Roomba roomba(&Serial, Roomba::Baud115200);

uint8_t tempBuf[52];

// Sends OI serial sequence command to roomba
void send_oi_serial_command(int command) {
  Serial.write(command);
  delay(OI_DELAY);
}

void wake_up_roomba() {
  // Toggle BAUD_PIN to wake up roomba in case it sleeps
  digitalWrite(BAUD_PIN, HIGH);
  delay(BAUD_TRIGGER_DELAY);
  digitalWrite(BAUD_PIN, LOW);
  delay(BAUD_TRIGGER_DELAY);
}

// Sends "start" OI command to roomba
void start_oi() {
  send_oi_serial_command(OI_START);
}

// Sends "safe mode" OI command to roomba
void roomba_safe_mode() {
 send_oi_serial_command(OI_SAFE_MODE);
}

// Sends "clean" OI command to roomba
void startCleaning() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_CLEAN);
}

// Sends "seek dock" OI command to roomba
void toDock() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_DOCK);
}

// Sends "stop" OI command to roomba
void stopRoomba() {
  wake_up_roomba();
  start_oi();
  roomba_safe_mode();
  send_oi_serial_command(OI_STOP);
}

float getRoombaBatteryLevel() {
  roomba.start();
  roomba.getSensors(OI_BATTERY_CHARGE, tempBuf, 2);
  long battery_current_mAh = tempBuf[1] + 256 * tempBuf[0];
  delay(OI_DELAY);

  roomba.getSensors(OI_BATTERY_CAPACITY, tempBuf, 2);
  long battery_total_mAh = tempBuf[1] + 256 * tempBuf[0];

  if (battery_total_mAh != 0) {
    return (battery_current_mAh / battery_total_mAh) * 100;
  }

  return 0.0;
}

bool getRoombaChargingState() {
  roomba.start();
  roomba.getSensors(OI_CHARGING_STATE, tempBuf, 1);
  int charging_state = tempBuf[0];
  delay(OI_DELAY);

  if (charging_state != 0) {
    return true;
  }

  return false;
}

void setup() {
  // Init BAUD_PIN
  pinMode(BAUD_PIN, OUTPUT);
  digitalWrite(BAUD_PIN, LOW);

  // Start serial and baud rate to match roomba default baud rate
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
    startCleaning();
    server.send(200, "application/json", "{}");
  });

  // "dock" route handler, just send empty JSON back
  server.on("/dock", []() {
    toDock();
    server.send(200, "application/json", "{}");
  });

  // "stop" route handler, just send empty JSON back
  server.on("/stop", []() {
    stopRoomba();
    server.send(200, "application/json", "{}");
  });

  // "status" route handler, send "power status", "charging status" and "battery level" JSON back
  server.on("/status", []() {
    server.send(200, "application/json", "{\"status\": 0, \"is_charging\": false, \"battery_level\": 100.00}");
  });

  // Start HTTP server
  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();
  delay(MAIN_LOOP_DELAY);
}
