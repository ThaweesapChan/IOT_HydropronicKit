#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <Adafruit_ADS1015.h>

// กำหนดข้อมูลสำหรับ LED
const int LED1_PIN = D1;  // กำหนดขา D1 
const int LED2_PIN = D2;  // กำหนดขา D2 
const int RESET_BUTTON_PIN = D3;  // กำหนดขา D3 
const int WATER_PUMP_PIN_2 = D5; // กำหนดขา D5  (Pump 2)
const int WATER_PUMP_PIN_3 = D6; // กำหนดขา D6 (Pump 3)
const int WATER_PUMP_PIN_4 = D7; // กำหนดขา D7  (Pump 4)
const int WATER_PUMP_PIN_5 = D9; // กำหนดขา D9  (Pump 5)
const int WATER_PUMP_PIN_6 = D10; // กำหนดขา D10  (Pump 6)
const int WATER_PUMP_PIN_10 = D8; // กำหนดขา D8  (Pump 10)

// กำหนดข้อมูลสำหรับเซ็นเซอร์ DHT11
#define DHT_PIN D4  // กำหนดขา D4 
#define DHT_TYPE DHT11

// กำหนดข้อมูลสำหรับ Blynk
char auth[] = "Your_Blynk_Auth_Token"; // ใส่ Auth Token
char ssid[] = "Your_SSID"; // ใส่ชื่อ Wi-Fi SSID 
char pass[] = "Your_Password"; // ใส่รหัส Wi-Fi 

DHT_Unified dht(DHT_PIN, DHT_TYPE);
Adafruit_ADS1115 ads; // ใช้งานรีเลย์ EC ผ่าน ADS1115

BlynkTimer timer;
int ecValue = 0; // ค่า EC
int minEcValue = 1000; // ค่า EC ต่ำสุดที่กำหนดจาก Blynk
int maxEcValue = 2000; // ค่า EC สูงสุดที่กำหนดจาก Blynk

int phValue = 0; // ค่า pH
int minPhValue = 4; // ค่า pH ต่ำสุดที่กำหนดจาก Blynk
int maxPhValue = 7; // ค่า pH สูงสุดที่กำหนดจาก Blynk

bool isPump2On = false; // สถานะปั้ม 2
bool isPump3On = false; // สถานะปั้ม 3
bool isPump4On = false; // สถานะปั้ม 4
bool isPump5On = false; // สถานะปั้ม 5
bool isPump6On = false; // สถานะปั้ม 6
bool isPump10On = false; // สถานะปั้ม 10

void setup() {
  // กำหนดขา LED เป็น OUTPUT
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // กำหนดขา Reset Button เป็น INPUT
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  // เริ่ม Serial Monitor
  Serial.begin(115200);

  // กำหนดเซ็นเซอร์ DHT11
  dht.begin();

  // เชื่อมต่อกับ Wi-Fi
  Blynk.begin(auth, ssid, pass);

  // กำหนดเซ็นเซอร์ ADS1115
  ads.begin();

  // ระบุฟังก์ชันที่เรียกเมื่อมีการอัปเดตข้อมูลจากเซ็นเซอร์
  timer.setInterval(10000L, sendSensorData); // ส่งข้อมูลทุก 10 วินาที
  timer.setInterval(600000L, checkAndControlPump); // ตรวจสอบและควบคุมปั้มทุก 10 นาที

  // ตรวจสอบการกดปุ่ม Reset เพื่อรีเซ็ตค่า
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    WiFiManager wifiManager;
    wifiManager.resetSettings();  // รีเซ็ตค่าใน WiFiManager
    ESP.reset();  // รีเซ็ต ESP8266
  }
}

void loop() {
  // ใช้ Blynk.run() เพื่อให้ Blynk ทำงาน
  Blynk.run();

  // ใช้ timer.run() เพื่อให้ BlynkTimer ทำงาน
  timer.run();
}

void sendSensorData() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;

  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;

  Blynk.virtualWrite(V5, temperature); // ส่งข้อมูลอุณหภูมิไปยังแอพ Blynk
  Blynk.virtualWrite(V6, humidity); // ส่งข้อมูลความชื้นไปยังแอพ Blynk

  // อ่านค่า EC จากเซ็นเซอร์
  ecValue = readEcValue();

  Blynk.virtualWrite(V7, ecValue); // ส่งข้อมูลค่า EC ไปยังแอพ Blynk

  // อ่านค่า pH จากเซ็นเซอร์
  phValue = readPhValue();

  Blynk.virtualWrite(V8, phValue); // ส่งข้อมูลค่า pH ไปยังแอพ Blynk
}

int readEcValue() {
  // อ่านค่า EC จากเซ็นเซอร์ (ต้องปรับโค้ดให้ตรงกับเซ็นเซอร์ EC ที่ใช้)
  int sensorValue = ads.readADC_SingleEnded(0); // อ่านค่า ADC ของเซ็นเซอร์ EC
  // นำค่า ADC มาคำนวณเป็นค่า EC ตามความต้องการ
  
  return sensorValue; // ค่า EC ที่อ่านได้
}

int readPhValue() {
  // อ่านค่า pH จากเซ็นเซอร์ (ต้องปรับโค้ดให้ตรงกับเซ็นเซอร์ pH ที่ใช้)
  int sensorValue = analogRead(A0); // อ่านค่า pH จากขา A0
  // นำค่าอ่านมาคำนวณเป็นค่า pH ตามความต้องการ

  return sensorValue; // ค่า pH ที่อ่านได้
}

