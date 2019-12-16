/*
  Ultrasonic Sensor with GSM Flood Detection
  For DOSCST Thesis Project
  #bumblebee
*/
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
#include <ListLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
//GSM pins (TX, RX)
SoftwareSerial gsmSerial(9, 10);
List<char*> numbers;
// Ultrasonic sensor pins
const int trigPin = 6;
const int echoPin = 7;
// Define event tick
SimpleTimer timer;
long scanInterval = 1000;//millis
int distance;
int duration;
String readValue;

void setup() {
  Serial.println("Setting up..");
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  gsmSerial.begin(9600);
  Serial.begin(9600); // Starts the serial communication  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  timer.setInterval(scanInterval, checkDistanceValue);

  initDisplay();
  initGsm();
  //The first number added here is considered as the admin number
  numbers.Add("+639488587638");
}

void loop() {
  scanDistance();
  timer.run();
}

void initGsm() {
  gsmSerial.println("AT+CGPSPWR=1");
  delay(100);
  gsmSerial.println("ATS0=1");

}

void initDisplay() {
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(500);
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
}

void printDisplay(String a) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Sensor Test");
  display.println(a);
  display.println("Read Val: "+readValue);
  display.setCursor(0, 0);
  display.display(); // actually display all of the above

}
void checkDistanceValue(){
  readValue = distance;
  }

int  scan() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  return duration * 0.034 / 2;

}

void scanDistance() {
  distance = scan();
  // Prints the distance on the Serial Monitor
  String m = "Distance: ";
  m += distance;
  m += "cm";
  printDisplay(m);
}
