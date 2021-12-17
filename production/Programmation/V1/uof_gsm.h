//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Gestion des envois
//                        
// - Description  : Ces fonctions servent à piloter le module FonaMiniGSM
//                  Elles permettent la configuration des différentes fonctions du GSM (mise sous/hors tension
//                  initialisation, connection au réseau, identification, configuration du FTP, transfert de données
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
//                    bool   uof_etat_gsm()
//                    bool   uof_initialisation_liaison_serie_gsm(unsigned long timeout)
//                    void   uof_gsm_init()
//                    String uof_enleve_CR(String xx)
//                    String uof_attend_reponse_GSM(unsigned long timeout)
//                    bool   uof_mise_sous_tension_gsm(unsigned long timeout)
//                    bool   uof_mise_hors_tension_gsm(unsigned long timeout)
//                    void   uof_gsm_serial_send(String commande)
//                    bool   uof_gsm_commande_CPIN(bool fct, unsigned long Timeout)
//                    bool   uof_gsm_commande_COPS(unsigned long Timeout)
//                    bool   uof_gsm_commande_CGSOCKCONT(unsigned long Timeout){
//                    bool   uof_gsm_commande_CSOCKSETPN(unsigned long Timeout)
//                    bool   uof_gsm_commande_CIPMODE(unsigned long Timeout)
//                    bool   uof_configuration_GSM(bool fct)
//
//                    bool   uof_gsm_commande_CFTPSERV(unsigned long Timeout)
//                    bool   uof_gsm_commande_CFTPPORT(unsigned long Timeout)
//                    
//                    bool   uof_gsm_commande_CFTPUN(unsigned long Timeout)
//                    bool   uof_gsm_commande_CFTPPW(unsigned long Timeout)
//                    bool   uof_configuration_FTP(boolfct)
//                    
//                    bool   uof_gsm_commande_CFTPPUT(String fichier, unsigned long Timeout)

//                    bool   uof_gsm_fin_de_transmission(unsigned long Timeout)
//                    bool   uof_envoi_donnees(String fichier)
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//                    
//  -----------------------------------------------------------------------------------------------------------


//
//
//
//      Les commandes :
//        configuration GSM
//            AT+CPIN=1234
//            AT+CGSOCKCONT=1,"IP","free"
//            AT+CSOCKSETPN=1
//            AT+CIPMODE=0
//            AT+NETOPEN
//        configuration FTP
//            AT+CFTPSERV="xxx"
//            AT+CFTPPORT=xxxx
//            AT+CFTPUN="xxx"
//            AT+CFTPPW="xxx"
//        envoi des données
//            AT+CFTPPUT="coucou.txt" 
//             data ... data ... data ...
//             CTRL+Z
//        Fermeture du socket
//             AT+NETCLOSE
//
//
//
//
//
//
//
















// Pin mapping
#define GSM_KEY 16
#define GSM_PS  39

// Status module GSM
#define GSM_ON  1
#define GSM_OFF 0

// Definition des paramétre GSM
#define ERREUR               9999
#define GSM_NOCONNECT           0
#define GSM_CONNECTED           1
#define GSM_HTTPCONNECT         2
#define GSM_TMIN_NOCONNECT    700
#define GSM_TMAX_NOCONNECT    900
#define GSM_TMIN_CONNECTED   2700
#define GSM_TMAX_CONNECTED   3300
#define GSM_TMIN_HTTPCONNECT  200
#define GSM_TMAX_HTTPCONNECT  400

// definition de la vitsee de communication des port SERIE 
#define GSM_SERIAL_RATE 115200


// tableau des opérateurs trouvés par le module FONA 
struct operateur {
    byte stat;        // 0 à 3
    String nom;       // nom complet de l'opérateur
    String reseau;    // nom du réseau
    String id;        // identifiant international 
};

String id_operateur_choisi=""; // est renseigné par la procédure commande_get_COPS

// compteur 
String str="";
byte cptr=0; 


/******************************************************************************
 * 
 *          FONCTIONS
 *
 *****************************************************************************/


