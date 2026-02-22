



void affiche_calendrier() {

     // Obtenir l'heure
     lectureHeure();

     // ===== JOUR, DATE ET HEURE =====
     epd.setFont(&FreeSansBold18pt7b);
     epd.setCursor(startX, startY + 15);
     epd.print(daysFull[currentWday]); epd.print(" ");
     epd.print(currentDay);
     epd.drawBitmap(startX + 415, startY - 18, epd_bitmap_clock, 40, 40, GxEPD_BLACK);
     epd.setCursor(startX + 465, startY + 15);
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

     // ==== LOGO ====    
     epd.drawBitmap(startX + 240, startY - 15, fog_moon, 100, 87, GxEPD_BLACK);
         
     // ===== MOIS ET ANNÉE =====
     char monthYear[50];
     sprintf(monthYear, "%s %d", monthsFull[currentMonth], currentYear);
     epd.setFont(&FreeSansBold18pt7b);
     epd.setCursor(startX, startY + 55);
     epd.print(monthsFull[currentMonth]);
     epd.setCursor(startX + 482, startY + 55);
     epd.print(currentYear);
    
     // ===== GRILLE CALENDRIER =====
     int calStartY = startY + 60;
     int cellWidth = 80;
     int cellHeight = 27;  // Augmenté de 25 à 27 pixels (+2px)
     int headerHeight = 30;
     int gridWidth = cellWidth * 7;
    
     // Fond gris foncé pour l'en-tête (simulation avec quadrillage)
     epd.fillRect(startX, calStartY, gridWidth, headerHeight, GxEPD_BLACK);
    
     // En-têtes des jours (en blanc sur fond noir)
     epd.setFont(&FreeSansBold9pt7b);
     epd.setTextColor(GxEPD_WHITE);
     for (int i = 0; i < 7; i++) {
          int x = startX + i * cellWidth;
          epd.getTextBounds(daysShort[i], 0, 0, &tbx, &tby, &tbw, &tbh);
          int textX = x + (cellWidth - tbw) / 2;
          epd.setCursor(textX, calStartY + 25);
          epd.print(daysShort[i]);
         }
     epd.setTextColor(GxEPD_BLACK);
    
     // Bordure de la grille
     epd.drawRect(startX, calStartY, gridWidth, headerHeight + cellHeight * 5, GxEPD_BLACK);
    
     // Lignes verticales
     for (int i = 1; i < 7; i++) {
          int x = startX + i * cellWidth;
          epd.drawLine(x, calStartY, x, calStartY + headerHeight + cellHeight * 5, GxEPD_BLACK);
         }
    
     // Ligne sous l'en-tête
     epd.drawLine(startX, calStartY + headerHeight, startX + gridWidth, calStartY + headerHeight, GxEPD_BLACK);
    
     // Calculer le premier jour du mois
     struct tm firstDay = {0};
     firstDay.tm_year = currentYear - 1900;
     firstDay.tm_mon = currentMonth;
     firstDay.tm_mday = 1;
     mktime(&firstDay);
     int startDayOfWeek = firstDay.tm_wday;    
     int daysInMonth = getDaysInMonth(currentMonth, currentYear);
    
     // Dessiner toutes les lignes horizontales (5 semaines)
     for (int row = 1; row <= 5; row++) {
          int y = calStartY + headerHeight + row * cellHeight;
          epd.drawLine(startX, y, startX + gridWidth, y, GxEPD_BLACK);
         }
    
     // Dessiner les dates
     epd.setFont(&FreeSansBold9pt7b);
     int day = 1;
    
     for (int row = 0; row < 5; row++) {
          for (int col = 0; col < 7; col++) {
               if (row == 0 && col < startDayOfWeek) {
                   continue;
                  }
        
               if (day > daysInMonth) {
                   break;
                  }
        
               String dayStr = String(day);
               int x = startX + col * cellWidth;
               int y = calStartY + headerHeight + row * cellHeight;
        
               // Mettre en évidence le jour actuel (case inversée : fond noir, texte blanc)
               if (day == currentDay) {
                   epd.fillRect(x + 1, y + 1, cellWidth - 1, cellHeight - 1, GxEPD_BLACK);
                   epd.setTextColor(GxEPD_WHITE);
                  }
        
               epd.getTextBounds(dayStr, 0, 0, &tbx, &tby, &tbw, &tbh);
               int textX = x + (cellWidth - tbw) / 2;
               int textY = y + cellHeight / 2 + tbh / 2;
               epd.setCursor(textX, textY);
               epd.print(dayStr);
        
               epd.setTextColor(GxEPD_BLACK);
               day++;
              }
          }
  
         Serial.println("Affichage terminé");
  
}

int getDaysInMonth(int month, int year) {  
    switch(month) {
    case  0: return 31;
    case  1: return isLeapYear(year) ? 29 : 28;
    case  2: return 31;
    case  3: return 30;
    case  4: return 31;
    case  5: return 30;
    case  6: return 31;
    case  7: return 31;
    case  8: return 30;
    case  9: return 31;
    case 10: return 30;
    case 11: return 31;
    default: return 30;
  }
}

bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


//
