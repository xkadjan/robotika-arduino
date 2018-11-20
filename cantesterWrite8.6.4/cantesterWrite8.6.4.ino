/*
CAN node according:
CiA 301: https://www.can-cia.org/standardization/specifications/ - 301_canopen.pdf - str. 75

NMT controll (dec): 
id=0; two bytes: 1. cs-"requirement to status change", 0. CANnodeID or 0=broatcast
cs: 1: to Operational
    2: to Stop
    128 or 129: Reset

HB responses ones per second about status of node (dec):
id=0x700+NodeID; one byte: s-"state of the heartbeat producer"
s:  0: Boot-Up
    4: Stopped
    5: Operational
    127: Pre-operational

RPDO 200+nodeID (220)
 .... doplnit
 todo: - RPDO: pro majacek staci 1x asynchroni RPDO (CAN-ID=0x200+NodeID)
       - po rtesetu nedava boot-up
       - doplnit popis pouzitych knihoven

*/

#include <mcp_can.h>
#include <DueTimer.h>

const int ledPin =  13;
byte Zatez[6];
byte Digital[3]={22,24,26};
const int Syrena = 6;

const byte resetPin = 50;

MCP_CAN CAN(2);

int           CANstatus = 0;
int           CANnodeID = 32; //  (20 hexa)
int           HBflag = 0;     //  HBflag=1 -> posle se "s" po0x700+NodeID ; HBflag=0 -> neposle se
int           CANmessageID;
unsigned char HBstatus = 0;   //  0=Boot-up, 4=Stop, 5=Operational, 127=Pre-operational
unsigned char len = 0;
byte buf[8];


void setup() {
  pinMode(resetPin,OUTPUT);
  digitalWrite(resetPin,HIGH);
  
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(Syrena,OUTPUT); //syrena
  
  digitalWrite(ledPin, LOW);
  digitalWrite(Syrena, LOW);

  SetPinout (); // nastaveni zatezovych pinu
   
  Serial.println("Setup can..");
  while(CAN.begin(CAN_1000KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(ledPin, HIGH);
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  LEDIndicator ();
  
  Timer3.attachInterrupt(TimerInterrupt);
  Timer3.start(1000000); // Calls every 1 s
  delay(500);
  
  Serial.println("From Init");
  HBstatus = 127;
  Serial.println("To Pre-Operational");
}


void loop() {
  ClearBuffer();
  if(CAN_MSGAVAIL == CAN.checkReceive()) {  
    CAN.readMsgBuf(&len, buf);
    CANmessageID = CAN.getCanId();
    CANmsgToSerial();
    NMTcontroll();
  }
  HBresponse();
  RPDO();
}

