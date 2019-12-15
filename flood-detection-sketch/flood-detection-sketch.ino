/*
  Ultrasonic Sensor HC-SR04 and Arduino Tutorial

  by Dejan Nedelkovski,
  www.HowToMechatronics.com

*/
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

SoftwareSerial gsmSerial(9, 10);

// defines pins numbers
const int trigPin = 6;
const int echoPin = 7;
// defines variables
long duration;
int distance;
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  gsmSerial.begin(9600);
  Serial.begin(9600); // Starts the serial communication  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();

  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.println("IO test");

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);


  gsmSerial.println("AT+CMGF=1");
  //Sets the GSM Module in Text Mode
  delay(500);
  
  Serial.println("Sending text message.");
  gsmSerial.println("AT+CMGS=\"+639488587638\"\r");
  delay(1000);
  gsmSerial.println("Hello world!");
  // The SMS text you want to send delay(100);
  gsmSerial.println((char)26);
  delay(1000);
  Serial.println("Message sent.");
}
void printDisplay(String a) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Sensor Test");
  display.println(a);
  display.setCursor(0, 0);
  display.display(); // actually display all of the above

}
void loop() {
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
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  String m = "Distance: ";
  m += distance;
  m += "cm";
  printDisplay(m);
}
