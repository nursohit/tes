/*
GENERIC SDK FOR ESP
Add ESP8266 and PubSubClient libraries into your arduino libraries forlder
You can find those library files inside the zip file.
update the send_event function and do_actions function with respect your requirements.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define PUB_INTERVAL 30

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define PUB_INTERVAL 30
unsigned long lastPublish = 0;

static const int RXPin = D3, TXPin = D4;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


// Update these with values suitable for your network.
const char* ssid = "sohit";
const char* password = "bukasandi";

//Backend credentials
const char* mqtt_server = "mqtt.flexiot.xl.co.id";
String DEVICE_SERIAL = "5016148064631839" ; //update the device serial according to the serial given by the consumer portal 
const char* EVENT_TOPIC = "generic_brand_758generic_devicev2/common";
String SUB_TOPIC_STRING = "+/" + DEVICE_SERIAL + "/generic_brand_758/generic_device/v2/sub";


WiFiClient espClient;
PubSubClient client(espClient);
char msg[300];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//receiving a message
void callback(char* topic, byte* payload,long length) {
  Serial.print("Message arrived [");
  Serial.print(SUB_TOPIC_STRING);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  do_actions(msg);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"rabbit","rabbit")) {
      Serial.println("connected");
      //subscribe to the topic
      const char* SUB_TOPIC = SUB_TOPIC_STRING.c_str();
      client.subscribe(SUB_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
   ss.begin(GPSBaud);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

 

}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
    setup_wifi();  
  }
  if (WiFi.status()== WL_CONNECTED && !client.connected()) {
    reconnect();
  }

  client.loop();


}

void publish_message(const char* message){
  client.publish(EVENT_TOPIC,message);  
}

//====================================================================================================================================================================  
void send_event(const char* message){
  //create this function to send some events to the backend. You should create a message format like this
  /* Eg :{
        "mac":"6655591876092787",
        "eventName":"eventOne",
        "state":"none",
        "eventOne":{
          "ev1Value1":30
        }
      }
  
  */
  
      StaticJsonBuffer<800>jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(message);
      
      if(strcmp(root["event"],"GPS") == 0){
        while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }
  }
 }
      
  Serial.println(message);
  //Should call publish_message() function to send events. Pass your message as parameter
  // Eg : publish_message(message);
}
//====================================================================================================================================================================
//==================================================================================================================================================================== 
void do_actions(const char* message){
  //Create this function according to your actions. you will receive a message something like this
  /* Eg : {
        "action":"actionOne",
        "param":{
          "ac1Value1":"1110" ,
          "parentMac":"6655591876092787",
          "ac1Value4":"port:03",
          "ac1Value5":"on",
          "mac":"6655591876092787",
          "ac1Value2":"2220",
          "ac1Value3":"567776"
        }
      } */

      
}

//=====================================================================================================================================================================
