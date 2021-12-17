//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Variables globales
//                        
// - Description  : contient les variables globales utilisées dans le programme.
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
//                  Variables globales
//
//  -----------------------------------------------------------------------------------------------------------


bool autorise_mesure = true;
bool autorise_transmission = true;  // si autorise_emission est false la transmission ne sera pas réalisée
bool autorise_emission = true;

bool affiche_debug = true;

// affectation des ports UART du module Gravity (I2C vers 2 UART)
#define FONA_UART   1
#define TRILUX_UART 2

// 
#define OK 1
#define KO 0


unsigned long debut = 0;        // pour les calculs de durées de procédures
unsigned long time_out = 0;     // pour définir un time out


// Les variales suivantes seront renseignées lors de
// la lecture du fichier config.txt
String heure_des_envois = "";
String id_logger="";
String gsm_code_pin="";
String serveur_adresse="";
String serveur_port="";
String serveur_user="";
String serveur_pwd="";
int intervale_entre_mesures = 5; // en minutes par défaut 5

// pour le GSM
char tx_buffer[256];//Define the TX array tx_buffer
char rx_buffer[256];//Define the RX array rx_buffer
String nom_operateur_choisi="";

// pour la gestion de l'envoi des données
String uof_fichier_a_envoyer="";
unsigned long debut_transmission; 
unsigned long duree_transmission;
unsigned long duree_mesure;
int taille_du_buffer_lecture_carte_sd=2048;

// pour l'uof
byte uof_tab_heure_des_envois[24];
byte uof_nb_heure_des_envois = 0;

// pour la RTC
//Variables
byte Annee ;
byte Mois ;
byte Jour ;
byte DoW ;
byte Heure ;
byte Minute ;
byte Seconde ;
bool Century  = false;
bool h12=false ;
bool PM ;
