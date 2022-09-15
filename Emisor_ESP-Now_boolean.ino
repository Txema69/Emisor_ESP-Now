#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xEC, 0xFA, 0xBC, 0xBC, 0xA0, 0x5C};

int D5 = 14; //Salida D5 ESP8266
boolean prevState=false;
boolean currState=false;
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;  // send readings timer
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    bool Rele1;
    bool Rele2;
} struct_message;
struct_message myData; // Create a struct_message called myData
// ---------Callback when data is sent-----------
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }else{
    Serial.println("Delivery fail");}
}
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  pinMode(D5,INPUT);
  //pinMode(2, OUTPUT); // Led Integrado en placa.
  pinMode(13,OUTPUT);//D7
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;}
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
   currState = digitalRead(D5);
   if(currState != prevState & currState){
      // A transition occurred (from released to pressed)
      if (myData.Rele1 == true)myData.Rele1 =false;
      else myData.Rele1 =true;
      Serial.println("Salida " + String(myData.Rele1));
      //digitalWrite(2, !myData.Rele1);
      digitalWrite(13, myData.Rele1);
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   }
   prevState = currState;
  //----------------------------envio cada x seg-------------
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
}
