#include "DHT.h"

// -------- DHT SETUP --------
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// -------- PIN DEFINITIONS --------
int ldrPin = A0;
int waterPin = A1;
int pirPin = 3;

int ledLight = 13;
int acPin = 12;
int buzzer = 8;
int relayPin = 7;

// -------- THRESHOLDS --------
int lightThreshold = 400;
int tempThreshold = 30;
int fireTemp = 45;
int waterLow = 300;
int waterHigh = 700;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(pirPin, INPUT);
  pinMode(ledLight, OUTPUT);
  pinMode(acPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
}

void loop() {

  // -------- SENSOR READINGS --------
  int lightValue = analogRead(ldrPin);
  int waterLevel = analogRead(waterPin);
  int motion = digitalRead(pirPin);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // -------- ERROR CHECK --------
  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ DHT Sensor Error!");
    return;
  }

  // -------- SERIAL DASHBOARD --------
  Serial.println("------ SMART HOME SYSTEM ------");
  Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(hum); Serial.println(" %");
  Serial.print("Light: "); Serial.println(lightValue);
  Serial.print("Water Level: "); Serial.println(waterLevel);
  Serial.print("Person: "); Serial.println(motion ? "YES" : "NO");

  // -------- 🔥 FIRE DETECTION --------
  if (temp > fireTemp) {
    tone(buzzer, 1000);   // buzzer ON
    Serial.println("🔥 FIRE ALERT!");
  } else {
    noTone(buzzer);       // buzzer OFF
  }

  // -------- 👤 OCCUPANCY CONTROL --------
  if (motion == LOW) {
    digitalWrite(ledLight, LOW);
    digitalWrite(acPin, LOW);
    Serial.println("No Person → Systems OFF");
  } 
  else {
    // -------- 💡 SMART LIGHT --------
    if (lightValue < lightThreshold) {
      digitalWrite(ledLight, HIGH);
      Serial.println("Light ON (Dark)");
    } else {
      digitalWrite(ledLight, LOW);
      Serial.println("Light OFF (Bright)");
    }

    // -------- 🌡️ SMART AC --------
    if (temp > tempThreshold) {
      digitalWrite(acPin, HIGH);
      Serial.println("AC ON");
    } else {
      digitalWrite(acPin, LOW);
      Serial.println("AC OFF");
    }
  }

  // -------- 🚰 WATER TANK CONTROL --------
  if (waterLevel < waterLow) {
    digitalWrite(relayPin, HIGH);
    Serial.println("Motor ON (Tank Low)");
  } 
  else if (waterLevel > waterHigh) {
    digitalWrite(relayPin, LOW);
    Serial.println("Motor OFF (Tank Full)");
  }

  Serial.println("-------------------------------\n");

  delay(2000);
}