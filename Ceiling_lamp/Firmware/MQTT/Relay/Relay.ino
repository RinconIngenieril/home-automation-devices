#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "TuRedWiFi"
#define WLAN_PASS       "contrasena"

/*************************** Device Setup ************************************/

//"Constant" char*
char* introduce = "Device: ";
// MQTT client ID and device's main topic
char* clientId = "habitacion/rele";
char* code_version = "1.2";


/*************** ESP8266 WiFi client for comunications *********************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

/*************************** MQTT Broker IP ***********************************/

IPAddress server(192, 168, 1, 50);

/**************************** MQTT Setup ***********************************/

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

PubSubClient mqtt_client(server, 1883, callback, client);
//General global variables
long lastMsg = 0;
char msg[50];
int value = 0;
String ip_address = "";
int outputEEPROM = 10;



void setup() {
  EEPROM.begin(16);
  Serial.begin(115200);
  delay(10);
/****************** Pins setup **************************/
  pinMode(D0, OUTPUT);
  bool lastState = EEPROM.read(outputEEPROM);
  Serial.println("");
  Serial.print("Output state: ");
  Serial.print(lastState);
  digitalWrite(D0, lastState);
  pinMode(D2, INPUT);


  /***************** Boot info **************************/
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    checkPushButton();
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  ip_address = String(WiFi.localIP()[0]) + String(".") +  String(WiFi.localIP()[1]) + String(".") +  String(WiFi.localIP()[2]) + String(".") +  String(WiFi.localIP()[3]);
  
  //Conect to MQTT broker
  reconnect(true);
  

}

void loop() {

  //Read the push button as LOW Level
  checkPushButton();

  if (!mqtt_client.connected()) {
    reconnect(false);
  }
  
  if (WiFi.status() == WL_DISCONNECTED) {
    Serial.println("WiFi connection lost");
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      checkPushButton();
      Serial.print(".");
    }
    Serial.println("WiFi connection recovered");
  }
  
  mqtt_client.loop();

}

void checkPushButton(){
  if (digitalRead(D2) == LOW){
      if (digitalRead(D0) == HIGH){
        digitalWrite(D0, LOW);
        EEPROM.write(outputEEPROM, 0);
        EEPROM.commit();
        if (mqtt_client.connected())
          mqtt_client.publish(clientId, "OFF");
      } else  {
        digitalWrite(D0, HIGH);
        EEPROM.write(outputEEPROM, 1);
        EEPROM.commit();
        if (mqtt_client.connected())
          mqtt_client.publish(clientId, "ON");
      }
      delay(200);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String topic_str = String((char *)topic);
  if ( topic_str.indexOf(clientId) >= 0){
    //Change the realay output
    String payload_str = String((char *)payload);
  
    if ( payload_str.indexOf("ON") >= 0) {
      digitalWrite(D0, HIGH);
      EEPROM.write(outputEEPROM, 1);
      EEPROM.commit();
    } else if ( payload_str.indexOf("OFF") >= 0) {
      digitalWrite(D0, LOW);
      EEPROM.write(outputEEPROM, 0);
      EEPROM.commit();
    }
  }  

}

void reconnect(bool first_run) {
  // Loop until we're reconnected
  if (!mqtt_client.connected()) {
    Serial.print("Connecting to MQTT broker");
    // Attempt to connect
    if (mqtt_client.connect(clientId)) {
      Serial.println("MQTT broker connected");
      // Once connected, publish an announcement...
      strcat(introduce, clientId);
      if (first_run)
        strcat(introduce, " Initial Signal: ");
      else
        strcat(introduce, " Reconnect Signal: ");
      
      String rssi = String(WiFi.RSSI());
      strcat(introduce, rssi.c_str());
      strcat(introduce, " dBm Version: ");
      strcat(introduce, code_version);
      strcat(introduce, " IP: ");
      strcat(introduce, ip_address.c_str());
      
      mqtt_client.publish("devices", introduce);
      strcpy(introduce, "Device. ");
      // ... and resubscribe to topis
      mqtt_client.subscribe(clientId);  //Main topic      
      mqtt_client.subscribe("info");    //Info topic
    } else {
      Serial.print("MQTT broker failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

