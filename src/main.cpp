/*
 3D Printer Box Monitor (3Dpbm)

 Description :
 WiFi Client
 Controls the temperature, humidity and pressure inside the print box
 Turn On/Off the printer
 Turn off the printer if temperature is excessive (exemple 60°C)
 Displays the NTP time (GMT+1) with summer time change
 Monitoring by Web browser or Android App trought Blynk broker

 Circuit:
 * Led and Relay Optocoupler on GPIO 27 (Power On/Off Pronter)
 * DHT22 on GPIO 26
 * BMP280 i2C interface with GPIO 22 as I2C_SCL and GPIO 21 as I2C_SDA
 * LCD Display 1602A 2x16 on GPIOs 22,23,5,18,19,21

 Release History :
 . 03/06/2018   V1.0    Lionel Fortier  Initial program
 . 05/06/2018   V1.1    Lionel Fortier  Add LCD Display

 Framework : Arduino (C++)
 Platform : ESP32
 Hardware : ESP32 WROOM-32 Dev Module (Geekcreit)

 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h> // Blynk Library for ESP32
#include <DHT.h> // DHT Sensors Library
#include <LiquidCrystal.h> // LCD 1602 Display Library
#include <Adafruit_Sensor.h> // For BMP280 Sensor
#include <Adafruit_BMP280.h> // For BMP280 Sensor

/* Comment this out to disable BLYNK prints and save space */
#define BLYNK_PRINT Serial

// Relay PIN
#define RELAYPIN 27
// DHT22 Sensor
#define DHTPIN 26
#define DHTTYPE DHT22
DHT myDHTSensor(DHTPIN, DHTTYPE);

// LCD 1602A Crystal
LiquidCrystal lcd(17,23,5,18,19,16);

// BMP280 Sensor
#define I2C_SCL 22
#define I2C_SDA 21
#define BME280_ADDRESS 0x76  //If the sensor does not work, try the 0x77 address as well
Adafruit_BMP280 bmp; // Interface I2C
//Adafruit_BMP280 bme(BMP_CS); // Interface SPI
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

// Auth Token in the Blynk App.
char auth[] = "d885d49e9b97457d91bb99c233b305db";

// WiFi Credentials
char ssid[] = "CATWIFI_2G_4";
char pass[] = "vesperboncamiitrobarasbonafi";

BlynkTimer timer_Sensors;

String floatToString(float x, byte precision = 1)
{
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

void ReadSensors()
{

    // BMP Sensor
    float fPressure = bmp.readPressure();
    if (isnan(fPressure)) {
        Serial.println("Reading... BMP 280 Sensor [ERROR]");
        return;
    }
    Serial.println("Reading... BMP 280 Sensor [OK]");

    Serial.print("Pressure = ");
    Serial.print(fPressure / 100);
    Serial.println(" hPa");

    Serial.print("Approx. altitude = ");
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");

    Serial.println();
    // End BMP

    // DHT22
    float fHum = myDHTSensor.readHumidity();
    float fTemp = myDHTSensor.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(fHum) || isnan(fTemp)) {
        Serial.println("Reading... DHTSensor [ERROR]");
        return;
    }
    Serial.println("Reading... DHTSensor [OK]");
    Serial.print("Humidity: ");
    Serial.print(fHum);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(fTemp);
    Serial.println(" °C");

    // End DHT22

    //Show values on LCD Display
    String sTemp = floatToString(fTemp); // Round Temp
    String sHum = floatToString(fHum); // Round Hum
    String sPressure = floatToString(fPressure / 100 , 0);
    lcd.setCursor(0,0);
    lcd.print(sPressure+"h");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("T="+sTemp+"C"+" H="+sHum+"%");

    //Push data to Labeled Value on Blynk Android App
    /*
    For example: your sensor sends vaule of 12.6789 to Blynk application. Next formatting options are supported:
    /pin/ - displays the value without formatting (12.6789)
    /pin./ - displays the rounded value without decimal part (13)
    /pin.#/ - displays the value with 1 decimal digit (12.7)
    /pin.##/ - displays the value with two decimal places (12.68)
    */
    Blynk.virtualWrite(V1, fTemp);
    Blynk.virtualWrite(V2, fHum);
}

void setup() {

    Serial.begin(115200);
    Serial.println("Starting...");

    myDHTSensor.begin();

    if (!bmp.begin(BME280_ADDRESS)) {
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
    }

    // Gestion NTP...

    Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
    //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

    timer_Sensors.setInterval(5000L, ReadSensors);

    // Display init
    lcd.begin(16, 2);
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("3Dpbm HH:MM OFF!");

    // go to row 1 column 0, note that this is indexed at 0
    lcd.setCursor(0,1);
    lcd.print ("  Loading...    ");
    delay(4000);

}

void loop() {

    Blynk.run();
    timer_Sensors.run();

}
