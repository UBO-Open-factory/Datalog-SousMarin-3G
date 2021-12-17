//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Pilotage du lecteur de carte SD
//                        
// - Description  : Ces fonctions servent à piloter le module RTC.
//                  Outre les fonctions de base de la bibliothèque, ce module intègre des fonctions plus
//                  évoluées : effacement ou copie d'un fichier, lecture et exploitation d'un fichier "ini"
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
//                  void uof_efface_fichier(String fichier)
//                  void lecture_fichier_config()
//                  void uof_copie_fichier(String source, String destination)
//
//  Attention : l'appel d'un fichier à la racine se fait en le nommant "/fichier.txt" et non "fichier.txt"
//
//
//  -----------------------------------------------------------------------------------------------------------


#include <SPI.h>                        // pour connection ecran bus SPI
#include <SD.h>                         // pour carte SD
//#include <SDConfigFile.h>

// variables

#define UOF_CSPIN_FOR_SD  5   // IO5
File myFile;
File ss;
File dd;

String fichier_config = "/config.txt";




void uof_efface_fichier(String fichier) {
  //
  // fichier = chemin + nom de fichier
  //
  SD.remove(fichier);
}



void lecture_fichier_config() {
 // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(fichier_config, FILE_READ);

  char phrase[200];
  byte index = 0;
  char x=0;
  String reste = "";
  int k=0;
  
  if (myFile) {
    
    while (myFile.available()) {
      x = myFile.read();
      if (x!=10){
        if (x!=13) {
          phrase[index] = x;
          index++;
        } else {
          if (index != 0) {
            reste = phrase;
            reste = reste.substring(0,index);
            index=0;
            // suppression des commentaires
            k = reste.indexOf(";");
            if (k!=0) {
              if (k!=-1) {
                reste = reste.substring(0,k);
              }
              // supprime les espaces au début et à la fin
              reste.trim();
              //
              // extrait les valeurs pour les placer dans les variables du programme
              //
              if (reste.indexOf('=') >0) {
                // signe égal trouvé
                String clef = reste.substring(0,reste.indexOf('='));
                String valeur = reste.substring(reste.indexOf('=')+1);
                if (clef == "id_logger") id_logger = valeur;
                if (clef == "gsm_code_pin") gsm_code_pin = valeur;
                if (clef == "heure_des_envois") heure_des_envois = valeur;
                if (clef == "serveur_adresse") serveur_adresse = valeur;
                if (clef == "serveur_port") serveur_port = valeur;
                if (clef == "serveur_user") serveur_user = valeur;
                if (clef == "serveur_pwd") serveur_pwd = valeur;
                if (clef == "intervale_entre_mesures") intervale_entre_mesures = valeur.toInt();
              }
            }
          }          
        }
      }
     }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    if (affiche_debug) Serial.println("error opening "+fichier_config);
  }
}


void uof_copie_fichier(String source, String destination) {
  //
  //  copie le fichier source vers le fichier destination
  //
  if (affiche_debug) Serial.println ("Copie du fichier");
  if (affiche_debug) Serial.println ("     '"+source+"' vers '"+destination+"'");
  unsigned long debut = millis();
  
  ss = SD.open(source,FILE_READ);
  dd = SD.open(destination,FILE_APPEND);
  while (ss.available()){
    dd.write(ss.read());
  }
  dd.close();
  ss.close();
  
  unsigned long duree = millis() - debut;
  if (affiche_debug) Serial.print ("     (Durée :");
  if (affiche_debug) Serial.print (duree);
  if (affiche_debug) Serial.println (" ms)"); 
}
