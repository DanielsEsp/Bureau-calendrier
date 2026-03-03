



// ------ EPD ------
#include <GxEPD2_BW.h>
#define   GxEPD2_DISPLAY_CLASS GxEPD2_BW
// Connections for ESP32-C3 Supermini, trying to have most ADC pins left available
static const uint8_t EPD_BUSY = 21; // violet
static const uint8_t EPD_CS =   10; // orange
static const uint8_t EPD_RST =  06; // blanc
static const uint8_t EPD_DC =   07; // vert
static const uint8_t EPD_SCK =  20; // jaune
static const uint8_t EPD_MISO = -1; // Master-In Slave-Out not used, as no data from display
static const uint8_t EPD_MOSI = 05; // bleu
#define   GxEPD2_DRIVER_CLASS GxEPD2_426_GDEQ0426T82 // GDEQ0426T82 480x800, SSD1677 (P426010-MF1-A)
#define   GxEPD2_426_GDEQ0426T82_IS_BW true
          GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT>epd(GxEPD2_426_GDEQ0426T82(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)); // GDEQ0426T82 480x800, SSD1677 (P426010-MF1-A)
#define   CPU_FREQUENCY_MHZ     80
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ---- Position de départ ----
     int startX = 40;
     int startY = 40;
     int16_t tbx, tby; 
     uint16_t tbw, tbh;

#include <Adafruit_GFX.h> 
#include <U8g2_for_Adafruit_GFX.h>
          U8G2_FOR_ADAFRUIT_GFX affiche;

// ------ Configuration WiFi ------
#include <WiFi.h>
const char* ssid = "SSID";
const char* password = "PASS";
int wifitime, esprestart;

#include <HTTPClient.h>
#include <ArduinoJson.h>

// ------ Configuration NTP ------
#include <time.h>
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;        // GMT+1
const int daylightOffset_sec = 3600;    // Heure d'été
int currentDay, currentMonth, currentYear, currentWday, currentHour, currentMinutes;
#include "clock.h"
      int timeCall = (1*60)*1000; //  pour 1 minute en microsecondes
      int lastTimeCall;
          
// -------- Noms des jours --------
const char* daysShort[] =  { "Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam" };
const char* daysFull[] =   { "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi" };

// -------- Noms des mois --------
const char* monthsFull[] = { "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin",
                             "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre" };

//-------------------------------------------------------------------------------------------------
#include "bitmap_icones.h"
#include "icones_grille.h"
      int pointeur;
const unsigned char* icone;


//        Varialbles WEATHER DATAS LOAD
   String current = ("https://api.open-meteo.com/v1/forecast?latitude=48.477&longitude=7.6788&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,weather_code,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=auto");    
   String alerte = ("https://api.weatherapi.com/v1/alerts.json?key=1110af7d6e0c4d21a99104458242011&q=ICHTRATZHEIM");      
   String url;
   String current_date;
   String last_weather_update;
   String temperature;
   String ressenti;
   String humidite;
   String pression;
   String vent_vitesse;
   String vent_rafale;
      int vent_direction, Direction;
   String vient_De; 
   String is_day;
     bool grande_icone = true;
   String weather_code;
   String weather_code_txt;
      int meteoCall = (5*60)*1000; //       pour 5 minutes en microsecondes
      int lastMeteoCall;
      int alerteCall = (30*60)*1000; //     pour 30 minutes en microsecondes
      int lastAlerteCall;
     bool firstFlag = true;
      int yDecal;
   String oldWeatherCode, temperatureFormatTxt, oldTemperatureFormatTxt, ressentiFormatTxt, humiditeFormatTxt;
    float temperatureFormat, ressentiFormat;
      int humiditeFormat;

// ====== GOOGLE APPS SCRIPT URL ======
const char* calendarURL = "https://script.google.com/macros/s/#################/exec";
const char* calendarTomorrow = "https://script.google.com/macros/s/################/exec";
int httpCode;

// ====== TIMING ======
unsigned long lastCheck = 0;
const unsigned long interval = 2 * 60 * 1000; // 2 minutes
bool firstLoad = true;
bool   flagAlarme;
bool   flagAlarmeTab[5];
bool   lastAlarme; 
String evenement;
String tomorrEvenement;
String oldEvenement = "init";
String oldTomorrEvenement;
String evenementTab[5];
String lastEvenement;
String tomorrEvenementTab[5];
int    boucle;
int    count = 0;
int    nbrCount, oldNbrCount;
int    lastCount = 99;
int    nbrTomorrCount, oldNbrTomorrCount;
int    alarmeHeures;
String alarmeTab[5];
int    alarmeMinutes;
String tomorrAlarmeTab[5];
bool   sequanceAffichage = false;

//----------------------------------------------------------------------------



void setup() {

     Serial.begin(115200); Serial.println(); Serial.println();
     setCpuFrequencyMhz(CPU_FREQUENCY_MHZ);     
     Serial.println("Initialisation E-Paper");  
     SPI.end();
     SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
     epd.init(115200, true, 50, false); 
     epd.setRotation(0); 
     epd.setTextColor(GxEPD_BLACK);
     affiche.begin(epd); // connect u8g2 procedures to Adafruit GFX
        
     connectionWiFi();

     configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
     delay(100);

     epd.drawPaged(drawFull, 0); //  affiche calendrier
     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);
    
}


