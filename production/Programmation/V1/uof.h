//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Fonctions pour les besoins de développement
//                        
// - Description  : Ces fonctions sont nécessaires pour le fonctionnement du programme. Elles concernent
//                  essentiellement l'initialisation des différents périphériques de l'ESP-32 
//
// - Auteur       : Laurent Marchal(UBO OpenFactory) - Arthur Pignalet(UBO OpenFactory)
// 
// - Date         : sept 2021
// 
// - Licence      : CC-BY-SA
// 
// - Version      : 1.0 
// 
// - Contenu      :
//  
//                  bool uof_initialisation_sd()
//                  bool uof_initialisation_i2c()
//                  bool uof_initialisation_rtc()
//                  bool uof_initialisation_usb()
//
//  -----------------------------------------------------------------------------------------------------------

// initialisation carte SD
bool uof_initialisation_sd() {
  SD.begin(UOF_CSPIN_FOR_SD);
  delay(50);
  if (!SD.begin(UOF_CSPIN_FOR_SD)) {              // see if the card is present and can be initialized
      return KO;
  } else {
    return OK;
  }
}

// initialisation I2C
bool uof_initialisation_i2c() {
    if (!Wire.begin()) {
      return KO;
    } else {
      return OK;
    }
}

// initialisation de la RTC
bool uof_initialisation_rtc() {
  
  uof_get_date();
  uof_get_time ();

  if (Jour >31 or Mois >12 or Heure >24 or Minute >59) {
    return KO;
  }
  return OK;
}

// initialisation de la liaison USB de l'ESP
bool uof_initialisation_usb() {
  Serial.begin(USB_SERIAL_RATE);
  delay(100);
  /*if (affiche_debug)*/ Serial.println ("--------------------------------------------------------------");
  /*if (affiche_debug)*/ Serial.println ("INITIALISATION");
  if (affiche_debug) Serial.print("    liaison USB (Serial...)             : ");
  debut = millis();
  time_out = 5000;
    while(!Serial){
      if (affiche_debug) Serial.print(".");
      delay(100);
      if (millis() - debut > time_out) {
        return KO;
      }
    }
  if (affiche_debug) Serial.println("OK");
}
