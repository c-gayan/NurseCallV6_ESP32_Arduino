#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

const int BedPin = 34;

bool previous = false;
bool Now = false;
int raw = 0;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setTxPower(WIFI_POWER_17dBm);
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("");
  Serial.println("Connecting to AWS IOT");

  Serial.print("Device Name : ");
  Serial.println(DEVICENAME);

  while (!client.connect(DEVICENAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
}

void publishMessage(String call_type)
{
  StaticJsonDocument<200> doc;
  doc["bed"] = BED;
  doc["room"] = ROOM;
  doc["call"] = call_type;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println(jsonBuffer);

  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(20);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(20);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(30);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(30);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(40);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["call"];
  Serial.println(message);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(20);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(20);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(30);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(30);
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(40);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW

}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  connectAWS();

  mySwitch.enableReceive(0);  // Connected to GPIO0 (D3) of Nodemcu 1.0 module

  delay(5000);
  // call type     Emergency || Help
  //publishMessage("Emergency");
  client.loop();
}

void loop() {



}

void remote() {

  int value = mySwitch.getReceivedValue();

  if (mySwitch.available()) {

    if (value == 5592368)
    {
      // call type     Emergency || Help
      publishMessage("Help");
      Serial.println("Help"); //print call
    }

    else if (value == 5592332)
    {
      
    }
    else
    {
      Serial.print("Bad code. You code is ");
      Serial.println(mySwitch.getReceivedValue());
    };
    // output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    mySwitch.resetAvailable();
  }
}

void bed() {

  raw = analogRead(BedPin);

  if (raw > 1000) {
    Now = true;
    if (!previous && Now) {
      // call type     Emergency || Help
      publishMessage("Emergency");
      Serial.println("Emergency"); //print call
    }
  }
  else {
    Now = false;
  }
  previous = Now;
}