bool uof_etat_gsm() {
  // renvoie true si la carte FONA est ON
  // renvoie false si la carte FONA est OFF
  if (digitalRead(GSM_PS) == HIGH) {
    return true;
  } else {
    return false;
  }
}

bool uof_initialisation_liaison_serie_gsm(unsigned long timeout) {
  if (affiche_debug) Serial.print ("        initialisation UART via Gravity : ");
  //SET FONA.iicUART1
  uof_iicSerialBegin(FONA_UART,USB_SERIAL_RATE);

  // Vérification fonctionnement
  debut = millis();
  if(!uof_iicSerialAvailable(FONA_UART)){
    if (millis() - debut > timeout) {
      if (affiche_debug) Serial.println ("Time Out : ");
      return KO;
    }
  }
  if (affiche_debug) Serial.println ("OK");
  return OK;
}

void uof_gsm_init() {
  // initialisation des liaisons discrètes avec le GSM hors liaison série
  if (affiche_debug) Serial.println ("        Initialisation des ports E/S vers le GSM");
  pinMode(GSM_KEY,OUTPUT);
  pinMode(GSM_PS,INPUT);
  digitalWrite(GSM_KEY,HIGH);
}

String uof_enleve_CR(String xx) {
  while (true) {
    int l=xx.length();
    if (xx.charAt(l-1)==13){
      xx=xx.substring(0,l-1);
    } else {
      return xx;
    }
  }
}

String uof_attend_reponse_GSM(unsigned long timeout) {
  // renvoie une chaine reçue du module GSM si on n'a pas dépassé le time out
  // renvoie "TimeOut" si time out dépassé
  debut = millis();
  String recu = "";
  
  while (millis() - debut < timeout) {
    if (uof_iicSerialAvailable(1)){
      recu = recu+char(uof_iicSerialRead(1));
    }
    int l=recu.length();
    if (recu.charAt(l-2)==13 and  recu.charAt(l-1)==10  ) {
      recu = recu.substring(0,l-2); // enlève les deux derniers caractères (chr(13) et chr(10))
      recu = uof_enleve_CR(recu);
      if (affiche_debug) Serial.println ("              Reçu : "+recu);
      return recu;
    }
  }
  return "TimeOut";
}

bool uof_mise_sous_tension_gsm(unsigned long timeout) {
  // envoie la commande de mise sous tension
  if (affiche_debug) Serial.println ("        Mise sous tension GSM           : ");
  if (!uof_etat_gsm()) {
    // le Fona est sur OFF , commande la mise en marche
    // commande Key sur Low
    digitalWrite(GSM_KEY,LOW);
    debut = millis();
    while (!uof_etat_gsm()){
      if (millis() - debut > timeout) {
        if (affiche_debug) Serial.println ("        => Time out renvoyé par la fonction mise sous tension");
        digitalWrite(GSM_KEY,HIGH);
        return KO;
      }
    }
    digitalWrite(GSM_KEY,HIGH);
    // si la broche PS est passée à 1 (GSM sous tension
    // on attend les messages prévus
    
    while (true) {
      str=uof_attend_reponse_GSM(timeout);
      if (str == "START") cptr+=1;
      if (str == "+CPIN: SIM PIN") cptr += 1;
      if (str == "TimeOut") {
        if (affiche_debug) Serial.println ("        => Time Out renvoyé par le GSM");
        return KO;
      }
      if (cptr == 2){
        if (affiche_debug) Serial.println ("        => OK");
        return OK;
      }
      if (millis() - debut > timeout) {
        if (affiche_debug) Serial.println ("        => Time out renvoyé par la fonction mise sous tension");
        return KO;
      }
    }    
  } else {
    if (affiche_debug) Serial.println ("        => Le GSM est déjà sous tension");
    return OK;
  }

}

