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
#include <LiquidCrystal.h>

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define DHTPIN 26
#define DHTTYPE DHT22

DHT myDHTSensor(DHTPIN, DHTTYPE);
LiquidCrystal lcd(22,23,5,18,19,21);

// Auth Token in the Blynk App.
char auth[] = "d885d49e9b97457d91bb99c233b305db";

// WiFi Credentials
char ssid[] = "CATWIFI_2G_4";
char pass[] = "vesperboncamiitrobarasbonafi";

BlynkTimer timer_DHT;

void DHTSensor()
{

    float fHum = myDHTSensor.readHumidity();
    float fTemp = myDHTSensor.readTemperature();

    //Arrondi des valeurs flottantes
    

    // Check if any reads failed and exit early (to try again).
    if (isnan(fHum) || isnan(fTemp)) {
        Serial.println("> DHTSensor() : ERROR");
        return;
    }
    Serial.println("> DHTSensor() : OK");
    Serial.print("Humidity: ");
    Serial.print(fHum);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(fTemp);
    Serial.println(" *C ");

    //Show values on LCD Display

    String sTemp;
    String sHum;
    sTemp = String(fTemp);
    sHum = String(fHum);

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

    myDHTSensor.begin();

    Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
    //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

    timer_DHT.setInterval(5000L, DHTSensor);

    // Display init
    lcd.begin(16, 2);
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("   3D P.M.Box   ");

    // go to row 1 column 0, note that this is indexed at 0
    lcd.setCursor(0,1);
    lcd.print ("  Loading...    ");
    delay(4000);

}

void loop() {

    Blynk.run();
    timer_DHT.run();

}
