//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Pilotage du module RTC (DS3231)
//                        
// - Description  : Ces fonctions servent à piloter le module RTC.
//                  Outre les fonctions de base de la bibliothèque, ce module intègre des fonctions plus
//                  évoluées : Lecture de la date complète, lecture de l'heure complète, formatage de la date
//                  ou de l'heure...
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
//                  String uof_annee_sur_4_chr(byte a)
//                  String uof_mois_sur_2_chr(byte m)
//                  String uof_jour_sur_2_chr(byte j)
//                  String uof_heure_sur_2_chr(byte h)
//                  void uof_get_heure_des_envois()
//                  void uof_set_date(byte j, byte m, byte a)
//                  void uof_set_time (byte h, byte m,byte s)
//                  byte uof_get_annee()
//                  byte uof_get_mois()
//                  byte uof_get_jour()
//                  void uof_get_date()
//                  byte uof_get_heure ()
//                  byte uof_get_minute ()
//                  byte uof_get_seconde ()
//                  void uof_get_time ()
//                  void uof_affiche_date()
//                  void uof_affiche_time()
//                  void veille_de(byte *jj, byte *mm, byte *aa)
//
//
//  -----------------------------------------------------------------------------------------------------------




#include <Wire.h>       //https://www.arduino.cc/en/reference/wire

#include <DS3231.h>     //https://github.com/NorthernWidget/DS3231

//Objects
DS3231 horloge;


String uof_annee_sur_4_chr(byte a) {
  //
  //  Entrée : année sur un byte
  //  Sortie : année sur 4 caractères
  //
  String aa = String(a);
  if (aa.length() <2) aa="00"+aa;
  aa=aa.substring(aa.length()-2);
  aa="20"+aa;
  return aa;
}

String uof_mois_sur_2_chr(byte m){
  //
  //  Entrée : mois sur un byte
  //  Sortie : mois sur 2 caractères
  //
  String mm = String(m);
  if (mm.length() <2) mm="00"+mm;
  mm=mm.substring(mm.length()-2);
  return mm;
}

String uof_jour_sur_2_chr(byte j){
  //
  //  Entrée : jour sur un byte
  //  Sortie : jour sur 2 caractères
  //
  String jj = String(j);
  if (jj.length() <2) jj="00"+jj;
  jj=jj.substring(jj.length()-2);
  return jj;
}

String uof_heure_sur_2_chr(byte h){
  //
  //  Entrée : heure sur un byte
  //  Sortie : heure sur 2 caractères
  //
  String hh = String(h);
  if (hh.length() <2) hh="00"+hh;
  hh=hh.substring(hh.length()-2);
  return hh;
}
 
void uof_get_heure_des_envois() {
  //
  // à partir de la variable String heure_des_envois lue dans le fichier 'config.txt', rempli le tableau
  //   uof_heure_des_envois et uof_nb_heure_des_envois
  bool fin=false;
  String x = heure_des_envois;
  uof_nb_heure_des_envois = 0;
  while (!fin) {
    int p = x.indexOf(",");
    if (p==-1) {
      fin=true;
      String z = x.substring(0,p);
      int k = z.toInt();
      uof_tab_heure_des_envois[uof_nb_heure_des_envois] = k;
      uof_nb_heure_des_envois = uof_nb_heure_des_envois+1;
    } else {
      String z = x.substring(0,p);
      int k = z.toInt();
      uof_tab_heure_des_envois[uof_nb_heure_des_envois] = k;
      uof_nb_heure_des_envois = uof_nb_heure_des_envois+1;
      x=x.substring(p+1);
    }
  }
}

void uof_set_date(byte j, byte m, byte a) {
  horloge.setYear(a);
  horloge.setMonth(m);
  horloge.setDate(j);
}

void uof_set_time (byte h, byte m,byte s) {
  horloge.setHour(h);
  horloge.setMinute(m);
  horloge.setSecond(s);
}

byte uof_get_annee() {
  return horloge.getYear();
}

byte uof_get_mois() {
  return horloge.getMonth(Century);
}

byte uof_get_jour() {
  return horloge.getDate();
}

void uof_get_date() {
  Annee = horloge.getYear();
  Mois = horloge.getMonth(Century);
  Jour = horloge.getDate();
}

byte uof_get_heure () {
  return horloge.getHour(h12, PM);
}

byte uof_get_minute () {
  return horloge.getMinute();
}

byte uof_get_seconde () {
  return horloge.getSecond();
}

void uof_get_time () {
  Heure=uof_get_heure();
  Minute=uof_get_minute();
  Seconde=uof_get_seconde();
}

void uof_affiche_date() {
  /*if (affiche_debug)*/ Serial.print(Jour);
  /*if (affiche_debug)*/ Serial.print ("/");
  /*if (affiche_debug)*/ Serial.print(Mois);
  /*if (affiche_debug)*/ Serial.print ("/");
  /*if (affiche_debug)*/ Serial.print(Annee);
}

void uof_affiche_time() {
  /*if (affiche_debug)*/ Serial.print(Heure);
  /*if (affiche_debug)*/ Serial.print (":");
  /*if (affiche_debug)*/ Serial.print(Minute);
  /*if (affiche_debug)*/ Serial.print (":");
  /*if (affiche_debug)*/ Serial.print(Seconde);
}

void veille_de(byte *jj, byte *mm, byte *aa) {
  //
  //  *jj : pointe vers un int représentant le jour du mois
  //  *mm : pointe vers un int représentant le mois de l'année
  //  *aa : pointe vers un int représentant l'année
  //
  //  La fonction calcule la date du jour précédent (la veille)
  //  et modifie les 3 valeurs (jj, mm et aa) en conséquence
  //
  //  Prend en compte les mois à 28, 29, 30 et 31 jours
  //
  //  
  *jj = *jj-1;
  if (*jj == 0) {
    *jj=31; // pour le moment, avant de avois si c'est un mois à 28, 29, 30 ou 31jours 
    *mm=*mm-1;
    if (*mm == 0) {
      *mm=12;
      *aa = *aa -1;
    }
    if (*mm==4 or *mm == 6 or *mm == 9 or *mm == 11) {
      *jj = 30;
    }
    if (*mm == 2) {
      if ((*aa % 4) == 0) {
        *jj=29;
      } else {
        *jj=28;
      }
    }
  }
}
