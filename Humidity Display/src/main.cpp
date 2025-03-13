#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <AccelStepper.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define LED_PIN D10
#define STEPPER_MAX 315  // Adjust based on your stepper's range

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
AccelStepper stepper(AccelStepper::FULL4WIRE, A0, A1, A2, A3);

static BLEUUID serviceUUID("ec23c52d-1e1b-416d-aa74-805c6ce40e8c");
static BLEUUID charUUID("9e9a6659-9757-47a0-aec9-b78b64c21191");
BLERemoteCharacteristic* pRemoteCharacteristic;
bool connected = false;
float currentHumidity = 0;
unsigned long lastBlink = 0;
bool ledState = LOW;

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) { connected = true; }
    void onDisconnect(BLEClient* pclient) { connected = false; }
};

void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                    uint8_t* pData, size_t length, bool isNotify) {
    String humidityStr = String((char*)pData);
    currentHumidity = humidityStr.toFloat();
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Humidity: ");
    display.print(currentHumidity);
    display.println("%");
    
    if(currentHumidity < 30) display.println("Use humidifier");
    else if(currentHumidity > 60) display.println("Reduce humidity");
    else display.println("Comfortable");
    display.display();

    int targetPos = map(currentHumidity, 0, 100, 0, STEPPER_MAX);
    stepper.moveTo(targetPos);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.display();

    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(500);
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5);
}

void loop() {
    stepper.run();
    unsigned long currentMillis = millis();
    
    if (!connected) {
        if (currentMillis - lastBlink >= 500) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            lastBlink = currentMillis;
            
            display.clearDisplay();
            display.setCursor(0,0);
            display.println("Disconnected");
            display.display();
        }
    } else {
        digitalWrite(LED_PIN, HIGH);
    }
    delay(10);
}

// Keep the MyAdvertisedDeviceCallbacks class and connectToServer() function 
// from the original client example unchanged here
// [Rest of BLE client code from example remains unchanged]
