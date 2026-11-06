#include <Servo.h>
#include <SoftwareSerial.h>

// Ultrasonic sensor pins
const int trigPin = 10;
const int echoPin = 11;

// Servo pin
const int servoPin = 12;

// GSM module pins (connect TX of SIM800L to Arduino pin 7, RX to pin 8)
SoftwareSerial gsm(7, 8); // RX, TX

// Variables
long duration;
int distance;
Servo myServo;

// Phone number (change to your number)
String phoneNumber = "+919026730541"; // <-- Replace with your number

// Threshold distance to trigger alert
const int dangerDistance = 20;

// Flags to prevent repeated alerts
bool alertSent = false;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  myServo.attach(servoPin);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Radar System with GSM Initialized...");
  delay(2000);

  // Optional: test GSM module
  gsm.println("AT");
  delay(1000);
  gsm.println("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
}

void loop() {
  for (int i = 15; i <= 165; i++) {
    myServo.write(i);
    delay(30);
    distance = calculateDistance();

    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");

    // Check for object within danger distance
    if (distance < dangerDistance && distance > 0) {
      if (!alertSent) {
        sendAlert();
        alertSent = true; // prevent repeated alerts
      }
    } else {
      alertSent = false; // reset flag when object moves away
    }
  }

  for (int i = 165; i > 15; i--) {
    myServo.write(i);
    delay(30);
    distance = calculateDistance();

    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");

    if (distance < dangerDistance && distance > 0) {
      if (!alertSent) {
        sendAlert();
        alertSent = true;
      }
    } else {
      alertSent = false;
    }
  }
}

// Function to calculate distance
int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

// Function to send SMS and make a call
void sendAlert() {
  Serial.println("Object detected! Sending alert...");

  // Send SMS
  gsm.println("AT+CMGF=1"); 
  delay(1000);
  gsm.print("AT+CMGS=\"");
  gsm.print(phoneNumber);
  gsm.println("\"");
  delay(1000);
  gsm.println("⚠️ ALERT! Object detected near the radar system.");
  gsm.println("Distance less than threshold! Check immediately.");
  gsm.write(26); // CTRL+Z to send SMS
  delay(5000);

  // Make a call
  gsm.print("ATD");
  gsm.print(phoneNumber);
  gsm.println(";");
  Serial.println("Calling...");
  delay(15000); // wait 15 seconds before hanging up
  gsm.println("ATH"); // hang up call
}