// === Motor Driver Pins ===
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define ENA 10
#define ENB 11

// === Components ===
#define SERVO_PIN 9
#define RELAY_PIN 8
#define FLAME_SENSOR 2
#define MQ2_SENSOR A0
#define BUZZER 3

#include <Servo.h>
Servo myServo;

// === Variables ===
bool pumpRunning = false;
unsigned long pumpStartTime = 0;

int flameDetected = 0;
int gasValue = 0;

// === Thresholds ===
int gasThreshold = 400;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(MQ2_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER, LOW);

  myServo.attach(SERVO_PIN);
  myServo.write(90);

  Serial.begin(9600);
}

void loop() {
  flameDetected = digitalRead(FLAME_SENSOR);
  gasValue = analogRead(MQ2_SENSOR);

  Serial.print("Flame: ");
  Serial.print(flameDetected);
  Serial.print(" Gas: ");
  Serial.println(gasValue);

  // === GAS DETECTION ===
  if (gasValue > gasThreshold) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // === FIRE DETECTION ===
  if (flameDetected == LOW) {
    stopMotors();

    // Buzzer ON when fire detected
    digitalWrite(BUZZER, HIGH);

    // Servo scanning
    for (int pos = 60; pos <= 120; pos += 5) {
      myServo.write(pos);
      delay(50);
    }
    for (int pos = 120; pos >= 60; pos -= 5) {
      myServo.write(pos);
      delay(50);
    }

    if (!pumpRunning) {
      digitalWrite(RELAY_PIN, LOW);
      pumpRunning = true;
      pumpStartTime = millis();
      Serial.println("Fire detected - Pump ON");
    }

  } else {
    // No fire
    myServo.write(90); // center
    searchFire();
  }

  if (pumpRunning && millis() - pumpStartTime >= 3000) {
    digitalWrite(RELAY_PIN, HIGH);
    pumpRunning = false;
    Serial.println("Pump OFF after 3 seconds");
  }

  delay(200);
}

// === SEARCH MODE ===
void searchFire() {
  digitalWrite(RELAY_PIN, HIGH);

  analogWrite(ENA, 180);
  analogWrite(ENB, 180);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  Serial.println("Searching for fire...");
}

// === STOP ===
void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}