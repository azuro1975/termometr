#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <NtpClientLib.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "FS.h"
#include "dbjason.h"
#include "RelaysModule.h"
#include <time.h>

#define ONE_WIRE_BUS 0

ESP8266WebServer server(80);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
IPAddress timeServerIP; // time.nist.gov NTP server address
std::shared_ptr<LiquidCrystal_I2C> lcd = std::make_shared<LiquidCrystal_I2C>(0x27,16,2);
std::shared_ptr<CRelaysModule> wlaczniki = std::make_shared<CRelaysModule>();
std::shared_ptr<DBjason> database;

const char* ntpServerName = "ntp.itl.waw.pl";
bool bLightOn = true;


void handleRoot() {
  lcd->setCursor(0,15);
  lcd->cursor();
  constexpr size_t buforsize = 512;
  char temp[buforsize];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, buforsize,

           "<html>\
  <head>\
    <meta charset='UTF-8'/>\
    <title>ESP8266 Info</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Info from ESP8266!</h1>\
    <p>Time: %s</p>\
    <p>Temperatura: %.1f C</p>\
    <h1>Właczniki: </h1>\
    <p>Wł1: %s</p>\
    <p>Wł2: %s</p>\
    <p>Wł3: %s</p>\
    <p>Wł4: %s</p>\
  </body>\
</html>",
           NTP.getTimeDateString().c_str(), sensors.getTempC(insideThermometer)
           ,wlaczniki->getYellow()?"ON":"OFF"
           ,wlaczniki->getOrangr()?"ON":"OFF"
           ,wlaczniki->getGreen()?"ON":"OFF"
           ,wlaczniki->getBlue()?"ON":"OFF"
           
          );
  server.send(200, "text/html", temp);
  lcd->noCursor();
}

void printDir(String dir)
{
  if (SPIFFS.exists(dir))
    Serial.println(dir); 
  Dir curDir = SPIFFS.openDir(dir);
  Serial.println(curDir.fileName());
  while (curDir.next())
  {
    Serial.println(curDir.fileName());
    //SPIFFS.remove(curDir.fileName());
  }
}

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  if (SPIFFS.begin())
  {
    Serial.println();
    database = std::make_shared<DBjason>();
    Serial.println(database->getFileName());

    if (!SPIFFS.exists(database->getFileName()))
    {
        database->saveFile(database->getFileName(), database->createEmptyDB());
    }
  }
   else
    Serial.println("SPIFFS.bgin ERROR!");
  
  wlaczniki->init();
  wlaczniki->connectLCD(lcd);
  
  lcd->begin(4,5);      // In ESP8266-01, SDA=4, SCL=5
  lcd->backlight();
  lcd->home();                // At column=0, row=0
  lcd->print("Connecting");
  WiFi.begin();
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (count>16)
      break;
    lcd->setCursor(count,1);
    lcd->print(".");
    count++;
  }
   if (WiFi.status() == WL_CONNECTED) {
    lcd->home();
    lcd->print("WiFi connected");
    lcd->setCursor(0,1);
    lcd->print(WiFi.localIP());
    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }
    NTP.begin(ntpServerName, 2, 0);
    NTP.setInterval(30);
    NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
        switch (event) {
            case timeSyncd:
                    Serial.println("Time successfully got from NTP server");
                break;
            case noResponse:
                    Serial.println("No response from server");
                break;
            case invalidAddress:
                    Serial.println("Address not reachable");
                break;
            default:
                Serial.println("No recognize event of NTP server");
                break;
        }
    });
    server.on("/", handleRoot);
    server.on("/test.svg", drawGraph);
    server.on("/database", []() {
        Serial.println("\n\rHTTP GET....");
        JsonObject& root = database->loadFile(database->getFileName());
        Serial.println("HTTP GET........");
        if (!root.success())
        {
            server.send(500, "text/plain; charset=UTF-8", "DataBase read ERROR!");
            return;
        }
        Serial.println("HTTP GET............");
        root["Time"] = NTP.getTimeDateString();
        Serial.println("HTTP GET................");
        root["Light"]=analogRead(A0)*(5.0/1023.0);
        Serial.println("HTTP GET....................");
        root["Temperature"]=sensors.getTempC(insideThermometer);
        Serial.println("HTTP GET........................");
        root["Switches"][0]["state"]=wlaczniki->getYellow();
        root["Switches"][1]["state"]=wlaczniki->getOrangr();
        root["Switches"][2]["state"]=wlaczniki->getGreen();
        root["Switches"][3]["state"]=wlaczniki->getBlue();
        Serial.println("HTTP GET........................");
        std::unique_ptr<String> buffStr = std::unique_ptr<String>(new String);
        Serial.println("HTTP GET............................");
        root.prettyPrintTo(*buffStr.get());
        database->freeBuffer();
        Serial.println("HTTP GET................................");
        server.send(200, "text/plain; charset=UTF-8", buffStr->c_str());
        Serial.println("HTTP GET................................END");
    });
    server.on("/clear", []() {
        database->saveFile(database->getFileName(), database->createEmptyDB());
      server.send(200, "text/plain", "database cleand");
    });
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server run");
    delay(1000);
    lcd->clear();
    lcd->home();
    lcd->print("HTTP server run");
    JsonObject& root = database->loadFile(database->getFileName());
    wlaczniki->jsonToMem(root);
    database->freeBuffer();
    delay(1000);
   }
   else
   {
    lcd->home();
    lcd->print("Connecting ERROR");
    delay(1000);
   }
  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
  lcd->clear();
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  String strLine1 = "Temp C:",strLine2="Temp F:";
  strLine1+=tempC;
  lcd->home();
  lcd->print(strLine1);
  strLine2+=DallasTemperature::toFahrenheit(tempC);
}
void lightSensor(float level)
{
  int lightSensor = analogRead(A0);
  float value =lightSensor*(5.0/1023.0);
  lcd->setCursor(8,1);
  lcd->print(value);
  if (value<level)
  {
    bLightOn= false;
    lcd->noBacklight();
  }
  else
  {
    bLightOn= true;
    lcd->backlight();
  }
}

void powerSwiches()
{
  wlaczniki->yellowOnOff(bLightOn);
  wlaczniki->orangrOnOff(minute(NTP.getTime())%2);
}
/*
 * Main function. It will request the tempC from the sensors and display on Serial.
 */
void loop(void)
{ 
  printTime();
  lightSensor(0.5);
  powerSwiches();
  printTemperature(insideThermometer); // Use a simple function to print out the data
  server.handleClient();
}

void printTime()
{
    //Serial.println(NTP.getTimeStr()); // print the second
    lcd->setCursor(0,1);
    lcd->print(NTP.getTimeStr());
    //wlaczniki->tickTime(NTP.getTime());
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

