
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL4aPruMaW-"
#define BLYNK_TEMPLATE_NAME "Animal Health Monitor"
#define BLYNK_AUTH_TOKEN "y_pIn1AIj2b-e_XgqO5PdNG_uxmqByVn"


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "MPU6050.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>

// WIFI information
char ssid[] = "Galaxy A50";
char pass[] = "Mohammad1381";



BlynkTimer timer;

// Setting the initial parameters of MAX30102 sensor
MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

const long FINGER_THRESHOLD = 5000;
float filteredSpO2 = 0;
unsigned long lastSpo2Update = 0;
const unsigned long SPO2_PERIOD = 1000;

float calculateSpO2(long irValue, long redValue) {
  if (irValue == 0 || redValue == 0) return 0;

  static long lastIR = 0, lastRed = 0;
  float dcIR  = irValue;
  float dcRed = redValue;
  float acIR  = abs(irValue - lastIR);
  float acRed = abs(redValue - lastRed);
  lastIR  = irValue;
  lastRed = redValue;

  float R = ( (acRed / dcRed) ) / ( (acIR / dcIR) );
  float SpO2 = 110.0 - 25.0 * R;
  if (SpO2 > 100) SpO2 = 100;
  if (SpO2 < 70)  SpO2 = 70;
  return SpO2;
}


//  MAX30102 datd reading function 
void readMAX30102() {
  long irValue = particleSensor.getIR();

  if (irValue < FINGER_THRESHOLD) {
    beatAvg = 0;
    filteredSpO2 = 0;
  } else {
  
    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  }

  // sending the data every 1 second
  if (millis() - lastSpo2Update > SPO2_PERIOD) {
    lastSpo2Update = millis();
    long redValue = particleSensor.getRed();
    float rawSpo2 = calculateSpO2(irValue, redValue);

    float alpha = 0.2;
    filteredSpO2 = (alpha * rawSpo2 + (1 - alpha) * filteredSpO2) + 0.2;

    Serial.print("[MAX30102] BPM=");
    Serial.print(beatAvg);
    Serial.print("  SpO2=");
    Serial.println(filteredSpO2);

    Blynk.virtualWrite(V10, beatAvg);
    Blynk.virtualWrite(V11, filteredSpO2);
  }
}


// Setting the initial parameters of GPS-NEO-6M sensor 
static const int RXPin = D7, TXPin = D6;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

unsigned long lastGpsUpdate = 0;  


//  function for calculating the elapsed time
String Calculate_Time(unsigned long lastUpdate) { 
  unsigned long secondsAgo = (millis() - lastUpdate) / 1000;
  unsigned long minutesAgo = secondsAgo / 60;
  unsigned long hoursAgo   = minutesAgo / 60;
  secondsAgo = secondsAgo % 60;
  minutesAgo = minutesAgo % 60;
  char buffer[32];
  sprintf(buffer, "%02lu:%02lu:%02lu ago", hoursAgo, minutesAgo, secondsAgo);
  return String(buffer);
}


//  GPS-NEO-6M datd reading function
void readGPS() {


  if (gps.location.isValid()) {
    if (gps.location.isUpdated()) {
      lastGpsUpdate = millis();
      String loc = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
      Serial.print("[GPS] Location: ");
      Serial.println(loc);
      Blynk.virtualWrite(V20, loc);
    }
    String timeStr = Calculate_Time(lastGpsUpdate);
    Serial.print("[GPS] Last update: ");
    Serial.println(timeStr);
    Blynk.virtualWrite(V21, timeStr);
  } else {
    String timeStr = Calculate_Time(lastGpsUpdate);
    Serial.println("[GPS] GPS not ready yet...");
    Blynk.virtualWrite(V21, timeStr);
    Serial.print("[BLYNK] Last update: ");
    Serial.println(timeStr);
  }
}



// Setting the initial parameters of MPU6050 sensor 
MPU6050 mpu;
float accX, accY, accZ;
float totalAcc, prevAcc = 0;
const int windowSize = 4;
float diffArray[windowSize] = {0};
int idx = 0;

// Thresholds
float walkingThreshold = 0.02;
float runningThreshold = 0.35;
float alertThreshold = 2;

String statusMPU = "Initializing...";

void readMPU6050() {
  accX = mpu.getAccelerationX() / 16384.0;
  accY = mpu.getAccelerationY() / 16384.0;
  accZ = mpu.getAccelerationZ() / 16384.0;
  totalAcc = sqrt(accX*accX + accY*accY + accZ*accZ);
  float diff = abs(totalAcc - prevAcc);
  prevAcc = totalAcc;

  diffArray[idx] = diff;
  idx = (idx + 1) % windowSize;

  float avgDiff = 0, maxVal = 0;
  for(int i=0; i<windowSize; i++){
    avgDiff += diffArray[i];
    if(diffArray[i] > maxVal) maxVal = diffArray[i];
  }
  avgDiff /= windowSize;

  if(maxVal > alertThreshold){
    statusMPU = "ALERT! FALL DETECTED";
    Blynk.logEvent("fall_detected", "حیوان افتاد!");
    Blynk.virtualWrite(V31, 255);
  }
  else if(avgDiff > runningThreshold){
    statusMPU = "RUNNING";
    Blynk.virtualWrite(V31, 0);
  }
  else if(avgDiff > walkingThreshold){
    statusMPU = "WALKING";
    Blynk.virtualWrite(V31, 0);
  }
  else{
    statusMPU = "RESTING";
    Blynk.virtualWrite(V31, 0);
  }

  Serial.print("[MPU6050] Status: ");
  Serial.println(statusMPU);

  Blynk.virtualWrite(V30, statusMPU);
}



//Setting the initial parameters of DS18B20 sensor
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void readDS18B20() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("[DS18B20] Sensor not found!");
    return;
  }
  Serial.print("[DS18B20] Temperature: ");
  Serial.println(tempC);
  Blynk.virtualWrite(V40, tempC);
}



// The set up function
void setup() {
  Serial.begin(115200);

  //  WiFi + Blynk Debug
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) { 
    delay(1000);
    Serial.print(".");
    wifi_attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
  }

  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  int blynk_attempts = 0;
  while(!Blynk.connected() && blynk_attempts < 20){
    Blynk.run();
    delay(500);
    Serial.print(".");
    blynk_attempts++;
  }

  if(Blynk.connected()){
    Serial.println();
    Serial.println("Blynk connected!");
  } else {
    Serial.println();
    Serial.println("Blynk connection failed!");
  }

  //  MAX30102 
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("[MAX30102] FAILED");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeIR(0x0A);

  //  GPS NEO6M 
  ss.begin(GPSBaud);

  // MPU6050
 Wire.begin();     
 mpu.initialize(); 
  if(!mpu.testConnection()){
    Serial.println("[MPU6050] connection failed!");
    while(1);
  }

  //  DS18B20 
  sensors.begin();

  //  set the Timers 
  timer.setInterval(0.0001L, readMAX30102);
  timer.setInterval(1000L, readGPS);
  timer.setInterval(400L,  readMPU6050);
  timer.setInterval(1500L, readDS18B20);
}


void loop() {
 
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

 
  Blynk.run();
  timer.run();

}


