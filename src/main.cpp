#include <WiFi.h>
#include <PubSubClient.h>

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
#define TRIGGER_PIN 5
#define ECHO_PIN 18
#define MSG_BUFFER_SIZE 50

const char* ssid = "Locutus";
const char* password = "secret-passw";
const char* mqtt_server = "192.168.1.79";
const char* topic = "sensors/salt-level/meter-cupboard";
const char* status_topic = "sensors/status/salt-level/meter-cupboard";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;
long duration;
float distanceCm;
float distanceInch;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
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

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(length);
  Serial.print("] ");
  Serial.println(status_topic);
  if (strcmp(topic, status_topic) == 0 && length == 0) {
    Serial.println("Received the proper message.");
    snprintf (msg, MSG_BUFFER_SIZE, "{ \"level\" : \"open\" }");
    client.publish(status_topic, msg);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "SaltLevelSensor-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, MSG_BUFFER_SIZE, "init");
      client.publish(topic, msg);
      // ... and resubscribe
      client.subscribe(status_topic);
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
    pinMode(TRIGGER_PIN, OUTPUT); // Sets the TRIGGER_PIN as an Output
    pinMode(ECHO_PIN, INPUT); // Sets the ECHO_PIN as an Input
    Serial.begin(9600); // Starts the serial communication
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {

    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    // Clears the TRIGGER_PIN
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
    duration = pulseIn(ECHO_PIN, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_SPEED/2;

    // Convert to inches
    distanceInch = distanceCm * CM_TO_INCH;

    snprintf (msg, MSG_BUFFER_SIZE, "{ \"level\" : \"%f\" }", distanceCm);
    client.publish(topic, msg);
    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);

    delay(10000);
}