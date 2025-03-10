#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <AccelStepper.h>

// 函数原型声明
void updateDisplay(float humidity);
void updateStepper(float humidity);
void generateHumidity();

// 硬件配置
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define LED_PIN       D10
#define STEPPER_MAX   315
#define UPDATE_INTERVAL 6000 // 6秒更新

// 全局对象初始化
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
AccelStepper stepper(AccelStepper::FULL4WIRE, A0, A1, A2, A3);

// 波动参数
unsigned long lastUpdate = 0;
const float BASE_HUMIDITY = 33.45f;       // 基准值 (33.2+33.7)/2
const float SINE_AMPLITUDE = 0.25f;       // 正弦振幅 (±0.25)
const float NOISE_AMPLITUDE = 0.3f;       // 随机噪声幅度

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // OLED初始化
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(1);
  }
  display.setTextColor(SSD1306_WHITE);

  // 步进电机配置
  stepper.setMaxSpeed(800);
  stepper.setAcceleration(400);
  stepper.moveTo(0);

  // 启动显示
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("System Initialized");
  display.display();
  delay(800);
}

void loop() {
  generateHumidity();
  stepper.run();
}

void generateHumidity() {
  if(millis() - lastUpdate >= UPDATE_INTERVAL) {
    // 正弦波动基础值 (周期2分钟)
    float timeFactor = millis() / 120000.0 * TWO_PI; // 120秒周期
    float sineWave = sin(timeFactor) * SINE_AMPLITUDE;
    
    // 随机噪声（±0.3%）
    float randomNoise = random(-30, 30)/100.0f; 
    
    // 合成最终湿度值
    float currentHumidity = BASE_HUMIDITY + sineWave + randomNoise;
    
    // 限制最终范围 (33.2-33.7 ±0.3 = 32.9-34.0)
    currentHumidity = constrain(currentHumidity, 32.9f, 34.0f);
    
    updateDisplay(currentHumidity);
    updateStepper(currentHumidity);
    
    lastUpdate = millis();
  }
}

void updateDisplay(float humidity) {
  // 屏幕闪烁效果
  display.clearDisplay();
  display.display();
  delay(80);  // 增加黑屏时间到80ms

  // 显示新数据
  display.clearDisplay();
  
  // 主显示区
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print(humidity, 1);  // 显示1位小数
  display.println("% RH");

  // 状态信息
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Status:"); 
  display.setCursor(0, 40);
  
  // 自动颜色警示（根据实际范围）
  if(humidity >= 30 && humidity <= 50) {
    display.println("Comfort level OK");
  } else {
    display.println("Too dry!");  // 超出主要范围时显示
  }

  display.display();
}

void updateStepper(float humidity) {
  // 带边界保护的映射计算
  float clampedHumidity = constrain(humidity, 0, 100);
  int targetPos = map(clampedHumidity * 10, 0, 1000, 0, STEPPER_MAX);
  stepper.moveTo(targetPos);
  
  // 快速定位
  if(stepper.distanceToGo() != 0) {
    stepper.runToPosition();
  }
}
