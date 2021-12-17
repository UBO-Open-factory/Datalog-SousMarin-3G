// 
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Datalogger (partie émission des données)
//                        
// - Description  : Ce programme est intégré dans un datalogger chargé de mesurer certains paramètres de l'eau
//                  d'un fleuve et de les envoyer par FTP sur un serveur de données. Il n'intègre que la partie
//                  envoie des données. Celles-ci sont contenues dans des fichiers stockés sur la carte SD.
//                  Ce programme ne réalise ni les mesures ni leur stockage sur la carte SD.
//                  La carte SD ne contient que des fichiers exemple. 
//
//                  Le programme est divisé en 4 parties :
//                       - 1:initialisation : qui permet la configuration des périphériques
//                       - 2:mesures : mesure des paramètres de l'eau (non traité dans ce programme)
//                       - 3:envoi des données
//                       - 4:mise en sommeil
//
//                  Architecture matérielle : 
//                      le logger est architecturé autour d'un FireBeetle ESP-32 relié à
//                        - une RTC (DS3231)
//                        - un lecteur de carte SD (ADAFRUIT)
//                        - un module GSM SIM5320E (Fona 3G)
//                        - un module IIC vers 2 UART (Gravity) pour le dialogue avec un capteur et le module GSM
//                        - des capteurs (non intégrés dans ce programme)
//                  
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
//                  Modification par rapport à la version 0.4
//                  -----------------------------------------
//                  1 - Stabilisation des commandes
//                  2 - Résolution de certains bugs concernant le choix de l'envoi d'un fichier
//                  3 - insertion d'une variable en fichier ini : intervalle_entre_mesures
//
//
//                  Modification par rapport à la version 0.3
//                  -----------------------------------------
//                  1- insertion d'une simulation de mesure (bibliothèque)
//                      cette bibliothèque comporte toutes les fonctions permettant de simuler les mesures et leur sauvegarde sur la carte SD
//                      Les mesures sont fictives, seule la date et l'heure sont correctes. Cette fonction renvoie sa durée en ms.
//
//                  2 - insertion de la mise en sommeil pendant une durée de 15 minutes moins les durées des mesures et de la transmission pour avoir des mesures régulières toutes les 15 minutes.
//
//                  Modification par rapport à la version 0.2
//                  -----------------------------------------
//                  1-insertion de variables autorisant ou non l'affichage des traces de débug dans la console
//
//
//
//                  Modification par rapport à la version 0.1
//                  -----------------------------------------
//                  1-Lit les données sur la carte SD par paquets de 'taille_du_buffer_lecture_carte_sd' et non octet par octet .
//                    Envoie toujours les données en FTP octet par octet.
//
//                      Comparaison (pour un fichier de 154360 octest)
//                                      lecture        envoi              durée
//                                    (nb d'otets)   (nb d'otets)          (ms)
//                                          1            1                143650
//                                        1024           1                135311
//                                        1024          1024               64296
//                                          '             '                63548
//                                          '             '                64285
//                                        2048          2048               63438
//
//                                      un buffer de 1ko est un bon compromis.
//                                    Durée estimée : 2,4 ko/s
//
//                  2 - modification de l'intitulé des fonctions de la bibliothèque 'uof_iic_to_dual_uart.h'
//                        Ajout de 'uof_' devant le nom de chaque fonction
//
// - anciennes versions : 
//                  Initialisation du Firebeetle l'ESP 32
//                  Initialisation des ports de communication (uart/IIC..)
//                  Initialisation RTC (DS3231)
//                  Initialisation carte SD
//                  Appel de la fonction de gestion de l'envoi des données/ Si un fichier peut et doit être envoyé
//                        Initialisation du GSM
//                        Configuration du GSM
//                        Configuration de la liaison FTP
//                        Envoi du fichier octet par octet (on lit un octet dans le fichier source sur la carte SD et on l'envoi par FTP)
//                  Une seule tentative d'initialisation/configuration/envoi.
//                        
//                        
//  -----------------------------------------------------------------------------------------------------------


  #include "configuration_ESP.h"
  #include "uof_variables_globales.h"
  #include "uof_iic_to_dual_uart.h"
  
  #include "uof_rtc.h"
  #include "uof_sd.h"
  #include "uof_gsm.h"
  #include "uof.h"
  #include "uof_gestion_des_envois.h"

  #include "uof_simulation.h"
  






  

 // ------------------------------------------------------------------
 //          SETUP
 // ------------------------------------------------------------------
 
