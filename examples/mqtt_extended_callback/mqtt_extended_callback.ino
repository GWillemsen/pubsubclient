/*
 Extended callback MQTT example

 This sketch demonstrates the capabilities of the library with regards to
 optionally supply a user data pointer to callback that is called when a
 message is received from the server.
 It connects to an MQTT server then:
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
    It will also increment the counter in the user data which is then printed
    by the loop method.

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 For the basic subscribe and publish mechanism see 'mqtt_basic'.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress server(172, 16, 0, 2);

void callback(char* topic, byte* payload, unsigned int length, void *userdata) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (userdata != nullptr) {
    ((int*)userdata) += 1;
  }
}

EthernetClient ethClient;
PubSubClient client(ethClient);
int callbackCounter = 0;

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  client.setCallback(callback, &callbackCounter);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  int previousCounter = callbackCounter;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (previousCounter != callbackCounter)
  {
    Serial.print("Callback counter was updated from: ");
    Serial.print(previousCounter);
    Serial.print(" to: ");
    Serial.print(callbackCounter);
    Serial.println(".");
  }
}
