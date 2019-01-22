/*
  Repeating Wifi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WifiWebClientRepeating
 This code is in the public domain.
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Servo.h>

Servo oneServo;
Servo leftServo;
Servo rightServo;

//#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "*****";        // your network SSID (name)
char pass[] = "*****";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "infinityloops.xyz";
//IPAddress server(64,131,82,241);

HttpClient http(client, server);
const char kPathOne[] = "/hirota/getSleepHour.php";
const char kPathTwo[] = "/hirota/getSleep.php";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 15L * 1000L; // delay between updates, in milliseconds

String okOrNot;
String responseNum;
int counter = 0;

int okInt;
int responseInt;

void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
  
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);

  }

  // you're connected now, so print out the status:
  printWifiStatus();


}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  responseNum = "";
  responseInt = 0;

  okOrNot = "";
  okInt = 0;

  int err =0;

  if (millis() - lastConnectionTime > postingInterval) {

    err = http.get(kPathOne);
    if (err == 0)
    {
      Serial.println("startedRequest ok");
      counter = 0;
      err = http.responseStatusCode();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               (!http.endOfBodyReached()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();    
                // Print out this character
                Serial.println("ok or not from server is:");
                Serial.println(c);

                okOrNot += c;
//                Serial.println("responseNum is:");
//                Serial.println(responseNum);
               
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                Serial.println("pause to delay");
                delay(kNetworkDelay);
            }
        }
      }
      else
      {    
        Serial.print("Getting response failed: ");
        Serial.println(err);
      }
      okInt = okOrNot.toInt();
    }
    else
    {
      Serial.print("Connect failed: ");
      Serial.println(err);
    }
    http.stop();
    Serial.println("okInt:");
    Serial.println(okInt);
    

    err = http.get(kPathTwo);
    if (err == 0)
    {
      Serial.println("startedRequest ok");
      counter = 0;
      err = http.responseStatusCode();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               (!http.endOfBodyReached()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();    
                // Print out this character
//                Serial.println("http.read()->c is:");
//                Serial.println(c);

                responseNum += c;
//                Serial.println("responseNum is:");
//                Serial.println(responseNum);
               
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                Serial.println("pause to delay");
                delay(kNetworkDelay);
            }
        }
      }
      else
      {    
        Serial.print("Getting response failed: ");
        Serial.println(err);
      }
      responseInt = responseNum.toInt();
    }
    else
    {
      Serial.print("Connect failed: ");
      Serial.println(err);
    }
    http.stop();

    Serial.println("outside of http request loop. responseInt is:");
    Serial.println(responseInt);

    if (responseInt >= 0 && okInt == 1)
    {
      flowerServo(responseInt);        
      delay(1000);
    } else if (okInt == 0)
    {
      flowerServo(0);        
      delay(1000);      
    }

    lastConnectionTime = millis();

  }

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");


}


void flowerServo(int responseInt) {

  oneServo.attach(13);
  leftServo.attach(12);
  rightServo.attach(8);

  delay(1000);

  if ( responseInt > 510 ) //8.5hr- 寝すぎ
  {
    Serial.println("SERVO: >510");

    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);

    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);

    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);

  }
  else if ( responseInt > 450 && responseInt <= 510) //7.5-8.5hr まあオッケー
  {
    Serial.println("SERVO: >450");
   oneServo.write(80);
   leftServo.write(120);
   rightServo.write(70);
   delay(600);
   oneServo.write(90);
   leftServo.write(140);
   rightServo.write(50);
   delay(600);

   oneServo.write(80);
   leftServo.write(120);
   rightServo.write(70);
   delay(600);
   oneServo.write(90);
   leftServo.write(140);
   rightServo.write(50);
   delay(600);

      oneServo.write(80);
   leftServo.write(120);
   rightServo.write(70);
   delay(600);
   oneServo.write(90);
   leftServo.write(140);
   rightServo.write(50);
   delay(600);
   
  }
  else if ( responseInt > 390 && responseInt <= 450 ) //6.5-7.5hr ベスト
  {
    Serial.println("SERVO: >390");

  oneServo.write(70);
  leftServo.write(110);
  rightServo.write(80);
  delay(300);
  oneServo.write(100);
  leftServo.write(140);
  rightServo.write(50);
  delay(300);

  oneServo.write(70);
  leftServo.write(110);
  rightServo.write(80);
  delay(300);
  oneServo.write(100);
  leftServo.write(140);
  rightServo.write(50);
  delay(300);

  oneServo.write(70);
  leftServo.write(110);
  rightServo.write(80);
  delay(300);
  oneServo.write(100);
  leftServo.write(140);
  rightServo.write(50);
  delay(300);

  }
  else if ( responseInt > 330  && responseInt <= 590) //5.5-6.5hr まあオッケー
  {
    Serial.println("SERVO: >350");

  oneServo.write(80);
  leftServo.write(120);
  rightServo.write(70);
  delay(600);
  oneServo.write(90);
  leftServo.write(140);
  rightServo.write(50);
  delay(600);

  oneServo.write(80);
  leftServo.write(120);
  rightServo.write(70);
  delay(600);
  oneServo.write(90);
  leftServo.write(140);
  rightServo.write(50);
  delay(600);

  oneServo.write(80);
  leftServo.write(120);
  rightServo.write(70);
  delay(600);
  oneServo.write(90);
  leftServo.write(140);
  rightServo.write(50);
  delay(600);

  }
  else if ( responseInt > 180  && responseInt <= 330) //3.0-5.5hr 少ない
  {
    Serial.println("SERVO: >180");
    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);

    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);

    oneServo.write(100);
    leftServo.write(135);
    rightServo.write(55);
    delay(1200);
    leftServo.write(140);
    rightServo.write(50);
    delay(1200);


  } else { //0.0-3.0hr 死ぬよ

    Serial.println("SERVO: >0");

    oneServo.write(120);
    leftServo.write(140);
    rightServo.write(50);
  }

  delay(1000);
  
    oneServo.detach();
    leftServo.detach();
    rightServo.detach();

}
