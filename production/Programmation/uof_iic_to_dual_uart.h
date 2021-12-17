//
//  -----------------------------------------------------------------------------------------------------------
//  
//                                          Pilotage du module Gravity (IIC vers 2 UART)
//                        
// - Description  : Ces fonctions servent à piloter le module Gravity.
//                  initialisation, envoi, lecture....
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
//                  void uof_iicSerialBegin(byte port, long int vitesse, byte format=IICSerial_8N1)
//                  void uof_iicSerialEnd(byte port)
//                  bool uof_iicSerialAvailable(byte port)
//                  byte uof_iicSerialRead(byte port)
//                  void uof_iicSerialWrite(byte port, byte n)
//                  void uof_iicSerialPrintln(byte port, String *texte)
//                  void uof_iicSerialFlush(byte port);
//  
//
//  -----------------------------------------------------------------------------------------------------------

// paramètrage de l'adresse IIC du module.
// configurer les microswitch en conséquence
#define IICtodualUARTadresseA0 1
#define IICtodualUARTadresseA1 1


// bibliothèque initiale
#include <DFRobot_IICSerial.h>      // download at https://github.com/DFRobot/DFRobot_IICSerial


// Construction de l'UART 1 (liaison vers ...)
DFRobot_IICSerial iicSerial1(Wire,SUBUART_CHANNEL_1,IICtodualUARTadresseA1,IICtodualUARTadresseA0);//Construct UART1

// Construction de l'UART 1 (liaison vers ...)
DFRobot_IICSerial iicSerial2(Wire, SUBUART_CHANNEL_2,IICtodualUARTadresseA1,IICtodualUARTadresseA0);//Construct UART2





//***************************************************************************
void uof_iicSerialBegin(byte port, long int vitesse, byte format=IICSerial_8N1) {
  //
  //  port : 1 ou 2
  //
  //  vitesse :     2400
  //                4800
  //                7200
  //               14400
  //               19200
  //               28800
  //               38400
  //               57600
  //               76800
  //              115200
  //              153600
  //              230400
  //              460800
  //              307200
  //              921600
  //
  //  format :  IICSerial_8N1
  //                IICSerial_8N2
  //                IICSerial_8Z1
  //                IICSerial_8Z2
  //                IICSerial_8O1
  //                IICSerial_8O2
  //                IICSerial_8E1
  //                IICSerial_8E2
  //                IICSerial_8F1
  //                IICSerial_8F2
  //                8 represents the number of data bit,
  //                N for no parity,
  //                Z for 0 parity
  //                O for Odd parity
  //                E for Even parity,
  //                F for 1 parity
  //                1 or 2 for the number of stop bit
  //                Default IICSerial_8N1
  //
  //
  //
  if (port ==1) {
    iicSerial1.begin(vitesse,format);
  }
  if (port==2) {
    iicSerial2.begin(vitesse,format);
  }
}

//***************************************************************************
void uof_iicSerialEnd(byte port) {
  //
  //  Termine une liaison UART
  //
  if (port == 1) {
    iicSerial1.end();
  }
  if (port == 2) {
    iicSerial2.end();
  }

  
}

//***************************************************************************
bool uof_iicSerialAvailable(byte port) {
  //
  //  Y a t il des données disponibles en lecture sur un des ports
  //     Renvoie true si oui, false si non
  //
  if (port == 1) {
    if (iicSerial1.available()) return true; else return false;
  }
  if (port == 2) {
    if (iicSerial2.available()) return true; else return false;
  }
}

//***************************************************************************
byte uof_iicSerialRead(byte port) {
  if (port ==1) {
    return(iicSerial1.read());
  }
  if (port==2) {
    return(iicSerial2.read());
  }
}

//***************************************************************************
void uof_iicSerialWrite(byte port, byte n) {
  //
  //  Envoie octet sur l'un des ports
  //
  if (port == 1) iicSerial1.write(n);
  if (port == 2) iicSerial2.write(n);

}


//***************************************************************************
void uof_iicSerialPrintln(byte port, String *texte) {
  //
  //  Envoie une chaine de caractères sur l'un des ports
  //
  if (port == 1) iicSerial1.println(*texte);
  if (port == 2) iicSerial2.println(*texte);
}

//***************************************************************************
void uof_iicSerialFlush(byte port) {
  //
  // Attend que le buffer émission d'un port soit vide
  //
  if (port == 1) iicSerial1.flush();
  if (port == 2) iicSerial2.flush();
  
}
