#include <Wire.h>
#include <Servo.h>
#include <Adafruit_LSM6DSOX.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Servo motors
Servo servoX, servoY, servoZ, servoW;

// Define pins for servos
const int servoXPin = 9;
const int servoYPin = 10;
const int servoZPin = 11;
const int servoWPin = 12;


// Accelerometer and gyroscope
Adafruit_LSM6DSOX lsm;

// Bluetooth
SoftwareSerial bluetooth(2, 3); // RX, TX

// LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

// PIR motion sensor
const int pirPin = 8;
bool motionDetected = false;

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {4, 5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Serial communication
  Serial.begin(9600);
  bluetooth.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  // Initialize LSM6DSOX
  if (!lsm.begin_I2C()) {
    lcd.setCursor(0, 1);
    lcd.print("LSM6DSOX Error");
    while (1); // Stop if sensor not detected
  }

  // Initialize servos
  servoX.attach(servoXPin); // Servo for X-axis
  servoY.attach(servoYPin); // Servo for Y-axis
  servoZ.attach(servoZPin);
  servoW.attach(servoWPin);
  servoX.write(90); // Neutral position
  servoY.write(90);
  servoZ.write(90);
  servoW.write(90);

  // PIR motion sensor
  pinMode(pirPin, INPUT);

  lcd.clear();
  lcd.print("System Ready");
}

void loop() {
  // Detect motion
  motionDetected = digitalRead(pirPin);
  if (motionDetected) {
    //lcd.setCursor(0, 1);
    //lcd.print("Motion Detected ");
    bluetooth.println("Motion Detected");
  }

  // Read keypad input
  char key = keypad.getKey();
  if (key) {
    //lcd.clear();
    //lcd.setCursor(0, 1);
    //lcd.print("Key: ");
    //lcd.print(key);
    bluetooth.println("Key Pressed: " + String(key));

    // Example: Manual servo control with keys
    if (key == 'A') servoX.write(servoX.read() + 10); // Move X servo
    if (key == 'B') servoX.write(servoX.read() - 10); // Move X servo

    if (key == 'C') servoY.write(servoY.read() + 10); // Move Y servo
    if (key == 'D') servoY.write(servoY.read() - 10); // Move Y servo
  }

  // Read accelerometer and gyroscope data
  sensors_event_t accel, gyro, temp;
  lsm.getEvent(&accel, &gyro, &temp);

  // Display motion data on LCD
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(accel.acceleration.x, 1);
  lcd.print(" Y:");
  lcd.print(accel.acceleration.y, 1);
  lcd.setCursor(0, 1);
  lcd.print("Z:");
  lcd.print(accel.acceleration.z, 1);

  // Use accelerometer data to adjust servo position
  int servoXPos = map(accel.acceleration.x, -9, 9, 0, 180);
  int servoYPos = map(accel.acceleration.y, -9, 9, 0, 180);
  int servoZPos = map(accel.acceleration.z, -9, 9, 0, 180);
  servoX.write(servoXPos);
  servoY.write(servoYPos);
  servoZ.write(servoZPos);

  delay(100);
}
