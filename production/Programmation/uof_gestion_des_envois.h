//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Gestion des envois
//                        
// - Description  : Ces fonctions servent à gérer l'envoi des données.
//                  Elles permettent de savoir, à partir de la date, de l'heure et de la liste des horaires d'envoi, 
//                  si un fichier peut être envoyé, s'il ne l'a pas déjà été et de l'envoyer.//
//                  L'envoi d'un fichier se fait après la configuration du GSM puis la configuration du FTP.
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
//                  bool          uof_envoi_du_fichier_en_FTP(String fichier)
//                  String        get_nom_fichier_a_envoyer()
//                  bool          uof_le_fichier_a_t_il_deja_ete_envoye()
//                  unsigned long uof_gestion_de_l_envoi_des_donnees()
//
// ------------------------------------------------------------------

bool uof_envoi_du_fichier_en_FTP(String fichier){
  if (affiche_debug) Serial.println ("    Envoi du fichier \""+fichier+"\" en FTP.");
  bool fctloc=OK;
  // initialisation des E/S
  uof_gsm_init();
  // initialisation de la liaison via le module Gravity
  if (!uof_initialisation_liaison_serie_gsm(10000)) fctloc=KO;
  // si le GSM est sous tension, on l'éteint
  if (uof_etat_gsm()){
    if (affiche_debug) Serial.println ("        => le GSM est sous tension, on l'éteint pour résoudre les pb");
    uof_mise_hors_tension_gsm(20000);
  }
  
  // mise sous tension du GSM
  if (fctloc) {
    if (!uof_mise_sous_tension_gsm(20000)) fctloc=KO;
  }

  // configuration du GSM
  if (!uof_configuration_GSM(fctloc)) fctloc=KO;

  // configuration du canal FTP
  if (!uof_configuration_FTP(fctloc)) fctloc=KO;

  // envoi du fichier
  bool xxxx = uof_envoi_donnees(uof_fichier_a_envoyer);
  if (!xxxx) fctloc=KO;

   
  // TODO fermeture du canal FTP  (???)  et fermeture du socket (netclose)


  // Enregistrement du C/R d'envoi dans last_file_sent
  String res_envoi = uof_fichier_a_envoyer+",";
  if (xxxx) {
    res_envoi = res_envoi + "OK";
  } else {
    res_envoi = res_envoi + "KO";
  }
  File myFile2;
  myFile2 = SD.open("/last_file_sent.txt",FILE_WRITE);  // une seule ligne 
  myFile2.print(res_envoi);
  myFile2.close();


  // Extinction du GSM
  if (!uof_mise_hors_tension_gsm(10000)) fctloc=KO;

 
  return fctloc;
}

