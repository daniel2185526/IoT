#include "WiFi.h"
#include "PubSubClient.h"
#include "sensorData.h"

// WiFi credentials
const char* ssid = "iPhone_DR";
const char* password = "DanielRH6";

// Broker details
char* server = "broker.emqx.io";
int port = 1883;

// Pin definitions
int led = 2;

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

// Variables
char datosTemperatura[40];
char datosProximidad[40];

void wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  digitalWrite(led, HIGH);
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Publisher")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Función para simular el envío de datos de proximidad
void enviarDatosProximidad() {
  float distancia = 23.0;  // Simulación de distancia fija de 23 metros
  char mensaje[50];
  sprintf(mensaje, "{\"location\":\"Paris\",\"distancia\":%.2f}", distancia);

  // Publicar el dato de proximidad en el tópico MQTT
  mqttClient.publish("PROXIMITY_SENSOR", mensaje);
  Serial.print("Proximity: ");
  Serial.println(distancia);
}

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  delay(10);
  wifi();
  mqttClient.setServer(server, port);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Generar y enviar datos de temperatura
  DatosSensor sensorData = generarDatosSensor(1);  // Ciudad de referencia
  char mensaje[50];
  sprintf(mensaje, "{\"location\":\"Paris\",\"temperature\":%.2f}", sensorData.temperatura);
  mqttClient.publish("TEM_CIUDAD", mensaje);
  Serial.print("Temperature: ");
  Serial.println(sensorData.temperatura);

  // Generar y enviar datos de proximidad
  enviarDatosProximidad();

  delay(5000);  // Espera de 5 segundos entre publicaciones
}
