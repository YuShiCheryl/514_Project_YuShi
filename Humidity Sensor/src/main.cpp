#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SERVICE_UUID        "ec23c52d-1e1b-416d-aa74-805c6ce40e8c"
#define CHARACTERISTIC_UUID "9e9a6659-9757-47a0-aec9-b78b64c21191"
#define LED_PIN D10
#define SEALEVELPRESSURE_HPA (1013.25)

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long sensorInterval = 10000;

Adafruit_BME280 bme;
bool ledOff = false;
unsigned long ledOffTime = 0;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    if (!bme.begin(0x76)) {
        Serial.println("Could not find BME280 sensor!");
        while (1);
    }

    BLEDevice::init("HumiditySensor");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Waiting for connections...");
}

void loop() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= sensorInterval) {
        float humidity = bme.readHumidity();
        // Apply calibration here if needed
        String humidityStr = String(humidity);
        
        if (deviceConnected) {
            pCharacteristic->setValue(humidityStr.c_str());
            pCharacteristic->notify();
            digitalWrite(LED_PIN, LOW);
            ledOff = true;
            ledOffTime = currentMillis;
            Serial.println("Notified value: " + humidityStr);
        }
        previousMillis = currentMillis;
    }

    if (ledOff && (currentMillis - ledOffTime >= 500)) {
        digitalWrite(LED_PIN, HIGH);
        ledOff = false;
    }

    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Advertising started");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
