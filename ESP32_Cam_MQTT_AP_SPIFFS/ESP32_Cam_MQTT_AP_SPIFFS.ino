#include "esp_camera.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //ArduinoJSON6
#include "SPIFFS.h"

const IPAddress apIP(192, 168, 1, 1);
const char* apSSID = "ESPCAM_SETUP";
boolean settingMode;
String ssidList;

DNSServer dnsServer;
WebServer webServer(80);

DynamicJsonDocument CONFIG(2048);
DynamicJsonDocument CONFIGTEMP(2048);
DynamicJsonDocument WIFI(2048);

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
#include "pins.h"

const char  *ssid, *password, *mqtt_server, *mqtt_user, *mqtt_pass, *topic_PHOTO, *topic_CONFIG, *topic_UP, *HostName;
WiFiClient espClient;
PubSubClient client(espClient);


void callback(String topic, byte* message, unsigned int length) {
  if (topic == topic_PHOTO) {
    Serial.println("PING");
    take_picture();
  }
  if (topic == topic_CONFIG) {
    deserializeJson(CONFIGTEMP, message, length);
    edit_config();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  if (load_wifi()) {
    if (setup_wifi()) {
      settingMode = false;
      WiFi.mode(WIFI_STA);
      camera_init();
      client.setServer(mqtt_server, 1883);
      client.setCallback(callback);
      load_config();
      return;
    }
  }
  settingMode = true;
  setupMode();
}

void loop() {
  if (settingMode) {
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  else {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
}
