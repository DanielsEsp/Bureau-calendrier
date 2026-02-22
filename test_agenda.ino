



// ====== REQUÊTE GOOGLE AGENDA ======
void checkCalendar() {
  
     if (WiFi.status() != WL_CONNECTED) return;

         HTTPClient http;

  // OBLIGATOIRE pour Google Apps Script
     http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
     http.begin(calendarURL);

     httpCode = http.GET();
     Serial.print("\nHTTP code: ");
     Serial.println(httpCode);

     if (httpCode == HTTP_CODE_OK) {
         String payload = http.getString();

         Serial.print("Payload brut :");
         Serial.println(payload); Serial.println();

    //   Taille suffisante pour plusieurs événements
         StaticJsonDocument<4096> doc;
         DeserializationError error = deserializeJson(doc, payload);

         if (error) {
             Serial.print("deserializeJson() failed: ");
             Serial.println(error.c_str());
             return;
            }

         const char* date = doc["date"]; // "Sun Jan 25 2026"
         Serial.println(); Serial.print("Date: "); Serial.println(date);

         time_t now = time(nullptr);
         struct tm local;
         localtime_r(&now, &local);
         char heures[4];
         strftime(heures, sizeof(heures), "%H", &local);
         int tempsHeures = atoi(heures);
         Serial.print("Heure: "); Serial.print(tempsHeures);
         char minutes[4];        
         strftime(minutes, sizeof(minutes), "%M", &local);
         int tempsMinutes = atoi(minutes);
         Serial.print(" et minutes: "); Serial.println(tempsMinutes);                

         count = doc["count"];
         Serial.print("Nombre evenements: "); Serial.println(count); Serial.println();
         nbrCount = 1;

         for (JsonObject event : doc["events"].as<JsonArray>()) {
              Serial.print("Evenment: "); Serial.println(nbrCount);
              const char* event_title = event["title"];
              Serial.print("  Nature: "); Serial.println(event_title);
              const char* event_start = event["start"];
              Serial.print("   Debut: "); Serial.println(isoToLocal(event_start));

              String alarmeDebut = isoToLocal(event_start);
              alarmeHeures = (alarmeDebut.substring(9,11).toInt());
              Serial.print("   Alarme heures: "); Serial.print(alarmeHeures);
              alarmeMinutes = (alarmeDebut.substring(12,14).toInt());
              Serial.print(" et minutes: "); Serial.println(alarmeMinutes);
              if ((tempsHeures == alarmeHeures) && (tempsMinutes >= alarmeMinutes)) {
                  Serial.println("============= ALARME =============");  
                  flagAlarme = true;
                 } else {
                  flagAlarme = false;
                 }
                 
              if (nbrCount < 5) { 
                  evenementTab[nbrCount] = event_title;                                 
                  flagAlarmeTab[nbrCount] = flagAlarme;
                  alarmeTab[nbrCount] = alarmeDebut.substring(9,14);  
                 }   
   
              const char* event_end = event["end"];
              Serial.print("   Fin  : "); Serial.println(isoToLocal(event_end));
              Serial.println(); nbrCount = nbrCount + 1; 
             }  
             
         } else {
          Serial.println("❌ Erreur requête HTTP");
         }

     http.end();

     delay(500);
     
  // OBLIGATOIRE pour Google Apps Script
     http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
     http.begin(calendarTomorrow);

     httpCode = http.GET();
     Serial.print("\nHTTP code: ");
     Serial.println(httpCode);

     if (httpCode == HTTP_CODE_OK) {
         String payload = http.getString();

         Serial.print("Payload brut :");
         Serial.println(payload); Serial.println();

    //   Taille suffisante pour plusieurs événements
         StaticJsonDocument<4096> doc;
         DeserializationError error = deserializeJson(doc, payload);

         if (error) {
             Serial.print("deserializeJson() failed: ");
             Serial.println(error.c_str());
             return;
            } 

         count = doc["count"];
         Serial.print("Nombre evenements: "); Serial.println(count); Serial.println();         
         nbrTomorrCount = 1;

         for (JsonObject event : doc["events"].as<JsonArray>()) {
              Serial.print("Evenment: "); Serial.println(nbrTomorrCount);
              const char* event_title = event["title"];
              Serial.print("  Nature: "); Serial.println(event_title);
              const char* event_start = event["start"];
              Serial.print("   Debut: "); Serial.println(isoToLocal(event_start));   
              String alarmeDebut = isoToLocal(event_start);
              const char* event_end = event["end"];
              Serial.print("   Fin  : "); Serial.println(isoToLocal(event_end));

              if (nbrTomorrCount < 5) { 
                  tomorrEvenementTab[nbrTomorrCount] = event_title;                  
                  tomorrAlarmeTab[nbrTomorrCount] = alarmeDebut.substring(9,14);  
                 }                 
              
              Serial.println(); nbrTomorrCount = nbrTomorrCount + 1; 
             }  
             
         } else {
          Serial.println("❌ Erreur requête HTTP");
         }

     http.end();
     
}


// ====== CONVERSION ISO → HEURE LOCALE (ESP32 COMPATIBLE) ======
String isoToLocal(const char* iso) {

    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    // Parse ISO 8601 UTC (ex: 2026-01-25T10:00:00)
    strptime(iso, "%Y-%m-%dT%H:%M:%S", &tm);

    tm.tm_isdst = 0;  // UTC = pas d'heure d'été

    // Sauvegarde du TZ courant
    char tzBackup[64];
    const char* oldTZ = getenv("TZ");
    if (oldTZ) strncpy(tzBackup, oldTZ, sizeof(tzBackup));

    // Force UTC
    setenv("TZ", "UTC0", 1);
    tzset();

    // mktime interprète maintenant tm comme UTC
    time_t utc = mktime(&tm);

    // Restaure le fuseau local
    if (oldTZ) setenv("TZ", tzBackup, 1);
    else unsetenv("TZ");
    tzset();

    // Conversion UTC → heure locale
    struct tm local;
    localtime_r(&utc, &local);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%y %H:%M", &local);
    return String(buffer);
}


//