void checkAndControlPump() {
  // ตรวจสอบค่า EC และควบคุมปั้มตามค่าที่กำหนด
  if (ecValue > maxEcValue) {
    // EC สูงกว่าค่าสูงสุดที่กำหนดจาก Blynk
    controlPump(WATER_PUMP_PIN_2, true, 60000); // เปิดปั้ม 2 เป็นเวลา 1 นาที (60,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_10, true, 600000); // เปิดปั้ม 10 เป็นเวลา 10 นาที (600,000 มิลลิวินาที)
  } else if (ecValue < minEcValue) {
    // EC ต่ำกว่าค่าต่ำสุดที่กำหนดจาก Blynk
    controlPump(WATER_PUMP_PIN_3, true, 60000); // เปิดปั้ม 3 เป็นเวลา 1 นาที (60,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_10, true, 180000); // เปิดปั้ม 10 เป็นเวลา 3 นาที (180,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_4, true, 60000); // เปิดปั้ม 4 เป็นเวลา 1 นาที (60,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_10, true, 600000); // เปิดปั้ม 10 เป็นเวลา 10 นาที (600,000 มิลลิวินาที)
  } else {
    // EC อยู่ในช่วงที่กำหนด
    controlPump(WATER_PUMP_PIN_2, false, 0); // ปิดปั้ม 2
    controlPump(WATER_PUMP_PIN_3, false, 0); // ปิดปั้ม 3
    controlPump(WATER_PUMP_PIN_4, false, 0); // ปิดปั้ม 4
    controlPump(WATER_PUMP_PIN_10, false, 0); // ปิดปั้ม 10
  }

  // ตรวจสอบค่า pH และควบคุมปั้มตามค่าที่กำหนด
  if (phValue > maxPhValue) {
    // pH สูงกว่าค่าสูงสุดที่กำหนดจาก Blynk
    controlPump(WATER_PUMP_PIN_5, true, 30000); // เปิดปั้ม 5 เป็นเวลา 30 วินาที (30,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_10, true, 600000); // เปิดปั้ม 10 เป็นเวลา 10 นาที (600,000 มิลลิวินาที)
  } else if (phValue < minPhValue) {
    // pH ต่ำกว่าค่าต่ำสุดที่กำหนดจาก Blynk
    controlPump(WATER_PUMP_PIN_6, true, 30000); // เปิดปั้ม 6 เป็นเวลา 30 วินาที (30,000 มิลลิวินาที)
    controlPump(WATER_PUMP_PIN_10, true, 600000); // เปิดปั้ม 10 เป็นเวลา 10 นาที (600,000 มิลลิวินาที)
  } else {
    // pH อยู่ในช่วงที่กำหนด
    controlPump(WATER_PUMP_PIN_5, false, 0); // ปิดปั้ม 5
    controlPump(WATER_PUMP_PIN_6, false, 0); // ปิดปั้ม 6
  }

  // ตรวจสอบสถานะปั้มและควบคุม LED
  updatePumpStatus();
}

void controlPump(int pumpPin, bool isOn, unsigned long duration) {
  if (isOn) {
    digitalWrite(pumpPin, HIGH); // เปิดปั้ม
    delay(duration); // รอเวลาที่กำหนด
    digitalWrite(pumpPin, LOW); // ปิดปั้ม
  } else {
    digitalWrite(pumpPin, LOW); // ปิดปั้ม
  }
}

void updatePumpStatus() {
  if (isPump2On || isPump3On || isPump4On || isPump5On || isPump6On || isPump10On) {
    // มีปั้มที่เปิดอยู่
    Blynk.virtualWrite(V1, "ON"); // ส่งสถานะ ON ไปยัง Blynk
    digitalWrite(LED1_PIN, HIGH); // เปิด LED 1 (สีเขียว)
  } else {
    Blynk.virtualWrite(V1, "OFF"); // ส่งสถานะ OFF ไปยัง Blynk
    digitalWrite(LED1_PIN, LOW); // ปิด LED 1 (สีแดง)
  }

  if (isPump5On || isPump6On || isPump10On) {
    // มีปั้มที่เปิดอยู่
    Blynk.virtualWrite(V2, "ON"); // ส่งสถานะ ON ไปยัง Blynk
    digitalWrite(LED2_PIN, HIGH); // เปิด LED 2 (สีเขียว)
  } else {
    Blynk.virtualWrite(V2, "OFF"); // ส่งสถานะ OFF ไปยัง Blynk
    digitalWrite(LED2_PIN, LOW); // ปิด LED 2 (สีแดง)
  }
}

BLYNK_WRITE(V2) { // อ่านค่า Min pH จาก Blynk
  minPhValue = param.asInt();
}

BLYNK_WRITE(V3) { // อ่านค่า Max pH จาก Blynk
  maxPhValue = param.asInt();
}

BLYNK_WRITE(V4) { // อ่านค่า Min EC จาก Blynk
  minEcValue = param.asInt();
}

BLYNK_WRITE(V5) { // อ่านค่า Max EC จาก Blynk
  maxEcValue = param.asInt();
}
////////////////////////////////////////////////////////////////