void loop() {

     if (WiFi.status() != WL_CONNECTED) {
         ESP.restart();
        }
     delay(50);
          
     if ((millis() - lastTimeCall) > timeCall) {
         lastTimeCall = millis();
         epd.drawPaged(drawPartial1, 0); //  horloge
         checkCalendar();               
         affiche_agenda();
         if ((oldEvenement != evenementTab[1]) || (oldNbrCount != nbrCount)) {
             oldEvenement = evenementTab[1];
             oldNbrCount = nbrCount;
             sequanceAffichage = true;            
             Serial.println("Nouvel evenement detecte!"); delay(50);
            } else {
             Serial.println("Pas de nouvel evenement detecte"); delay(50);           
            }

         if ((oldTomorrEvenement != tomorrEvenementTab[1]) || (oldNbrTomorrCount != nbrTomorrCount)) {
             oldTomorrEvenement = tomorrEvenementTab[1];
             oldNbrTomorrCount = nbrTomorrCount;
             sequanceAffichage = true; 
             Serial.println("Nouvel evenement pour demain detecte!"); delay(50);
            } else {
             Serial.println("Pas de nouvel evenement pour demain detecte"); delay(50);           
            }
          if (sequanceAffichage == true)  {
              sequanceAffichage = false;
              epd.drawPaged(drawPartial4, 0); //  agenda   
             }
        }

     if (((millis() - lastMeteoCall) > meteoCall) || firstFlag == true) {
         lastMeteoCall = millis();
         current_datas_load(); delay(50);               
         if (weather_code != oldWeatherCode) {
             oldWeatherCode = weather_code;
             epd.drawPaged(drawPartial2, 0); //  affiche icone              
            }
         if ((temperatureFormatTxt != oldTemperatureFormatTxt)  || firstFlag == true) {
             oldTemperatureFormatTxt = temperatureFormatTxt;
             epd.drawPaged(drawPartial3, 0); //  affiche temp. datas              
            }

         firstFlag = false;      
            
        }      

}



void connectionWiFi() {
  
     Serial.print("Connexion WiFi...");
     WiFi.begin(ssid, password);
  
     int boucleTest = 0;
     while (WiFi.status() != WL_CONNECTED && boucleTest < 20) {
            Serial.print("."); delay(500); 
            boucleTest++;
           }  
     if (WiFi.status() == WL_CONNECTED) {
         Serial.println(" > WiFi connecté ");
        } else {
         Serial.println(" > Échec connection WiFi !");
        }
        
}


void lectureHeure() {
  
     struct tm timeinfo;
     if (!getLocalTime(&timeinfo)) {
         Serial.println("Erreur NTP");
         timeinfo.tm_year = 126;
         timeinfo.tm_mon = 1;
         timeinfo.tm_mday = 12;
         timeinfo.tm_hour = 15;
         timeinfo.tm_min = 37;
         timeinfo.tm_wday = 1; // Lundi
        }
    
     currentDay = timeinfo.tm_mday;
     currentMonth = timeinfo.tm_mon;
     currentYear = timeinfo.tm_year + 1900;
     currentWday = timeinfo.tm_wday;
     currentHour = timeinfo.tm_hour;
     currentMinutes = timeinfo.tm_min;

}


void drawFull(const void* pv) { //   afficheCalendrier

     epd.setFullWindow();
     epd.fillScreen(GxEPD_WHITE);
    
     // Obtenir l'heure
     lectureHeure();
     affiche_calendrier();

     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);         
         
}


void drawPartial1(const void* pv) { //  horloge

     lectureHeure();
     epd.setPartialWindow(startX + 465, startY - 15, 90, 30);
     epd.setFont(&FreeSansBold18pt7b);
     epd.setCursor(startX + 465, startY + 10);
     if (currentHour < 10) { epd.setCursor(startX + 485, startY + 10); }
     epd.print(currentHour);
     epd.setFont(&FreeSans18pt7b);
     epd.print("h");
     epd.setFont(&FreeSansBold18pt7b);
     if (currentMinutes < 10) {
         epd.print("0"); epd.print(currentMinutes);      
        } else {
         epd.print(currentMinutes);
        }   

     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);
     
}


void drawPartial2(const void* pv) { //  affiche icone
  
     epd.setPartialWindow(startX + 590, startY - 20, 190, 120);
     epd.drawBitmap(startX + 590, startY - 15, icone_name[pointeur], 140, 120, GxEPD_BLACK);
     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);
      
}


