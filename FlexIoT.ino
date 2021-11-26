/*
GENERIC SDK FOR ESP
Add ESP8266 and PubSubClient libraries into your arduino libraries forlder
You can find those library files inside the zip file.
update the send_event function and do_actions function with respect your requirements.
*/
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define motor 5

// Update these with values suitable for your network.
const char* ssid = "XL-Guest";
const char* password = "Pri[o]rita$";

//Backend credentials
const char* mqtt_server = "devdeviceconnect.iot.xpand.asia";
String DEVICE_SERIAL = "0685394321580259" ; //update the device serial according to the serial given by the consumer portal 
const char* EVENT_TOPIC = "fishfeeder/tes123/v4/common";
String SUB_TOPIC_STRING = "+/" + DEVICE_SERIAL + "/fishfeeder/tes123/v4/sub";
const char* mqtt_username = "fishfeeder-tes123-v4_6795";
const char* mqtt_password = "1637549955_6795";

float hum;
float temp;
float weight;
float set_weight;
float set_temp;

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
  memset(msg, 0, sizeof(msg));
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
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_username,mqtt_password)) {
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
  setup_wifi();
  client.setServer(mqtt_server, 1893);
  client.setCallback(callback);
  pinMode(motor, OUTPUT);
}

void loop() {

  if(WiFi.status() != WL_CONNECTED){
    setup_wifi();  
  }
  if (WiFi.status()== WL_CONNECTED && !client.connected()) {
    reconnect();
  }
  hum = random(0,100);
  temp = random(0,100);
  weight = random(0,100);
  //set_weight = random(0,100);
  //set_temp = random(0,100);
  
  Serial.println(hum);
  Serial.println(temp);
  Serial.println(weight);
  Serial.println(set_weight);
  Serial.println(set_temp);
  
  client.loop();
  send_event();
  delay(5000);
}

void publish_message(const char* message){
  client.publish(EVENT_TOPIC,message);  
}

//====================================================================================================================================================================  
void send_event(){
  //create this function to send some events to the backend. You should create a message format like this

  String message = "{\"eventName\":\"EkanaXin_Data\",\"status\":\"none\",\"a\":" + String(temp) + ",\"b\":" + String(hum) + ",\"c\":" + String(weight)+ ",\"e\":" + String(set_temp) + ",\"d\":" + String(set_weight) + ",\"v4\":\"" + DEVICE_SERIAL + "\"}";
  char* msg = (char*)message.c_str();
  
  //Should call publish_message() function to send events. Pass your message as parameter
  publish_message(msg);
}
//====================================================================================================================================================================
//==================================================================================================================================================================== 
void do_actions(const char* message){
  StaticJsonDocument<800> root;
  deserializeJson(root, message);
  
  if (strcmp(root["action"], "buttonon") == 0){
    digitalWrite(motor, 0);
    Serial.println("Motor OFF");
  }
  
//  else {
//    digitalWrite(motor, 1);
//    Serial.println("Motor ON");
//  }

  if (strcmp(root["action"], "buttonoff") == 0){
    digitalWrite(motor, 1);
    Serial.println("Motor ON");
  }

  if (strcmp(root["action"], "Temp_Act") == 0) {
    set_temp = (int)root["param"]["temp"];
  }

  if (strcmp(root["action"], "Weight_Act") == 0) {
    set_weight = (int)root["param"]["weight"];
  }
  
  Serial.println(message);
}
//=====================================================================================================================================================================
