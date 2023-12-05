#include <MQUnifiedsensor.h>
#include <SoftwareSerial.h>

const byte rxPin = 14;
const byte txPin = 15;

// Objeto para comunicación serial
SoftwareSerial mySerial(rxPin, txPin);

// Definiciones
#define placa "Arduino MEGA"
#define Voltage_Resolution 5
#define pin0 A1                 // Para el MQ135
#define pin1 A3                 // Para el MQ7
#define typeMQ135 "MQ-135"      // MQ135
#define typeMQ135 "MQ-7"        // MQ135
#define ADC_Bit_Resolution 10   // Resolución de bits del ADC
#define RatioMQ135CleanAir 3.6  // RS / R0 = 3.6 ppm, para MQ135
#define RatioMQ7CleanAir 27.5   // RS / R0 = 27.5 ppm, para MQ7

// Variables para almacenar los valores de los sensores
// MQ7
float ppmCO;
float ppmH2;
float ppmLPG;
float ppmCH4;
// MQ135
float ppmCO2;
float ppmAlcohol;
float ppmTolueno;
float ppmNH4;
float ppmAcetona;

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin0, typeMQ135);
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, pin1, typeMQ7);

void printSensorValuesToJson(String id, float ppmCO, float ppmH2, float ppmLPG, float ppmCH4, float ppmCO2, float ppmAlcohol, float ppmAcetona, float ppmNH4, float ppmTolueno) {
    // Create a JSON document
    StaticJsonDocument<200> doc;

    // Add values to the JSON document
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
    doc["timestamp"] = now();  // Add the current Unix timestamp

    // Serialize the JSON document to a string
    String jsonString;
    serializeJson(doc, jsonString);

    // Print the JSON string
    Serial.println(jsonString);
}

void setup() {
    // Inicialización de la comunicación serial con la computadora
    Serial.begin(9600);
    // Inicialización de la comunicación serial con el ESP32
    mySerial.begin(9600);

    // Calibración del sensor MQ135
    MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
    MQ135.init();
    MQ135.setRL(1);
    Serial.print("Calibrando MQ135. Por favor espere.");
    float calcR0MQ135 = 0;
    for (int i = 1; i <= 10; i++) {
        MQ135.update();  // Update data, Arduino leerá el voltaje del pin analógico
        calcR0MQ135 += MQ135.calibrate(RatioMQ135CleanAir);
        Serial.print(".");
    }
    MQ135.setR0(calcR0MQ135 / 10);
    Serial.println("  hecho!.");

    if (isinf(calcR0MQ135)) {
        Serial.println("Advertencia: Problema de conexión, R0-MQ135 es infinito (circuito abierto detectado) por favor compruebe su cableado y alimentación.");
        while (1)
            ;
    }
    if (calcR0MQ135 == 0) {
        Serial.println("Advertencia: Problema de conexión detectado, R0-MQ135 es cero (la patilla analógica hace cortocircuito a tierra), compruebe el cableado y la alimentación.");
        while (1)
            ;
    }

    // Calibración del sensor MQ7
    MQ7.setRegressionMethod(1);  //_PPM =  a*ratio^b
    MQ7.init();
    MQ7.setRL(1);
    Serial.print("Calibrando MQ7. Por favor espere.");
    float calcR0MQ7 = 0;
    for (int i = 1; i <= 10; i++) {
        MQ7.update();  // Update data, Arduino leerá el voltaje del pin analógico
        calcR0MQ7 += MQ7.calibrate(RatioMQ135CleanAir);
        Serial.print(".");
    }
    MQ7.setR0(calcR0MQ7 / 10);
    Serial.println("  hecho!.");

    if (isinf(calcR0MQ7)) {
        Serial.println("Advertencia: Problema de conexión, R0-MQ7 es infinito (circuito abierto detectado) por favor compruebe su cableado y alimentación");
        while (1)
            ;
    }
    if (calcR0MQ7 == 0) {
        Serial.println("Advertencia: Problema de conexión detectado, R0-MQ7 es cero (la patilla analógica hace cortocircuito a tierra), compruebe el cableado y la alimentación.");
        while (1)
            ;
    }
}

void loop() {
    // MQ7
    // Actualizar valores del sensor MQ135
    MQ7.update();

    // Leer ppm de CO del sensor MQ7
    MQ7.setA(99.042);
    MQ7.setB(-1.518);
    ppmCO = MQ7.readSensor();

    // Leer ppm de H2 del sensor MQ7
    MQ7.setA(69.014);
    MQ7.setB(-1.374);
    ppmH2 = MQ7.readSensor();

    // Leer ppm de LPG del sensor MQ7
    MQ7.setA(700000000);
    MQ7.setB(-7.703);
    ppmLPG = MQ7.readSensor();

    // Leer ppm de CH4 del sensor MQ7
    MQ7.setA(60000000000000);
    MQ7.setB(-10.54);
    ppmLPG = MQ7.readSensor();

    // MQ135
    // Actualizar valores del sensor MQ135
    MQ135.update();

    // Leer ppm de CO2 del sensor MQ135
    MQ135.setA(110.47);
    MQ135.setB(-2.862);
    ppmCO2 = MQ135.readSensor();

    // Leer ppm de Alcohol del sensor MQ135
    MQ135.setA(77.255);
    MQ135.setB(-3.18);
    ppmAlcohol = MQ135.readSensor();

    // Leer ppm de Acetona del sensor MQ135
    MQ135.setA(34.668);
    MQ135.setB(-3.369);
    ppmAcetona = MQ135.readSensor();

    // Leer ppm de NH4 del sensor MQ135
    MQ135.setA(102.2);
    MQ135.setB(-2.473);
    ppmNH4 = MQ135.readSensor();

    // Leer ppm de Tolueno del sensor MQ135
    MQ135.setA(44.947);
    MQ135.setB(-3.445);
    ppmTolueno = MQ135.readSensor();

    // Definimos el id del dispositivo IOT
    String id = "lab-cs";

    // Imprimir valores de los sensores en formato JSON
    printSensorValuesToJson(id, ppmCO, ppmH2, ppmLPG, ppmCH4, ppmCO2, ppmAlcohol, ppmAcetona, ppmNH4, ppmTolueno);

    // Imprimir valores de los sensores en la consola serial
    Serial.print("CO: ");
    Serial.print(ppmCO);
    Serial.print(" ppm\t");
    Serial.print("H2: ");
    Serial.print(ppmH2);
    Serial.print(" ppm\t");
    Serial.print("LPG: ");
    Serial.print(ppmLPG);
    Serial.print(" ppm\t");
    Serial.print("CH4: ");
    Serial.print(ppmCH4);
    Serial.print(" ppm\t");
    Serial.print("CO2: ");
    Serial.print(ppmCO2);
    Serial.print(" ppm\t");
    Serial.print("Alcohol: ");
    Serial.print(ppmAlcohol);
    Serial.print(" ppm\t");
    Serial.print("Acetona: ");
    Serial.print(ppmAcetona);
    Serial.print(" ppm\t");
    Serial.print("NH4: ");
    Serial.print(ppmNH4);
    Serial.print(" ppm\t");
    Serial.print("Tolueno: ");
    Serial.print(ppmTolueno);
    Serial.println(" ppm");

    // Leer valores de sensores cada 2 segundos
    delay(2000);
}