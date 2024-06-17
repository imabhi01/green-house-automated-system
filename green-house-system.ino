#define BLYNK_TEMPLATE_ID "TMPL57y_2_Fcn"
#define BLYNK_TEMPLATE_NAME "IOT  WEEK 9"

#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <DHT.h>
#include<ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

#define dht_pin 19
#define moisture_pin 32
#define relay_pin 5
#define windowMotorPin 23
#define watterMotorPin 16

// Thresholds
const float temperatureThreshold = 25.0; // Threshold for temperatrue  (in percentage)
const float humidityThreshold = 70.0; // Threshold for humidity (in percentage)
const float maxMoistureThreshold = 60.0; // Max Threshold for soil moisture (in percentage)
const float minMoistureThreshold = 20.0; // Min Threshold for soil moisture (in percentage)

boolean isWindowOpen      = false; // Green House Window is closed initially
boolean isWaterpumpOpen  = false; // Servo Motor is closed initally for watering

DHT my_dht(dht_pin, DHT22);
Servo windowServo;
Servo waterServo;

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

char auth[] = "TP2XDpV8dzuQEkbOfWTvgZm4_E8Jw9Sr";
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

void setup() {
  Serial.begin(9600);
  windowServo.attach(windowMotorPin);
  waterServo.attach(watterMotorPin);
  LCD.init();
  LCD.backlight();
  connectToWifi(ssid, pass);  
  Blynk.begin(auth, ssid, pass);       
}

void loop() { 
  delay(1000);
  int moistureValue = analogRead(moisture_pin);

  // Normalizing the value to a percentage (0 to 100)
  float moisture = map(moistureValue, 0, 4095, 0, 100);
  float temperature = my_dht.readTemperature();
  float humidity = my_dht.readHumidity();
  
  Serial.println("Temperature: " + String(temperature));
  Serial.println("Humidity: " + String(humidity));
  Serial.println("Moisture: " + String(moisture));

  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, moisture);

  LCD.clear();
  displayOnScreen(temperature, humidity, moisture);

  // Condition to control the relay and servo motors
  if (moisture < minMoistureThreshold) {
    digitalWrite(relay_pin, HIGH); // Turn the relay on
    openWaterPump(isWaterpumpOpen);
    isWaterpumpOpen = true;
    Serial.println("Relay turned on and opened water pump servo due to low moisture.");
  } else if(moisture > maxMoistureThreshold){
    closeWaterPump(isWaterpumpOpen);
    isWaterpumpOpen = false;
    Serial.println("Water pump closed.");
  }else{
    closeWaterPump(isWaterpumpOpen);
    isWaterpumpOpen = false;
    Serial.println("Water pump closed.");
  }

  if (humidity > humidityThreshold || temperature > temperatureThreshold) {
    digitalWrite(relay_pin, HIGH); // Turn the relay on
    openWindow(isWindowOpen);
    isWindowOpen = true;
    Serial.println("Relay turned on and window opened due to hight humidity or high temperature");
  } else {
    closeWindow(isWindowOpen);
    isWindowOpen = false;
    Serial.println("Window closed due to sufficient humidity and normal temperature.");
  }

  delay(3000);
}

// Function to connect to Wifi
void connectToWifi(const char* ssid, const char* pass){
  WiFi.begin(ssid, pass, 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("WiFi not connected.");
  }
  LCD.println("WiFi Connected!");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

// Function to show the read sensor data to the LCD display
void displayOnScreen(float temperature, float humidity, int moisture){
  LCD.println("Temperature: " + String(temperature) + " C");
  LCD.setCursor(0, 1);
  LCD.println("Humidity: " + String(humidity) + "%");
  LCD.setCursor(0, 2);
  LCD.println("Soil Moisture: " + String(moisture) + "%");
}

// Function to open the Window
void openWindow(boolean isWindowOpen)
{
  if(isWindowOpen == false){
    for(int i=10; i<=180; i=i+5)
    {
      windowServo.write(i);
      delay(50);
    }
  }
}
 
// Function to close the Window
void closeWindow(boolean isWindowOpen)
{
  if(isWindowOpen == true){
    for(int i=180; i>=0; i=i-5) 
    {
      windowServo.write(i);
      delay(50);
    }
  }
}

// Function to open the Watering of plants
void openWaterPump(boolean isWaterpumpOpen)
{
  if(isWaterpumpOpen == false){
    for(int i=10; i<=180; i=i+5)
    {
      waterServo.write(i);
      delay(50);
    }
  }
}

// Function to close the Watering of plants
void closeWaterPump(boolean isWaterpumpOpen)
{
  if(isWaterpumpOpen == true){
    for(int i=180; i>=0; i=i-5) 
    {
      waterServo.write(i);
      delay(50);
    }
  }
}

// Function to Open and close the Window from Blynk cloud
BLYNK_WRITE(V0) {
  int value = param.asInt(); // Get value as integer
  if (value == 1) {
    openWindow(false);
    Serial.println("Opening Window!");
  } else {
    closeWindow(true);
    Serial.println("Closing Window!");
  }
}

// Function to Open and close the Water Pump from Blynk cloud
BLYNK_WRITE(V4) {
  int value = param.asInt(); // Get value as integer
  if (value == 1) {
    openWaterPump(false);
    Serial.println("Opening Window!");
  } else {
    closeWaterPump(true);
    Serial.println("Closing Window!");
  }
}