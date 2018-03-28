/*https://www.can-cia.org/standardization/specifications/
to do:
Pro stahovani specifikaci je potreba zaregistrovat se, ale melo by stacit vyplnit email. To hlavni je v CiA 301.
Bylo by dobre implementovat minimalne:
 - 301_canopen.pdf - str. 75
NMT stavovy automat (CAN-ID=0)
Heartbeat (CAN-ID=0x700+NodeID)
pro majacek staci 1x asynchroni RPDO (CAN-ID=0x200+NodeID)

nodeID konstantni treba 20
2) id (jakoHB=ID700H+nodeID) v tele 0 (1b) (to same jako reset)
probiha HB a ceka na prechopd do operat. - po vterine
HB:
0: Boot-up
4: Stopped
5: Operational
127: Pre-operational

NMT id=0 (2b...1. co se dela, 2.cislo bodulu (0=broatcast))

RPDO 200+nodeID (220)

auto-(1) At Power on the NMT state initialisation is entered autonomously
auto-(2) NMT state Initialisation finished - enter NMT state Pre-operational
automatically
1-(3) NMT service start remote node indication or by local control
128-(4),(7) NMT service enter pre-operational indication
2-(5),(8) NMT service stop remote node indication
1-(6) NMT service start remote node indication
129-(9),(10),(11) NMT service reset node indication
130-(12),(13),(14) NMT service reset communication indication.

poznámka - str. 77 
         - adresa je nula (0) - pak se kouknu do 2. bajtu (tedy bajt č. 1 jestli je to pro mě - tedy 0 nebo 20 hexa - pak se kounku do 1. bajtu (bajt 0) co má dělat př 129 hexa reset 1 přechod do operation ...........
          - strukturu programu (CANstatus) nechat jen místo kontroly adres kontrolovat to samé ve zprávě

*/

#include <mcp_can.h>
#include <avr/wdt.h>        // library for default watchdog functions
#include <avr/interrupt.h>  // library for interrupts handling

#define Monitor(x) Serial.println(x)
const int ledPin =  8;
MCP_CAN CAN(10);

int CANstatus = 0;
int CANnodeID = 32;  // 32 je 20 hexa
int HB = 0; //HB=1 - posle se HB; HB=0 - neposle
unsigned char HBstatus = 0;
unsigned char len = 0;
unsigned char buf[8];
int address;

void setup() {
  CANstatus = 0; //Initialisation
  HBstatus = 0;
  
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  if(CAN.begin(CAN_1000KBPS, MCP_8MHz) == CAN_OK){Serial.print("CAN init ok!!\r\n");}
    else {Serial.print("Can init fail!!\r\n");} // Pridat upozorneni, ze can nefunguje a zastavit to
  Serial.println("setup can");
  
  byte stmp[8]={HBstatus};
  CAN.sendMsgBuf(0x720, 0, 1, stmp); //pouze jeden byte - state
  
  WatchDog();
  CANstatus = 1; //Pre-Operational
  HBstatus = 127;
}

void (*Reset)(void)=0; //reset function

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive()) {  
    CAN.readMsgBuf(&len, buf); // read data,  len: data length, buf: data buf
    int address = CAN.getCanId();
    CANtoSerialMonitor();
    if (address == 0) {
        if ((buf[1] == 0) || (buf[1] == CANnodeID)){
          //Pre-Operational
          if (CANstatus == 1) { //        ((address == 297+CANnodeID)||(address == 304+CANnodeID))
            if ((buf[0] == 297) || (buf[0] == 304)) { // Reset
              Serial.println("Rset");
              CANstatus = 0; //Reset
              Reset();
            } else if (buf[0] == 1){ //Přechod do operational
              Serial.println("Přechod do operational");
              CANstatus = 3; //Operational
              HBstatus = 5;
            } else if (buf[0] == 2){ //Přechod do Stop
                Serial.println("Přechod do Stop");
                CANstatus = 2; //Stop
                HBstatus = 4;
              }
            } else if (CANstatus == 2){
              //Stoped
              if ((buf[0] == 297) || (buf[0] == 304)) { // Reset
                Serial.println("Rset");
                CANstatus = 0; //Reset
                Reset();
              } else if (buf[0] == 1){ //Přechod do operational
                  Serial.println("Přechod do operational");
                  CANstatus = 3; //Operational
                  HBstatus = 5;
                } else if (buf[0] == 296){ //Přechod do Pre-operational
                    Serial.println("Přechod do Pre-operational");
                    CANstatus = 1; //Pre-operational
                    HBstatus = 127;
                  }
             }else if (CANstatus == 3){
                //Operational
                  if ((buf[0] == 297) || (buf[0] == 304)) { // Reset
                Serial.println("Rset");
                CANstatus = 0; //Reset
                Reset();
                   } else if (buf[0] == 2){ //Přechod do Stop
                      Serial.println("Přechod do Stop");
                      CANstatus = 2; //Stop
                      HBstatus = 4;
                  } else if (buf[0] == 296){ //Přechod do Pre-operational
                    Serial.println("Přechod do Pre-operational");
                    CANstatus = 1; //Pre-operational
                    HBstatus = 127;
                    }
                if (buf[0]== 2){ //RPDO   200 hexa  //512
                   Serial.println("msg for this device'");
                   if (buf[7] == 1){ //Led IS ON
                    Serial.println(buf[7]);
                    digitalWrite(ledPin,HIGH);
                   }
                   if (buf[7] == 0){ //LED IS OFF
                    Serial.println(buf[7]);
                    digitalWrite(ledPin,LOW);
                  }
                }
              }  
      }
    }
  }
  if (HB == 1) {
    byte stmp[8]={HBstatus};
    CAN.sendMsgBuf(0x720, 0, 1, stmp); //pouze jeden byte - state
    HB = 0;
  }
}