String get_nom_fichier_a_envoyer(){
  // détermine le nom du fichier à envoyer en fonction des paramètres :
  //      heure actuelle
  //      id_logger
  //      date actuelle
  String res = id_logger+"_";
  // lit la date actuelle
  uof_get_date();
  if (affiche_debug) Serial.print ("    Date RTC       : ");
  uof_affiche_date();
  if (affiche_debug) Serial.print ("  -  ");
  // lit l'heure actuelle
  byte heu=uof_get_heure();
  byte heure_finale = 0;
  if (affiche_debug) Serial.print ("    Heure actuelle : ");
  if (affiche_debug) Serial.print (heu);
  if (affiche_debug) Serial.print ("  ");
  // trouve l'index de l'heure la plus proche avant l'heure actuelle dans le tableau uof_tab_heure_des_envois
  if (uof_nb_heure_des_envois == 1) {
    // on ne fait qu'un envoi par jour soit celui de la veille 
    if (heu < uof_tab_heure_des_envois[0]) {
      // ou celui de l'avant veille
      veille_de(&Jour, &Mois, &Annee);
    }
    veille_de(&Jour, &Mois, &Annee);
    heure_finale = uof_tab_heure_des_envois[0];  
  } else {
    if (heu < uof_tab_heure_des_envois[0]) {
      // si l'heure est avant la première heure d'envoi et qu'il y a deux heures d'envoi au moins 
      veille_de(&Jour, &Mois, &Annee);
      heure_finale = uof_tab_heure_des_envois[uof_nb_heure_des_envois-2];
    } else {
      // sinon on vérifie si elle est comprise entre deux heures d'envoi
      bool okk = false;
      for (int i=0; i<=uof_nb_heure_des_envois-2; i++) {
        if (heu>=uof_tab_heure_des_envois[i] and heu < uof_tab_heure_des_envois[i+1]) {
          if (i == 0) {
            veille_de(&Jour, &Mois, &Annee);
            heure_finale = uof_tab_heure_des_envois[uof_nb_heure_des_envois-1];
            
          } else {
            heure_finale = uof_tab_heure_des_envois[i-1];
          }
          okk = true;
        }
      }
      if (!okk) {
        // sinon elle est postérieure à la dernière heure d'envoi.
        heure_finale = uof_tab_heure_des_envois[uof_nb_heure_des_envois-2];
      }
    }
  }
  res = res + uof_annee_sur_4_chr(Annee);
  res = res + uof_mois_sur_2_chr(Mois);
  res = res + uof_jour_sur_2_chr(Jour);
  res = res + "_";
  res = res + uof_heure_sur_2_chr(heure_finale);
  res = res + ".csv";
  return res;
}

bool uof_le_fichier_a_t_il_deja_ete_envoye() {
  //
  //
  //
  //
  String sss;
  String fichier_lu = "";
  String compte_rendu = "";
  if (affiche_debug) Serial.print ("        Le fichier 'last_file_sent' existe sur la carte SD : "); 
  if (!SD.exists("/last_file_sent.txt")) {
    // le fichier last_file_sent n'existe pas
    if (affiche_debug) Serial.println ("NON");
    // on le crée en inscrivant le nom du fichier et "??"
    if (affiche_debug) Serial.println ("        Création du fichier last_file_sent");
    myFile = SD.open("/last_file_sent.txt", FILE_WRITE);// write et non append pour n'enregistrer qu'une seule ligne
    // on renseigne le fichier avec les valeurs initial d'envoi du fichier
    if (affiche_debug) Serial.println ("        Renseignement par défaut du fichier avec "+uof_fichier_a_envoyer+" et \"??\"");
    sss = uof_fichier_a_envoyer + ",??";
    myFile.print(sss);
    // on ferme le fichier
    if (affiche_debug) Serial.println ("        Fermeture du fichier");
    myFile.close();
  } else {
    if (affiche_debug) Serial.println ("OUI");
  }
  // le fichier last_file_sent existe et est renseigné
  //  contenu fichierlu,?? ou OK ou KO
  //      ?? : on a pas encore essayé d'envoyer le fichier
  //      KO : on a essayé d'envoyer le fichier mais cela n'a pas fonctionné
  //      OK : on a essayé d'envoyer le fichier mais cela n'a pas fonctionné
  if (affiche_debug) Serial.println ("    Récupération des informations du fichier last_file_sent.txt");
  myFile = SD.open("/last_file_sent.txt", FILE_READ);
  sss = myFile.readString();
  myFile.close();
  if (affiche_debug) Serial.println ("     Contenu => "+sss);
  fichier_lu = sss.substring(0,sss.indexOf(","));
  compte_rendu = sss.substring(sss.indexOf(",")+1);
  if (affiche_debug) Serial.println ("     Fichier lu : "+fichier_lu);
  if (affiche_debug) Serial.println ("     CR         : "+compte_rendu);
  if (affiche_debug) Serial.println ("");
  if (affiche_debug) Serial.println ("A envoyer : "+ uof_fichier_a_envoyer + "         Fichier lu : "+  fichier_lu);
  if (uof_fichier_a_envoyer != fichier_lu){
    return false;
  }else {
    if (compte_rendu == "OK") {
      return true;
    } else {
      return false;
    }
  }
}

