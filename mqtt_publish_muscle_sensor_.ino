

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define MQTT_SERVER "ajax-der-kleine.clients.ldv.ei.tum.de"
const char* ssid = "LDV-CPP";
const char* password = "LDV-LK-C++";
const int x = 800 ; 
char* Topic = "servo";
//MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  //we're not subscribed to any topics so we dont need to have anything here
}

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);
void setup() {
 //start the serial line for debugging
  Serial.begin(115200);
  delay(100); 
//start wifi subsystem
  WiFi.begin(ssid, password);

  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
      delay(2000);
}

void loop() {
   //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}
    client.loop();
    if(analogRead(A0) > x) {
      client.publish(Topic,"hand closed");
    
  }

//If the EMG data is lower than x the hand opens
  else if (analogRead(A0) < x) {
    client.publish(Topic,"hand open");
    
  }

//A delay to slow down the process
  delay(100);

}

void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(Topic);
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}
