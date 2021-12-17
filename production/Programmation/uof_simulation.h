//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Fonctions pour les besoins de développement : simulation des mesures
//                                          et enregistrement de ces mesures sur la carte SD
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
//                  String         uof_genere_mesure()
//                  String         uof_nom_du_fichier_pour_les_mesures()
//
//
//
//                  unsigned long  uof_simulation_de_mesures()
//
//  -----------------------------------------------------------------------------------------------------------


String uof_genere_mesure() {
  //
  //  génère une ligne d'enregistrement de mesures
  //  à partir de la date de l'heure et de mesures aléatoires
  //
  // format d'une ligne : 2021-09-24 11:16:14;12;1234;1.12;12.12;12.12;1234;123.123;-123.123;-123.123;123.12;123.12
  //
  String res="";
  uof_get_date();
  uof_get_time();

  res = res + uof_annee_sur_4_chr(Annee);
  res = res + "-";
  res = res + uof_mois_sur_2_chr(Mois);
  res = res + "-";
  res = res + uof_jour_sur_2_chr(Jour);
  res = res + " ";
  res = res + uof_heure_sur_2_chr(Heure);
  res = res + ":";
  res = res + uof_heure_sur_2_chr(Minute);
  res = res + ":";
  res = res + uof_heure_sur_2_chr(Seconde);
  res = res + ";";

  res = res + "45;1234";


  
  //res = res + uof_heure_sur_2_chr(random(5, 75));
  //res = res + ";";
  //res = res + uof_heure_sur_2_chr(random(98));
  //res = res + uof_heure_sur_2_chr(random(99));
  //res = res + ";";
  //res = res + "1.12;12.12;12.12;1234;123.123;-123.123;-123.123;123.12;123.12";
  
  return res;
}


String uof_nom_du_fichier_pour_les_mesures(){
  //
  //    Cette fonction établit le fichier dans lequel on devra enregistrer les données mesurées
  //    Elle utilise l'heure et la date de la RTC ainsi que les variables  
  //
  //      'uof_tab_heure_des_envois[24]
  //      'uof_nb_heure_des_envois
  //      'id_logger'
  //
  //    Renvoie 
  //        - Si ok un nom de fichier formaté : nnnnn_aaaammjj_hh.csv   
  //                        nnnnn = nom du logger (nombre de caractères variables) lu dans config.txt
  //                        aaaa  = année sur 4 digits
  //                        mm    = mois sur 2 digits 
  //                        jj    = jour sur 2 digits
  //                        hh    = heure sur 2 digits (0 à 23)
  //        - si KO la chaine :  "[AUCUN]"  
  //                        
  //                        
  uof_get_heure_des_envois(); // récupère les heures d'envois dans le fichier config.txt et les place dans un tableau
  String res = id_logger;
  // lit la date actuelle
  uof_get_date(); // lit la datedans la RTC et place les 3 valeurs dans des variables globales : Annee, Mois et Jour
  uof_get_time(); // lit l'heure dans la RTC et place les 3 valeurs dans des variables globales : Heure, Minute et Seconde
  // lit l'heure actuelle
  byte heu=uof_get_heure(); // lit l'heure actuelle
  // cherche l'indice de l'heure d'envoi précédente
  byte hf=255;    // pas d'heure trouvée
  if (heu < uof_tab_heure_des_envois[0]) {    // 'uof_tab_heure_des_envois' contient les heures d'envois définis dans le fichier 'config.txt'
    veille_de(&Jour, &Mois, &Annee);          // calcul la date de la veille de la date lue précédemment
    hf=uof_tab_heure_des_envois[uof_nb_heure_des_envois-1];
  }
  if (heu >=uof_tab_heure_des_envois[uof_nb_heure_des_envois-1]) {
    hf=uof_tab_heure_des_envois[uof_nb_heure_des_envois-1];
  }
  for (int i=0; i<uof_nb_heure_des_envois-1; i++) {
    if (heu >=uof_tab_heure_des_envois[i] and heu <uof_tab_heure_des_envois[i+1]) {
      hf = uof_tab_heure_des_envois[i];
    }
  }
  // Définit le nom du fichier dans lequel enregistrer les mesures
  if (hf !=255) {
  res = res +"_"+ uof_annee_sur_4_chr(Annee);
  res = res + uof_mois_sur_2_chr(Mois);
  res = res + uof_jour_sur_2_chr(Jour);
  res = res + "_" + uof_heure_sur_2_chr(hf);
  res = res + ".csv";
  } else {
    res="[AUCUN]";
  }
  return res;
}


unsigned long  uof_simulation_de_mesure() {
  debut = millis();
  String fic = "";
  // détermine le fichier dans lequel placer les mesures
  // réalise les mesures d'une ligne
  // écrit les mesures dans le fichier
  // détermine le fichier dans lequel placer les mesures
    fic = uof_nom_du_fichier_pour_les_mesures();

  // affiche date, heure et fichier de saisie pour la simulation
    uof_affiche_date();
    Serial.print (" - ");
    uof_affiche_time();
    Serial.print (" Mesures dans >>>>  ");
    Serial.print (fic);
    Serial.println("");

 
 // réalise les mesures d'une ligne
  fic = "/data/"+fic;
  //générateur de mesures fictives
  // ouvre le fichier en écriture
    myFile = SD.open(fic, FILE_APPEND);
  for (int k=0; k<10; k++) {
    String mesu=uof_genere_mesure();
    //Serial.print ("    ");
    //Serial.println (mesu);
    // écrit les mesures dans le fichier
    myFile.println (mesu);
    delay(50);
  }

  // ferme le fichier
  myFile.close();
  delay(1000);
  // durée de la procédure
  unsigned long duree = millis()-debut;

  return duree;  // ne sera pas affecté par le retour à 0 du compteur millis() au bout de 52 jours puisque le module est mis en deep sleep après chaque mesure/transmission
}
