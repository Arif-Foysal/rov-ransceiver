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

//define all the pins 
  //pot pin
  #define potPin A0
  //Button pins
    #define btnFw D0
  #define btnBw D3
  #define btnLeft D4
  #define btnRight D5

  #define btnTl D6
  #define btnTr D7
  #define btnSink 3 //GPIO 3


// REPLACE WITH THE MAC Address of your receiver 2C:F4:32:70:13:54
uint8_t broadcastAddress[] = {0x48, 0x3F, 0xDA, 0x5F, 0xDF, 0xEA};
//below structure is outgoing
typedef struct struct_commands {
//pot value
  byte potvalue;
//button status
// Variable to store the state of the pushbutton
  bool btnFwPressed=0;
  bool btnBwPressed=0;
  bool btnLeftPressed=0;
  bool btnRightPressed=0;
  bool btnTlPressed=0;
  bool btnTrPressed=0;
  bool btnSinkPressed=0; 
} struct_commands;
// Create a struct_commands called outgoingCommands to hold outgoing data
// Create a struct_commands called incomingData to hold incoming data
typedef struct struct_data {
byte data;
} struct_data;

struct_commands outgoingCommands;
struct_data inData;

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
  memcpy(&inData, incomingData, sizeof(inData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Message received:");
  // received=incomingData.data;
Serial.println(inData.data);
}
void setup() {
// Init Serial Monitor
  Serial.begin(9600);
//set pinmode of button pins as INPUT
  pinMode(btnFw,INPUT);
  pinMode(btnBw,INPUT_PULLUP);
  pinMode(btnLeft,INPUT_PULLUP);
  pinMode(btnRight,INPUT);
  pinMode(btnTl,INPUT);
  pinMode(btnTr,INPUT);
  pinMode(btnSink,INPUT);

//set pinmode of pot pin
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
 unsigned long previousMillis_pot = 0;  // Store the previous time
 const long sendInterval = 50;        // Set the sendInterval in milliseconds
//Note: previousMillis and interval should be different for every delay event
void loop() {
  unsigned long currentMillis = millis();
 

   // Check if it's time to send the data
  if (currentMillis - previousMillis_pot >= sendInterval) {
    previousMillis_pot = currentMillis;  // Save the last time you sent the data

    // Your code to send data goes here
  // strcpy(outgoingCommands.msg, "Hello from NodeMCU");
  outgoingCommands.potvalue= map(analogRead(potPin) , 10, 1023, 0, 180); //read pot value
//read button status
  outgoingCommands.btnFwPressed=digitalRead(btnFw);
  outgoingCommands.btnBwPressed=!digitalRead(btnBw);
  outgoingCommands.btnLeftPressed=!digitalRead(btnLeft);
  outgoingCommands.btnRightPressed=digitalRead(btnRight);
  outgoingCommands.btnTlPressed=digitalRead(btnTl);
  outgoingCommands.btnTrPressed=digitalRead(btnTr);
  outgoingCommands.btnSinkPressed=digitalRead(btnSink); 


  Serial.println("Outgoing readings potentiometer value:");
  Serial.println(outgoingCommands.potvalue);
  esp_now_send(broadcastAddress, (uint8_t *) &outgoingCommands, sizeof(outgoingCommands));

  }
}
