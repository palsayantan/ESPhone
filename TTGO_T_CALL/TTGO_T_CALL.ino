// Calibration data is stored in SPIFFS so we need to include it
#include <FS.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "pins.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// I2C for SIM800 (to keep it running when powered from battery)
TwoWire I2CPower = TwoWire(0);

// TinyGSM Client for Internet connection
TinyGsmClient client(modem);

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

bool setPowerBoostKeepOn(int en) {
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  if (en) {
    I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    I2CPower.write(0x35); // 0x37 is default reg value
  }
  return I2CPower.endTransmission() == 0;
}
/*
  #include <Adafruit_BME280.h>
  Adafruit_BME280 bme; // I2C
  Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
  Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
  Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();
*/

#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

#include <FastLED.h>
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

#include <TimerEvent.h>
TimerEvent Clock;

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  SerialMon.begin(9600);
  pinMode(TFT_BL, OUTPUT);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS initialised.");

  tft.init();
  tft.setRotation(2);
  touch_calibrate();
  tft.fillScreen(TFT_BLACK);

  // Start I2C communication
  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);

  // Keep power when running from battery
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));
  /*
    bool mpuStat = mpu.begin();   //0x75
    SerialMon.println(String("MPU6050 initiate ") + (mpuStat ? "OK" : "FAIL"));

    bool bmpStat = bmp.begin(0x76);    // 0x76
    SerialMon.println(String("BMP280 initiate ") + (bmpStat ? "OK" : "FAIL"));

    mpu_accel = mpu.getAccelerometerSensor();
    mpu_accel->printSensorDetails();

    mpu_gyro = mpu.getGyroSensor();
    mpu_gyro->printSensorDetails();

    // Default settings from datasheet.
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
                  Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtering.
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time.

    bmp_temp->printSensorDetails();

    bool bmeStat = bme.begin();    // 0x76
    SerialMon.println(String("BME280 initiate ") + (bmeStat ? "OK" : "FAIL"));

    bme_temp->printSensorDetails();
    bme_pressure->printSensorDetails();
    bme_humidity->printSensorDetails();
  */
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.init();
  do {
    tft.setTextColor(TFT_WHITE);
    drawBmp("/logo.bmp", 30, 100);
    Serial.write(SerialAT.read());
  } while (millis() < 2000);  //20000
  tft.fillScreen(TFT_BLACK);
  SerialMon.println("READY!");
  setTimerFunc();
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  /*
    SerialAT.println("AT+CMGD=1,4");
    SerialAT.println("AT+CNMI=1,2,0,0,0");
  */
}

int vBat, csq, yr;
String network;
bool ini;
int hh, mm, ss, DD, MM, YY;

bool Home = true, panel, Menu, recent, back;  // -+-+-
bool call, message, contact, settings;        // -+++
bool calculator, calender, camera, clocks;    // --+-
bool compass, gyro, weather, gps;             // ----
bool browser, rgb, gesture, info;             // -+--
bool files, terminal, cpu, sim;               // -+--

void clrscr() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  Home =  panel =  Menu =  recent =  back = false;
  call =  message =  contact =  settings = false;
  calculator = calender = clocks = false;
  rgb = terminal = false;
}

void loop() {
  if (terminal) {
    if (SerialAT.available()) {
      tft.println(SerialAT.readString());
    }
  }

  if (SerialAT.available()) {
    String response = SerialAT.readString();
    Serial.print("SIM800 : ");
    Serial.println(response);

    if (response.indexOf("CIEV") > 0) {
      int index = response.indexOf("CIEV");
      network = response.substring(index + 18, index + 26);
      Serial.print("Network :");
      Serial.println(network);
    }
    if (response.indexOf("CBC") > 0) {
      int index = response.indexOf("+CBC");
      vBat = response.substring(index + 8, index + 11).toInt();
      //Serial.println(vBat);
    }
    if (response.indexOf("CSQ") > 0) {
      int index = response.indexOf("+CSQ");
      csq = response.substring(index + 5, index + 8).toInt();
      //Serial.println(csq);
    }
    if (response.indexOf("RING") > 0) {
      clrscr();
      incall(response);
    }
    if (response.indexOf("NO CARRIER") > 0) {
      clrscr();
      Home = true;
    }
    if (response.indexOf("CMTI") > 0) {
      Serial.println("Message Received!");
      int index = response.indexOf("SM");
      int sms = response.substring(index + 4, index + 6).toInt();
      Serial.print("Num SMS :");
      Serial.println(sms);
    }
  }
  if (Serial.available()) {
    SerialAT.write(Serial.read());
  }
  if (millis() > 30000 && !ini) {
    ini = true;
    SerialAT.println("AT+CAAS=0");
    SerialAT.println("AT+CHF=0,1");
    SerialAT.println("AT+CLVL=100");
    SerialAT.println("AT+CMIC=1,15");
    SerialAT.println("AT+CMGF=1");
    SerialAT.println("AT+CBC");     // vBat
    SerialAT.println("AT+CSQ");     // strength
  }
  menuSelect();
  touch();
  //bme280();
  //mpu6050();
}
