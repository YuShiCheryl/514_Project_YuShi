#include <Wire.h>
#include <Adafruit_BME280.h>

#define LED_PIN 9  // Corrected LED pin
#define I2C_ADDRESS 0x76  // Default address for most BME280 sensors

Adafruit_BME280 bme;

void i2cScanner() {
  Serial.println("Scanning I2C bus...");
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(address, HEX);
    }
  }
  Serial.println("I2C scan complete.");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // Turn LED on at startup

  Wire.begin();
  delay(1000);

  // Scan the I2C bus to verify connection
  i2cScanner();

  // Initialize BME280
  if (!bme.begin(I2C_ADDRESS)) {
    Serial.println("⚠️ BME280 sensor not found! Check wiring and I2C address.");
    while (1) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200); // Rapid LED blinking indicates failure
    }
  } 

  Serial.println("✅ BME280 detected and initialized successfully!");
}

void loop() {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;  // Convert Pa to hPa

  Serial.print("🌡️ Temp: ");
  Serial.print(temperature);
  Serial.print(" °C,  💧 Humidity: ");
  Serial.print(humidity);
  Serial.print(" %,  ⬇️ Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  // Blink LED every read cycle
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  delay(5000);  // Read sensor every 5 seconds
}
