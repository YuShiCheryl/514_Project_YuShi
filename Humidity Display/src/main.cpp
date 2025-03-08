#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Stepper motor configuration (using 4 pins: A0 to A3)
#define STEPPER_PIN1  A0
#define STEPPER_PIN2  A1
#define STEPPER_PIN3  A2
#define STEPPER_PIN4  A3
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_PIN1, STEPPER_PIN2, STEPPER_PIN3, STEPPER_PIN4);

// LED on pin 11
#define LED_PIN 9

// BLE service UUID used in your project (change if needed)
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"

bool bleDeviceFound = false;

// Custom callback to handle discovered BLE devices
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.print("Found sensing device: ");
      Serial.println(advertisedDevice.toString().c_str());
      bleDeviceFound = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED on initially
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Display OK");
  display.display();
  delay(2000);
  
  // Initialize stepper motor test
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(100);
  stepper.setCurrentPosition(0);
  
  // Test the stepper: move forward 100 steps then back 100 steps.
  Serial.println("Testing stepper motor...");
  stepper.moveTo(100);
  while (stepper.currentPosition() != 100) {
    stepper.run();
  }
  delay(500);
  stepper.moveTo(0);
  while (stepper.currentPosition() != 0) {
    stepper.run();
  }
  Serial.println("Stepper test complete.");
  
  // Initialize BLE scan to look for the sensing device
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  // If no BLE device is found advertising our service, blink LED and show test message.
  if (!bleDeviceFound) {
    digitalWrite(LED_PIN, (millis() / 500) % 2 == 0 ? HIGH : LOW);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No BLE");
    display.println("device found");
    display.display();
  } else {
    digitalWrite(LED_PIN, HIGH);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BLE Device");
    display.println("Found!");
    display.display();
  }
  
  delay(1000);
}
