#include <WiFi.h>
#include <PubSubClient.h>
#include <CTBot.h>
#include <ArduinoJson.h>

#define pinButton 15

bool trigger = false;

// Replace the SSID/Password details as per your wifi router
const char* ssid = "Microcontroladores";
const char* password = "raspy123";

const String token_bot = "5623437737:AAEhKzdkNAzg1971Gb_ep29jszK87cZ6YFA";
const int ID_Chat = -699659624;
CTBot bot;

// Replace your MQTT Broker IP address here:
const char* mqtt_server = "200.126.14.167";
WiFiClient espClient;
PubSubClient client(espClient);

/*
   Esta es la función de interrupción.
   Esta interrupción es disparada cuando se presiona alguno de los botones
   conectados en paralelo al pin 15
*/

void IRAM_ATTR ISR()
{
  trigger = true;
}

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
    if (client.connect("ESP32_client1")) { // Change the name of client here if multiple ESP32 are connected
      //attempt successful
      Serial.println("connected");
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

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); //1883 is the default port for MQTT server
  pinMode(pinButton, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pinButton), ISR, RISING);
  bot.setTelegramToken(token_bot);
}

void loop()
{
  if (!client.connected()) {
    connect_mqttServer();
  }
  
  //client.loop();
  
  
  if (trigger) {
    client.publish("esp32/emisor", "alerta"); //topic name (to which this ESP32 publishes its data). "alerta" is the dummy value.
    bot.sendMessage(ID_Chat, "EMERGENCIA EN EL LABORATORIO DE SISTEMAS EMBEBIDOS");
    trigger = false;
    Serial.println("MENSAJE ENVIADO");
  }
  delay(100);
}