bool uof_mise_hors_tension_gsm(unsigned long timeout) {
  // envoie l commande de mise hors tension
  // le timeout doit être supérieur à 7000);
  if (affiche_debug) Serial.print ("        Mise hors tension GSM           : ");
  if (uof_etat_gsm()) {
    // le Fona est sur OFF , commande la mise en marche
    // commande Key sur Low
    while (uof_etat_gsm()){
      // commande hard d'extinction
      digitalWrite(GSM_KEY,LOW);
      delay(3000);
      digitalWrite(GSM_KEY,HIGH);
      delay(1000);
    }
    if (affiche_debug) Serial.println ("OK");
    return OK;
  } else {
    if (affiche_debug) Serial.println ("Le GSM est déjà hors tension");
    return OK;
  }
}

void uof_gsm_serial_send(String commande) {
  if (affiche_debug) Serial.println ("              Envoi ["+commande+"] :");
  String *pcommande=&commande;
  uof_iicSerialPrintln(1, pcommande);
}

bool uof_gsm_commande_CPIN(unsigned long Timeout){
  //
  //    vérifie si le code PIN a déjà été envoyé
  //    si oui (+CPIN:xxxx / OK)  => return OK
  //    si non (ERROR)            => return KO
  //    si +CME ERROR             => appelle affichage de la CME + return KO

  if (affiche_debug) Serial.println ("        Envoie le code PIN :");
  debut = millis();
  uof_gsm_serial_send("AT+CPIN="+gsm_code_pin);
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);  
    if (str.substring(0,8) == "AT+CPIN=")cptr+=1;
    if (str == "OK") cptr+=1;
    if (str == "+CPIN: READY") cptr+=1;
    if (str == "SMS DONE") cptr+=1;
    if (str.substring(0,11) == "+VOICEMAIL:") cptr+=1;
    if (str == "PB DONE") cptr+=1;
    if (cptr == 6) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_COPS(unsigned long Timeout){
  //
  //    
  if (affiche_debug) Serial.println ("        Choix de l'opérateur :");
  debut = millis();
  uof_gsm_serial_send("AT+COPS?");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+COPS?") cptr+=1;
    if (str.substring(0,7) == "+COPS: ") {
      // décompose la chaine pour récupérer les infos utiles
      // place le nom de l'opérateur dans 'nom_operateur_choisi'
      // extrait le nom de l'opérateur
      str = str.substring(str.indexOf(',')+1);  // supprime la première partie de la réponse ("+COPS : x,")
      str = str.substring(str.indexOf(',')+1);  // supprime la deuxième partie de la réponse ("x,")
      nom_operateur_choisi = str.substring(0,str.indexOf(','));  // ne garde que la troisième partie de la réponse : le nom ou l'id de l'opérateur
      cptr+=1; //si découpage ok
    }
    if (str=="OK") cptr+=1;
    if (cptr==3) {
      if (affiche_debug) Serial.println ("        => OK - opérateur choisi : " + nom_operateur_choisi);
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CGSOCKCONT(unsigned long Timeout){
  // définit le contexte du socket
  if (affiche_debug) Serial.println ("        Définit le contexte du socket :");
  debut = millis();
  uof_gsm_serial_send("AT+CGSOCKCONT=1,\"IP\","+nom_operateur_choisi);
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str.substring(0,14)=="AT+CGSOCKCONT=") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CSOCKSETPN(unsigned long Timeout){
  // Set active PDP socket profile number
  if (affiche_debug) Serial.println ("        Définit le contexte du socket :");
  debut = millis();
  uof_gsm_serial_send("AT+CSOCKSETPN=1");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CSOCKSETPN=1") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CIPMODE(unsigned long Timeout){
  // Set TCP/IP Application mode
  if (affiche_debug) Serial.println ("        Définit le contexte du socket :");
  debut = millis();
  uof_gsm_serial_send("AT+CIPMODE=0");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CIPMODE=0") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_NETOPEN(unsigned long Timeout){
  // Ouverture du socket
  if (affiche_debug) Serial.println ("        Définit le contexte du socket :");
  debut = millis();
  uof_gsm_serial_send("AT+NETOPEN");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+NETOPEN") cptr+=1;
    if (str=="OK") cptr+=1;
    if (str=="+NETOPEN: 0") cptr+=1;
    if (cptr==3) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_configuration_GSM(bool fct) {
  // appelle les fonctions qui configurent la liaison entre le GSM et le réseau
  // AT+CPIN=xxxx
  // AT+COPS? => nom et identifiant de l'opérateur pour la commande suivante
  // AT+CGSOCKCONT=1,"IP","free"
  // AT+CSOCKSETPN=1
  // AT+CIPMODE=0
  // AT+NETOPEN
  //
  // chaque fonction est monitorée pour s'assurée que les réponses attendues arrivent bien
  // un time out est défini pour chaque fonction affin de ne pas bloquer e système si une répons n'arrive pas.
  if (fct) if (!uof_gsm_commande_CPIN(10000)) fct=KO;         // envoie le code PIN
  if (fct) if (!uof_gsm_commande_COPS(10000)) fct=KO;         // définit l'opérateur choisi
  if (fct) if (!uof_gsm_commande_CGSOCKCONT(10000)) fct=KO;   // définit le contexte du socket
  if (fct) if (!uof_gsm_commande_CSOCKSETPN(10000)) fct=KO;   // Set active PDP socket profile number
  if (fct) if (!uof_gsm_commande_CIPMODE(10000)) fct=KO;      // Set TCP/IP Application mode
  if (fct) if (!uof_gsm_commande_NETOPEN(10000)) fct=KO;      // Ouverture du socket
  return fct;
}

bool uof_gsm_commande_CFTPSERV(unsigned long Timeout){
  // Définit l'adresse du serveur
  if (affiche_debug) Serial.println ("        Définit l'adresse du serveur :");
  debut = millis();
  uof_gsm_serial_send("AT+CFTPSERV=\""+serveur_adresse+"\"");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CFTPSERV=\""+serveur_adresse+"\"") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CFTPPORT(unsigned long Timeout){
  // Définit le port de communication du serveur
  if (affiche_debug) Serial.println ("        Définit le port de communication du serveur :");
  debut = millis();
  uof_gsm_serial_send("AT+CFTPPORT="+serveur_port);
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CFTPPORT="+serveur_port) cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CFTPUN(unsigned long Timeout){
  // Définit l'utilisateur
  if (affiche_debug) Serial.println ("        Définit le port de communication du serveur :");
  debut = millis();
  uof_gsm_serial_send("AT+CFTPUN=\""+serveur_user+"\"");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CFTPUN=\""+serveur_user+"\"") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_commande_CFTPPW(unsigned long Timeout){
  // Définit l'utilisateur
  if (affiche_debug) Serial.println ("        Définit le mot de passe :");
  debut = millis();
  uof_gsm_serial_send("AT+CFTPPW=\""+serveur_pwd+"\"");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(60000);
    if (str=="AT+CFTPPW=\""+serveur_pwd+"\"") cptr+=1;
    if (str=="OK") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_configuration_FTP(bool fct) {
  // Appelle les fonctions qui configurent la liaison FTP
  //          AT+CFTPSERV="xxx"
  //          AT+CFTPPORT=xxx
  //          AT+CFTPUN="xxx"
  //          AT+CFTPPW="xxx"
  if (fct) if (!uof_gsm_commande_CFTPSERV(10000)) fct=KO;          // Définit l'adresse du serveur
  if (fct) if (!uof_gsm_commande_CFTPPORT(10000)) fct=KO;          // Définit le port d'entrée du serveur
  if (fct) if (!uof_gsm_commande_CFTPUN(10000)) fct=KO;            // Définit l'utilisateur
  if (fct) if (!uof_gsm_commande_CFTPPW(10000)) fct=KO;            // Définit le mot de passe
  return fct;  
}

bool uof_gsm_commande_CFTPPUT(String fichier, unsigned long Timeout){
  // Définit l'utilisateur
  unsigned long duree = millis();
  if (affiche_debug) Serial.println ("        Définit le nom du fichier sur le serveur distant :");
  debut = millis();
  uof_gsm_serial_send("AT+CFTPPUT=\""+fichier+"\"");
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(120000);
    if (str=="AT+CFTPPUT=\""+fichier+"\"") cptr+=1;
    if (str=="+CFTPPUT: BEGIN") cptr+=1;
    if (cptr==2) {
      if (affiche_debug) Serial.print ("        => OK (Durée de la mise en relation : ");
      if (affiche_debug) Serial.print (millis() - duree);
      if (affiche_debug) Serial.println (")");         
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_gsm_fin_de_transmission(unsigned long Timeout){
  // Définit l'utilisateur
  unsigned long duree = millis();
  if (affiche_debug) Serial.println ("        fin de la transmission :");
  debut = millis();
  byte chr=26;
  uof_iicSerialWrite(FONA_UART, 26);
  cptr=0;
  while (true) {
    str=uof_attend_reponse_GSM(120000);
    if (str=="OK") cptr+=1;
    if (cptr==1) {
      if (affiche_debug) Serial.println ("        => OK");
      return OK;
    }
    if (str == "ERROR") {
      if (affiche_debug) Serial.println ("        => "+str+" ???");
      return KO;
    }
    if (str.substring(0,11) == "+CME ERROR:") {
      if (affiche_debug) Serial.println ("        => "+str);
      return KO;
    }
    if (millis() - debut > Timeout) {
      if (affiche_debug) Serial.println ("        => Time out de la fonction");
      return KO;
    }
  }
}

bool uof_envoi_donnees(String fichier) {
  //
  //
  //    envoi des données par paquets de 'taille_du_buffer_lecture_carte_sd'
  //
  //
  // envoie le fichier en FTP
  // fichier doit contenir le nom du fichier. ex. : oso_01_20210527_09.csv
  if (affiche_debug) Serial.println ("        Envoi du contenu du fichier :");
  String fichier_source = fichier;
  String fichier_cible = fichier;
  byte buf[taille_du_buffer_lecture_carte_sd];
   
  if (fichier_source == "") {
    if (affiche_debug) Serial.println ("        ECHEC : nom de fichier vide");
    return KO;
  } else {
    fichier_source = "/tmp_send/"+fichier_source;
  }
  //
  myFile = SD.open(fichier_source, FILE_READ);
  if (!myFile) {
    if (affiche_debug) Serial.println ("        => ECHEC : le fichier '"+fichier+"' n\'existe pas sur la carte SD.");
    return KO;             
  }
  if (!uof_gsm_commande_CFTPPUT(fichier_cible,120000)) {
      if (affiche_debug) Serial.println ("        => ECHEC : Problème sur la liaision FTP");
      return KO;
  } else {
    unsigned long deb = millis();
    // lecture du fichier source par paquets de x octets et transmission octet par octet à partir du buffer
    if (affiche_debug) Serial.println ("Début de l'envoi du contenu du fichier ....");
    if (affiche_debug) Serial.print (myFile.size());

    int nb_octets_sur_derniere_page = (myFile.size()%taille_du_buffer_lecture_carte_sd);
    
    if (affiche_debug) Serial.println (" octets à envoyer");
    int cp=0;
    int imax=0;
    while (myFile.available()) {
      myFile.read(buf,taille_du_buffer_lecture_carte_sd);
      String xx = "";
      if (!myFile.available()) {
        imax = nb_octets_sur_derniere_page;
      } else {
        imax = taille_du_buffer_lecture_carte_sd;
      }
      if (FONA_UART == 1) {
        iicSerial1.write(buf,imax);
        iicSerial1.flush();
      } else {
        iicSerial2.write(buf,imax);
        iicSerial2.flush();
      }
    }
    if (affiche_debug) Serial.print ("        Données envoyées en ");
    if (affiche_debug) Serial.print (millis() - deb);
    if (affiche_debug) Serial.println (" ms");
    if (affiche_debug) Serial.println ("        Fermeture du fichier à transmettre");
    myFile.close();
    if (!uof_gsm_fin_de_transmission(120000)) {
      if (affiche_debug) Serial.println ("          Fin de transmission KO");
      return KO;
    }
  }
  return OK;
}
