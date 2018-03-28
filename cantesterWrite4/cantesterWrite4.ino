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
130-(12),(13),(14) NMT service reset communication indication.*/

#include <mcp_can.h>
#include <avr/wdt.h>        // library for default watchdog functions
#include <avr/interrupt.h>  // library for interrupts handling


#define Monitor(x) Serial.println(x)
const int ledPin =  13;
MCP_CAN CAN(5);

int CANstatus = 0;
int CANnodeID = 20;
unsigned char len = 0;
unsigned char buf[8];
int address;

void setup() {

  CANstatus = 0; //Initialisation
  
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  if(CAN.begin(CAN_1000KBPS, MCP_8MHz) == CAN_OK){Serial.print("CAN init ok!!\r\n");}
    else {Serial.print("Can init fail!!\r\n");} // Pridat upozorneni, ze can nefunguje a zastavit to
  Serial.println("setup can");
  WatchDog();

  CANstatus = 1; //Pre-Operational
  
}

void loop() {
  if(CAN_MSGAVAIL == CAN.checkReceive()) {  
    CAN.readMsgBuf(&len, buf); // read data,  len: data length, buf: data buf
    int address = CAN.getCanId();
    CANtoSerialMonitor();
    if (address == 0){ 
      Serial.println("change state on: ");
      if (buf[0] == 0){ 
        Serial.println(buf[1]);
      if (buf[0] == CANnodeID){ 
        Serial.println(buf[1]);
      }
    }


    
    if (address == 200+CANnodeID){ //RPDO
      Serial.println("msg for this device'");
    }
    
    
  }

  if (CANstatus == 1){
    //Pre-Operational
  }
  if (CANstatus == 2){
    //Stoped
  }
  if (CANstatus == 3){
    //Operational
    
     /*   if (i > 255) {i=0;}
       byte stmp[8] = {255-i,2,0,3,22,0,7,i};
       CAN.sendMsgBuf(0x301, 0, 8, stmp); // Sending
       Serial.println("zprava");
       delay(1000);
       i++;*/
       //*********************
         unsigned char len = 0;
         unsigned char buf[8];
               if(CAN_MSGAVAIL == CAN.checkReceive()) {  
                CAN.readMsgBuf(&len, buf); // read data,  len: data length, buf: data buf
                int canId = CAN.getCanId();
                
                Serial.print("Adress: ");
                Serial.print(canId);
                Serial.print(": ");
                
                Serial.print("lenght: ");
                Serial.println(len);
                //Serial.println(buf[7]);
                
                if(canId==1040){
                  Serial.println("LED");
                  
                  digitalWrite(ledPin, buf[7]);
                  byte stmp[8]={buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[9]};
                  CAN.sendMsgBuf(0x411, 0, 8, stmp);
                  for (int i=0;i<len;i++){
                    Serial.print(buf[i]);
                    Serial.print("\t");
                  }
                  Serial.println("");
                  Serial.println("");
                }   
               }
          }
}
}
