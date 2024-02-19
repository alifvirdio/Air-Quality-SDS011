#include <Arduino.h>
#include <SDS011.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

int coloumnsLcd = 20;
int rowsLcd = 4;

//konek wifi ssid dan pw
const char* ssid = "MeteoJuanda";
const char* password = "hiluxpertek";

//init objek
SDS011 sds;
LiquidCrystal_I2C lcd(0x27, coloumnsLcd, rowsLcd);

void setup() {
    Serial.begin(9600);
    sds.begin(16, 17);  //GPIO16 (RX) dan GPIO17 (TX)
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0,0); //pojok kiri atas
    lcd.print("Connecting..");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Tidak Terhubung \n");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    lcd.setCursor(0, 2);
    lcd.print("Connected!");
    lcd.setCursor(0, 3);
    lcd.print(WiFi.localIP());
    delay(5000);
    lcd.clear();
}

void loop() {

    lcd.setCursor(0,0); //pojok kiri atas
    lcd.print("Air Quality Index!!");
    lcd.setCursor(0,1);
    lcd.print("BMKG JUANDA x PENS");

    float pm25, pm10;
    int error = sds.read(&pm25, &pm10);

    if (!error) {
        // Serial.print("PM2.5:");
        // Serial.print(pm25);
        // Serial.print(" µg/m³ ");
        // Serial.print("PM10 :");
        // Serial.print(pm10);
        // Serial.println(" µg/m³");
        //lcd
        lcd.setCursor(0,2);
        lcd.print("PM 2.5: ");
        lcd.print(pm25);
        
        lcd.setCursor(0,3);
        lcd.print("PM 10 : ");
        lcd.print(pm10);
    } else {
        Serial.println("Failed to read from SDS011 sensor.");
    }

    // Tunggu sebentar sebelum membaca kembali
    delay(1000); // Delay 1 detik (1000 milidetik)
}