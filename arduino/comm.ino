#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <WiFi.h>

const byte rxPin = 15;
const byte txPin = 14;

SoftwareSerial Serial2(rxPin, txPin);

const char* ssid = "IPhone de Mayra";
const char* password = "iot231@@";
const char* mqtt_server = "le862a11.emqx.cloud";
const char* mqtt_username = "juacs";
const char* mqtt_passowrd = "juacs";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup_wifi() {
    delay(10);
    Serial.println("Conectando a WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Conexión WiFi establecida");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Mensaje recibido en el tópico: ");
    Serial.print(topic);
    Serial.print(". Mensaje: ");
    String messageTemp;
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    if (String(topic) == "esp32/output") {
        Serial.print("Cambiando salida a ");
        if (messageTemp == "on") {
            Serial.println("encendido");
        } else if (messageTemp == "off") {
            Serial.println("apagado");
        }
    }
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Intentando conexión MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("conectado");
            client.subscribe("esp32/output");
        } else {
            Serial.print("falló, rc=");
            Serial.print(client.state());
            Serial.println(" intentando de nuevo en 5 segundos");
            delay(5000);
        }
    }
}

void publishSensorValues(String id, float ppmCO, float ppmH2, float ppmLPG, float ppmCH4, float ppmCO2, float ppmAlcohol, float ppmAcetona, float ppmNH4, float ppmTolueno) {
    DynamicJsonDocument doc(200);
    doc["id"] = id;
    doc["ppmCO"] = ppmCO;
    doc["ppmH2"] = ppmH2;
    doc["ppmLPG"] = ppmLPG;
    doc["ppmCH4"] = ppmCH4;
    doc["ppmCO2"] = ppmCO2;
    doc["ppmAlcohol"] = ppmAlcohol;
    doc["ppmAcetona"] = ppmAcetona;
    doc["ppmNH4"] = ppmNH4;
    doc["ppmTolueno"] = ppmTolueno;
    doc["timestamp"] = now();

    String jsonString;
    serializeJson(doc, jsonString);

    client.publish("esp32/data", jsonString.c_str());
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, rxPin, txPin);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCredentials(mqtt_username, mqtt_password);

    client.setCallback(callback);
}

void loop() {
    String input = Serial2.readString();
    Serial.println(input);

    int isep = input.indexOf(' ');
    String id = input.substring(0, isep);
    String tempStr = input.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmCO = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmH2 = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmLPG = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmCH4 = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmCO2 = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmAlcohol = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmAcetona = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmNH4 = tempStr.substring(0, isep).toFloat();
    tempStr = tempStr.substring(isep + 1);

    isep = tempStr.indexOf(' ');
    float ppmTolueno = tempStr.substring(0, isep).toFloat();

    publishSensorValues(id, ppmCO, ppmH2, ppmLPG, ppmCH4, ppmCO2, ppmAlcohol, ppmAcetona, ppmNH4, ppmTolueno);

    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    delay(2000);
}
