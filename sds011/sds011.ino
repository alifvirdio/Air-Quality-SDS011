#include <SDS011.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ThingSpeak.h>


#define CHANNEL_ID 2445591
#define CHANNEL_API_KEY "EG0YKIFYJK838AY2"

int coloumnsLcd = 20;
int rowsLcd = 4;
float pm25, pm10;
char buffer_25[10], buffer_10[10];
  

//konek wifi ssid dan pw
const char* ssid = "MeteoJuanda";
const char* password = "hiluxpertek";

//init objek
SDS011 sds;
LiquidCrystal_I2C lcd(0x27, coloumnsLcd, rowsLcd);
AsyncWebServer server(80);
WiFiClient client;   

void wifi_connect_flag() {
  lcd.setCursor(0, 0);  //pojok kiri atas
  lcd.print("Connecting..");
  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Tidak Terhubung! \n");
  }
  
//  int freq = WiFi.getFrequency();
//  if (freq > 2400 && freq < 2500) {
//      Serial.println("Connected to a 2.4GHz WiFi network");
//    } else if (freq > 5000 && freq < 6000) {
//      Serial.println("Connected to a 5GHz WiFi network");
//    } else {
//      Serial.println("Unable to determine WiFi frequency");
//    }

  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  lcd.setCursor(0, 1);
  lcd.print("Connected!");
  lcd.setCursor(0, 2);
  lcd.print("SSID : ");
  lcd.print(ssid);
  lcd.setCursor(0, 3);
  lcd.print(WiFi.localIP());
  delay(5000);
  lcd.clear();
}

void route_webpage(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/index.html");
  });
  server.on("/pm25", HTTP_GET, [](AsyncWebServerRequest *request){
  char buffer[8]; // Menggunakan buffer untuk mengonversi float ke string
  sprintf(buffer, "%.2f", pm25); // Mengonversi nilai pm25 menjadi string
  request->send_P(200, "text/plain", buffer);
});

  server.on("/pm10", HTTP_GET, [](AsyncWebServerRequest *request){
  char buffer[8]; // Menggunakan buffer untuk mengonversi float ke string
  sprintf(buffer, "%.2f", pm10); // Mengonversi nilai pm10 menjadi string
  request->send_P(200, "text/plain", buffer);
});
}

void setup() {
  Serial.begin(9600);
  sds.begin(16, 17);  //GPIO16 (RX) dan GPIO17 (TX)
  lcd.init();
  lcd.backlight();
  ThingSpeak.begin(client);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  wifi_connect_flag();
  route_webpage();

  server.begin(); //server start agar bisa di akses website
}

void loop() {
  lcd.setCursor(0, 0);  //pojok kiri atas
  lcd.print("Air Quality Index!!");
  lcd.setCursor(0, 1);
  lcd.print("BMKG JUANDA x PENS");
  
  int error = sds.read(&pm25, &pm10);

  if (!error) {
    sprintf(buffer_25, "%.1f", pm25);
    sprintf(buffer_10, "%.1f", pm10);
    
    // Serial.print("PM2.5:");
    // Serial.print(pm25);
    // Serial.print(" µg/m³");
    // Serial.print("PM10 :");
    // Serial.print(pm10);
    // Serial.println(" µg/m³");

    //lcd
    lcd.setCursor(0, 2);
    lcd.print("PM 2.5: ");
    lcd.print(buffer_25);
    lcd.setCursor(0, 3);
    lcd.print("PM 10 : ");
    lcd.print(buffer_10);

    pm25++;
    pm10++;
  
    ThingSpeak.setField(1, pm25);
    ThingSpeak.setField(2, pm10);
              
    ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);

    
  } else {
    Serial.println("Failed to read from SDS011 sensor.");
  }
  delay(1000);  // Delay 1 detik (1000 milidetik)
}