unsigned long uof_gestion_de_l_envoi_des_donnees() {
  //
  //  Détermine quel fichier doit être envoyé
  //  Détermine s'il doit être envoyé
  //  si oui
  //      gestion du fichier last_file_sent
  //      le copie sur la carte SD
  //      Essaye d'envoyer le fichier
  //      Renseigne le CR
  //
  //  si non
  //      ne fait rien
  //
  //
  unsigned long debut_emission = millis(); // pour mesurer la durée de la fonction
  unsigned long fin_emission   = 0;
  unsigned long duree_emission = 0;
  String ss="";
  String fichier_a_envoyer="";
  String fichier_lu = "";
  String compte_rendu = "";
  
  // construit le tableau des heures d'envoi
  uof_get_heure_des_envois();

  // détermine quel fichier doit être envoyé (il est impératif de conserver les 2 variables suivantes)
  fichier_a_envoyer = get_nom_fichier_a_envoyer();
  uof_fichier_a_envoyer = fichier_a_envoyer;

  if (affiche_debug) Serial.println ("    Quel fichier est (peut être) à envoyer : "+fichier_a_envoyer);
  // détermine si le fichier a déjà été envoyé
  if (affiche_debug) Serial.println ("    Détermine si le fichier a déjà été envoyé : ");
  if (uof_le_fichier_a_t_il_deja_ete_envoye()) {
    if (affiche_debug) Serial.println ("    Le fichier à déjà été envoyé avec succès.");
  } else {
    if (affiche_debug) Serial.println ("    Le fichier n'a pas encore été envoyé avec succès ou n'a pas encore été envoyé");
    // copie le fichier dans tmp_send
    if (!SD.exists("/data/"+fichier_a_envoyer)) {
      if (affiche_debug) Serial.println ("        Le fichier de données n'existe pas dans '/data'");
      if (affiche_debug) Serial.println ("        Il n'y a pas de fichier à envoyer.");
    } else {
      if (affiche_debug) Serial.println ("        Copie le fichier de 'data' vers 'temp_send'");
      uof_copie_fichier ("/data/"+fichier_a_envoyer,"/tmp_send/"+fichier_a_envoyer);
      if (affiche_debug) Serial.println ("    On essaye de l'envoyer...");
      // envoi du fichier en FTP
      if (autorise_emission) {
        bool res_envoi_fichier = uof_envoi_du_fichier_en_FTP(fichier_a_envoyer);
        if (res_envoi_fichier) {
          // le fichier à bien été envoyé
          if (affiche_debug) Serial.println ("La tentative d'envoi du fichier en FTP a réussie.");
        } else {
          if (affiche_debug) Serial.println ("La tentative d'envoi du fichier en FTP est un ECHEC");
        }
      } else {
        if (affiche_debug) Serial.println ("Emission non autorisée");
      }
    }
  }
  // efface le fichier temp si il existe
  if (affiche_debug) Serial.println ("    Efface le fichier /tmp_send/"+fichier_a_envoyer+" si il existe");
  if (SD.exists("/tmp_send/"+fichier_a_envoyer)) {
    if (affiche_debug) Serial.println ("        ... effacé");
    SD.remove ("/tmp_send/"+fichier_a_envoyer);
  } else {
    if (affiche_debug) Serial.println ("        Pas de fichier à effacer");
  }
  
  // calcul de la durée de la procédure
  fin_emission = millis();
  if (fin_emission<debut_emission){
    duree_emission = 4294967296-(debut_emission+fin_emission);
  } else {
    duree_emission = fin_emission-debut_emission;
  }
  if (affiche_debug) Serial.println ("    FIN DE LA TRANSMISSION :  ");
  return duree_emission;
}
