/*
  Flow of the code
  1 - Put WiFi in STA Mode
  2 - Intialize ESPNOW
  3 - Set Role to Combo
  4 - Add peer device
  5 - Define Send Callback Function
  6 - Define Receive Callback Function
*/
#include <ESP8266WiFi.h>
#include <espnow.h>
//servo
#include <Servo.h>
Servo armServo;
//define all the pins 
#define potPin A0
#define armServoPin D0

// REPLACE WITH THE MAC Address of your receiver
uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x3C, 0x95, 0xD0};
//structure of incoming packet
typedef struct struct_commands {
  byte potvalue;
} struct_commands;

// structure of the outgoing packet
typedef struct struct_data {

} struct_data;

// Create a struct_commands called outgoingData to hold outgoing data
struct_data outgoingData;
// Create a struct_commands called incomingCommands to hold incoming data
struct_commands incomingCommands;
// Variable to store if sending data was successful
String success;
// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: "); 
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }

  else {
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingCommands, incomingData, sizeof(incomingCommands));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Potentiometer Value Received:");
  Serial.println(incomingCommands.potvalue);
}
void setup() {
  //Init Servo
  armServo.attach(armServoPin);
  // armServo.write(0);
  // Init Serial Monitor
  Serial.begin(9600);
  //set pinmode
  pinMode(potPin,INPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
//delay management
unsigned long previousMillis_arm = 0;  // Store the previous time
const long interval_arm = 15;        // Set the interval_arm in milliseconds
//Note: previousMillis and interval should be different for every delay event
void loop() {
  unsigned long currentMillis = millis();
   // Check if it's time to send the data
  if (currentMillis - previousMillis_arm >= interval_arm) {
    previousMillis_arm = currentMillis;  // Save the last time you sent the data

    armServo.write(incomingCommands.potvalue);
  }



}