void setup() {
 
  bool initialisation=OK;

  randomSeed(analogRead(0));
  
  // initialisation de la liaison USB avec un PC (si connecté)
    uof_initialisation_usb();
Serial.println (" ---> ");

  // initialisation I2C
    if (affiche_debug) Serial.print ("    I2C                                 : ");
    if (!uof_initialisation_i2c()) {
      if (affiche_debug) Serial.println ("Echec");
      initialisation=KO;
    } else {
      if (affiche_debug) Serial.println ("OK");
    }

  // initialisation de la RTC
    if (affiche_debug) Serial.print ("    RTC                                 : ");
    if (!uof_initialisation_rtc()) {
      if (affiche_debug) Serial.println ("RTC absente ou endomagée");
      initialisation=KO;
    } else {
      
/* Les 2 lignes suivantes permettent de définir l'heure de la RTC */
//uof_set_date(15,9,21);
//uof_set_time(15,55,30);
     
      if (affiche_debug) Serial.println ("OK");
      if (affiche_debug) Serial.print ("                               Date RTC : ");
      if (affiche_debug) uof_affiche_date();
      if (affiche_debug) Serial.println ("");
      if (affiche_debug) Serial.print ("                              Heure RTC : ");
      if (affiche_debug) uof_affiche_time();
      if (affiche_debug) Serial.println ("");
    }
 
  //  initialisation de la carte SD
    if (affiche_debug) Serial.print ("    carte SD                            : ");
    
    if (!uof_initialisation_sd()) {
      /*if (affiche_debug)*/ Serial.println("Echec ou carte non présente.");
      initialisation=KO;
    } else {
      if (affiche_debug) Serial.println("OK");
      //
      if (affiche_debug) Serial.print ("    Lecture du fichier de configuration : ");
      lecture_fichier_config();
      if (affiche_debug) Serial.println ("OK");
    }

  // fin de l'initialisation
    if (affiche_debug) Serial.print ("    TERMINEE                            : ");
    if (initialisation) {
      if (affiche_debug) Serial.println ("OK");
    } else {
      /*if (affiche_debug)*/ Serial.println ("ECHEC - Arrêt du processus");
    }
    if (affiche_debug) Serial.println ("--------------------------------------------------------------");
 
//
//    MESURES
//
  if (initialisation) {
    // mesures
    if (autorise_mesure) { 
      if (affiche_debug) Serial.println ("MESURES");
      duree_mesure = uof_simulation_de_mesure();
      if (affiche_debug)Serial.println ("--------------------------------------------------------------");
    } else {
      if (affiche_debug) Serial.println ("Mesures non autorisée");
    }
  
//
//    TRANSMISSION
//
    // Gestion de l'envoi des données
    if (autorise_transmission) {
      if (affiche_debug) Serial.println ("TRANSMISSION");
      duree_transmission = uof_gestion_de_l_envoi_des_donnees();
    } else {
      if (affiche_debug) Serial.println ("Transmission non autorisée");
    }
  }

  // mise en sommeil
    if (affiche_debug) Serial.println ("MISE EN SOMMEIL");
    Serial.print ("Intervalle entre mesures : ");
    Serial.println (intervale_entre_mesures);
    unsigned long duree_sommeil = (intervale_entre_mesures*60*1000000)-(duree_transmission+duree_mesure);

if (affiche_debug) {
  Serial.print ("    La mesure à pris : ");
  Serial.print (duree_mesure);
  Serial.println (" ms");
  Serial.print ("    La transmission des données à pris : ");
  Serial.print (duree_transmission);
  Serial.println (" ms");

  Serial.print ("Mise en sommeil pour : ");
  Serial.print (duree_sommeil/1000);
  Serial.println ("ms");
  
}
  Serial.println ("------------------------------------------------------------");
     esp_deep_sleep(duree_sommeil);
  
  
}

 // ------------------------------------------------------------------
 //          LOOP
 // ------------------------------------------------------------------
 //
void loop() {

}
