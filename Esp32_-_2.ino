#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// Replace the SSID/Password details as per your wifi router
const char* ssid = "FIEC-PRUEBA";
const char* password = "FIECPRUEBA23";

// Replace your MQTT Broker IP address here:
const char* mqtt_server = "200.126.14.167";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    c = c + 1;
    if (c > 10) {
      ESP.restart(); //restart ESP after 10 seconds
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.print("conectado");
  delay(2000);
  lcd.clear();
}

void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {

    //first check if connected to wifi
    if (WiFi.status() != WL_CONNECTED) {
      //if not connected, then first connect to wifi
      setup_wifi();
    }

    //now attemt to connect to MQTT server
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_client2")) { // Change the name of client here if multiple ESP32 are connected
      //attempt successful
      Serial.println("connected");
      // Subscribe to topics here
      client.subscribe("esp32/emisor");
      
    }
    else {
      //attempt not successful
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 2 seconds");

      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

//this function will be executed whenever there is data available on subscribed topics
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Check if a message is received on the topic "rpi/broadcast"
  if (String(topic) == "esp32/emisor") {
    if (messageTemp == "alerta") {
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("*EMERGENCIA*");
      lcd.setCursor(3,1);
      lcd.print("LAB. S.E.");
      delay(15000);
      lcd.clear();
      lcd.noBacklight();
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  setup_wifi();
  client.setServer(mqtt_server, 1883); //1883 is the default port for MQTT server
  client.setCallback(callback);
  //lcd.init();
}

void loop() {
  if (!client.connected()) {
    connect_mqttServer();
  }
  
  client.loop();

  
  //client.publish("esp32/sensor1", "88"); //topic name (to which this ESP32 publishes its data). 88 is the dummy value.

  }
