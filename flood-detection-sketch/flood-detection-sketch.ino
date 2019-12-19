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

SoftwareSerial gsmSerial(9, 10);//TX, RX

List<char*> numbers;
// Ultrasonic sensor pins
const int trigPin = 6;
const int echoPin = 7;
// Define event tick
SimpleTimer timer;
long scanInterval = 60000;//millis
int duration;

int distance;
int maxDistance = 50;//cm
int readValue;
String serialMessage;
String adminNum= "+639488587638";

void setup() {
  Serial.begin(9600); Serial.begin(9600);   // Setting the baud rate of Serial Monitor (Arduino)
  gsmSerial.begin(9600);   // Setting the baud rate of GSM Module
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  //Ultrasonic setup
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  timer.setInterval(scanInterval, intervalCheck);
  display.display();

  //The first number added here is considered as the admin number
  numbers.Add("+639488587638");

  delay(1000);
  initDisplay();
  //gsmSerial.println("AT+CGPSPWR=1");
  //  gsmSerial.println("ATS0=1");
  if (!commandAndRead("AT+CGPSPWR=1", 1000, "OK")) {
    serialMessage = "Device is turned off.";
  } else {
    serialMessage = "Device is turned on.";
  }
  commandAndRead("ATS0=1", 1000, "OK");
  commandAndRead("AT+CMGF=1", 1000, "OK");
  commandAndRead("AT+CNMI=2,2,0,0,0", 1000, "OK");
}

void loop() {
  scanDistance();
  printWholeInfo();
  timer.run();
  if (Serial.available() > 0) {
    switch (Serial.read())
    {
      case '$':
        internalCommand(Serial.readString());
        break;
      case 's':
        int percentage = ((distance * 1.0) / (maxDistance * 1.0)) * 100;
        if (percentage >= 75) {
          sendSMS( "Alert! water level is very high.(75% or more)", adminNum);
        } else if (percentage >= 50 ) {
          sendSMS( "Alert! water level is high.(50%)", adminNum);
        } else if (percentage >= 25) {
          sendSMS("Warning, water level has increased to 25%.", adminNum);
        }
        break;
      case 'r':
        commandAndRead("AT+CMGR=1 ", 1000, "OK");
        break;
    }
  }
  if (gsmSerial.available() > 0) {
    String a = gsmSerial.readString();
    serialMessage = a;
    Serial.println(a);
  }
}
void internalCommand(String c) {
  if (c.indexOf("maxdst=") >= 0) {
    String x = c.substring(7);
    x.trim();
    Serial.println(x);
    maxDistance = x.toInt();
    Serial.println("Max distance changed into " + x + "cm.");
    //}
  } else if ("addnum=") {
    String x = c.substring(7);
    x.trim();
    char buff[x.length()];
    x.toCharArray(buff, x.length());
    Serial.println(x);
    numbers.Add(buff);
    Serial.println("Number " + x + " has been added to subscribers list.");
  }
}

void readCommand(const char*[]) {}

void intervalCheck() {
  readValue = distance;
}

float scan() {
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
}

void initGsm() {
}

void printWholeDisplay(String a) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(a);
  display.setCursor(0, 0);
  display.display(); // actually display all of the above
}

void printWholeInfo() {
  int nearest = ((distance * 1.0) / (maxDistance * 1.0)) * 100;
  String s = "Dist: ";
  s += distance;
  s += "cm ";
  s += nearest;
  s += "%\n\r";
  s += serialMessage;
  printWholeDisplay(s);
}

void initDisplay() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
}

void sendSMS(String msg, String pn) {
  Serial.println("Sending message");
  gsmSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gsmSerial.println("AT+CMGS=\"" + pn + "\"");
  delay(500);
  gsmSerial.println(msg);// The SMS text you want to send
  delay(100);
  gsmSerial.println((char)26);// ASCII code of CTRL+Z
  delay(500);
}

bool commandAndRead(String command, int maxTime, char readReplay[]) {
  int countTimeCommand = 0;
  int countTrueCommand = 0;
  boolean found = false;
  Serial.println("Executing: " + command);
  gsmSerial.print(command);
  while (countTimeCommand < (maxTime * 1)) {
    if (gsmSerial.available() > 0) {
      String a = gsmSerial.readString();
      if (a.indexOf(readReplay)) //ok
      {
        found = true;
        break;
      }
    }
    delay(1);
    countTimeCommand++;
  }
  Serial.println(found ? "Okay" : "Failed");
  return found;
}
