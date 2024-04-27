#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ThingSpeak.h>

#define wifi_ssid "Note9"
#define wifi_password "nidhi123"
#define api_key "WHGKAW3AM9IFS86V"

int mq6pin= D3;// digital pin for MQ-6 sensor
int mq135pin= A0; // analog pin for MQ135 sensor
int buzzerpin= D5; // digital pin for the buzzer
Adafruit_BMP280 bmp; // create BMP280 object
int mq6val;
int mq135val;
String apiKey = "LMPJ1FPGMZ3WYLUP";
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup() {

  Serial.begin(9600);
  // connect to WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // initialize BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP280 sensor initialized");
  // Set buzzer pin as OUTPUT
  pinMode(buzzerpin, OUTPUT);
  pinMode(mq6pin, INPUT);
  pinMode(mq135pin, INPUT);
}

void loop() {
  // read values from MQ-6 sensor
  int mq6val = digitalRead(mq6pin);

  // read values from MQ135 sensor
  int mq135val = analogRead(mq135pin);

  // read values from BMP280 sensor
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;

  // print sensor values
  Serial.print("MQ-6: ");
  Serial.println(mq6val);
  Serial.print(" MQ135: ");
  Serial.println(mq135val);
  Serial.print(" Temperature: ");
  Serial.println(temp);
  Serial.print(" Pressure: ");
  Serial.println(pressure);

  // Check if MQ-6 value is HIGH (1), indicating the gas is detected
  if (mq6val == LOW) {
    // Turn on the buzzer
    digitalWrite(buzzerpin, HIGH);
    delay(1000); // Buzzer ON time

    // Turn off the buzzer
    digitalWrite(buzzerpin, LOW);
  }

  if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(mq6val);
        postStr +="&field2=";
        postStr += String(mq135val);
        postStr +="&field3=";
        postStr += String(temp);
        postStr +="&field4=";
        postStr += String(pressure);
        postStr += "\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr); 
    }   
    client.stop(); 

  // send data to ThingSpeak
  // ThingSpeak.setField(1, mq6val);
  // ThingSpeak.setField(2, mq135val);
  // // ThingSpeak.setField(3, temp);
  // // ThingSpeak.setField(4, pressure);

  // int status = ThingSpeak.writeFields(2143016, api_key); 

  // if (status == 200) {
  //   Serial.println("Data sent to ThingSpeak");
  // } else {
  //   Serial.print("Error sending data to ThingSpeak. Status code: ");
  //   Serial.println(status);
  // }

  delay(2000); // wait for 10 seconds before sending the next set of data
}