void drawPartial3(const void* pv) { //   affiche temp. datas
     
     epd.setPartialWindow(startX + 580, startY + 100, 195, 310);     
     affiche.setBackgroundColor(GxEPD_WHITE);  
     affiche.setForegroundColor(GxEPD_BLACK);     
     affiche.setFont(u8g2_font_logisoso22_tf);     
     affiche.setCursor(startX + 580, startY + 130);
     affiche.print("Température");
     affiche.setFont(u8g2_font_fub30_tn);
     affiche.setCursor(startX + 620, startY + 170);
     affiche.print(temperatureFormatTxt);  
     affiche.setFont(u8g2_font_inb27_mf);      
     affiche.print("\u00B0");  
     affiche.setFont(u8g2_font_logisoso22_tf);     
     affiche.setCursor(startX + 610, startY + 200);
     affiche.print("Ressenti");     
     affiche.setFont(u8g2_font_fub30_tn);
     affiche.setCursor(startX + 615, startY + 240);
     affiche.print(ressentiFormatTxt); 
     affiche.setFont(u8g2_font_inb27_mf);      
     affiche.print("\u00B0");            
     affiche.setFont(u8g2_font_logisoso22_tf); 
     affiche.setCursor(startX + 610, startY + 270);
     affiche.print("Humidité");
     affiche.setFont(u8g2_font_fub30_tn);
     affiche.setCursor(startX + 630, startY + 310);
     affiche.print(humiditeFormatTxt); 
     affiche.setFont(u8g2_font_inr24_mr);      
     affiche.print("%"); 
     affiche.setFont(u8g2_font_logisoso22_tf); 
     affiche.setCursor(startX + 600, startY + 340);
     affiche.print("Vent km/h");
     affiche.setFont(u8g2_font_fub30_tn);
     affiche.setCursor(startX + 640, startY + 380);
     affiche.print(vent_vitesse); 
     affiche.setFont(u8g2_font_logisoso22_tf);
     affiche.setCursor(startX + 600, startY + 410);
     affiche.print("Nord Ouest");
                   
     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);
                       
}


void drawPartial4(const void* pv) { //  agenda

     epd.setPartialWindow(startX, startY + 245, 565, 170);

     affiche.setBackgroundColor(GxEPD_WHITE);  
     affiche.setForegroundColor(GxEPD_BLACK);     
//     affiche.setFont(u8g2_font_helvR24_tf);          
//     affiche.setFont(u8g2_font_helvB24_tf);
     affiche.setFont(u8g2_font_helvR24_tf); 
     affiche.setCursor(startX + 40, startY + 270);
     affiche.print("Aujourdhui");
     if (nbrCount > 1) {     
         yDecal = 60;
         for (boucle = 1; boucle < nbrCount; boucle ++) {
              if (flagAlarmeTab[boucle] == true) {
                  affiche.setFont(u8g2_font_helvB24_tf);
                  affiche.setCursor(startX, startY + 245 + yDecal); 
                  affiche.print(evenementTab[boucle]);                             
                  affiche.setCursor(startX + 170, startY + 245 + yDecal);
                  affiche.print(alarmeTab[boucle]);
                 } else {
                  affiche.setFont(u8g2_font_helvR24_tf); 
                  affiche.setCursor(startX, startY + 245 + yDecal);
                  affiche.print(evenementTab[boucle]);                                  
                  affiche.setCursor(startX + 170, startY + 245 + yDecal);
                  affiche.print(alarmeTab[boucle]);
                 }
                  if (boucle == 1) { oldEvenement = evenementTab[1]; }
                  yDecal = yDecal + 35;
                 }
              } else {
                epd.drawBitmap(startX + 50, startY + 285, epd_bitmap_calendar, 140, 120, GxEPD_BLACK);  
              }
         delay(100);
         yDecal = 60;
         affiche.setFont(u8g2_font_helvR24_tf);
         affiche.setCursor(startX + 350, startY + 270);
         affiche.print("Demain");
         if (nbrTomorrCount > 1) {     
             for (boucle = 1; boucle < nbrTomorrCount; boucle ++) {
                  affiche.setCursor(startX + 310, startY + 245 + yDecal);
                  affiche.print(tomorrEvenementTab[boucle]);                                  
                  affiche.setCursor(startX + 470, startY + 245 + yDecal);
                  affiche.print(tomorrAlarmeTab[boucle]);      
                  if (boucle == 1) { oldTomorrEvenement = tomorrEvenementTab[1]; }
                  yDecal = yDecal + 35;
                 }
           } else {
            epd.drawBitmap(startX + 340, startY + 285, epd_bitmap_calendar, 140, 120, GxEPD_BLACK);  
           }

     delay(50);
     epd.hibernate();
     delay(50);
     epd.powerOff();  
     delay(50);
  
}


void affiche_agenda() {

     Serial.println("Evenements a venir");
     for (int boucle = 1; boucle < nbrCount; boucle ++) {
         Serial.print(evenementTab[boucle]); Serial.print(" > ");
         Serial.print(flagAlarmeTab[boucle]); Serial.print(" : ");
         Serial.println(alarmeTab[boucle]);
        }
     Serial.println("------------------");

     Serial.println("Et pour demain");
     for (int boucle = 1; boucle < nbrTomorrCount; boucle ++) {
         Serial.print(tomorrEvenementTab[boucle]); Serial.print(" > ");
         Serial.println(tomorrAlarmeTab[boucle]);
        }
     Serial.println("------------------");
                 
}


//
