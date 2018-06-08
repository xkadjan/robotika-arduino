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
#include <avr/wdt.h>          // library for default watchdog functions
#include <avr/interrupt.h>    // library for interrupts handling
#include "pitches.h";  // knihovna pro syrenu

const int ledPin =  8;
const int Blikacka = 4;
const int Syrena = 6;
int Syr = 0;

MCP_CAN CAN(10);

int           CANstatus = 0;
int           CANnodeID = 32; //  (20 hexa)
int           HBflag = 0;     //  HBflag=1 -> posle se "s" po0x700+NodeID ; HBflag=0 -> neposle se
int           CANmessageID;
unsigned char HBstatus = 0;   //  0=Boot-up, 4=Stop, 5=Operational, 127=Pre-operational
unsigned char len = 0;
unsigned char buf[8];


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(Blikacka, OUTPUT);
  pinMode(Syrena,OUTPUT); //syrena

  digitalWrite(ledPin, LOW);
  digitalWrite(Blikacka, LOW);
  digitalWrite(Syrena, LOW);
   
  Serial.println("Setup can..");
  while(CAN.begin(CAN_1000KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(ledPin, HIGH);
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  LEDIndicator ();
  
  WatchDog();
  delay(500);
  
  Serial.println("From Init");
  HBstatus = 127;
  Serial.println("To Pre-Operational");
}

void (*Reset)(void)=0;        //reset function

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
  if (Syr == 1) {
  tone(Syrena,NOTE_C7,100);
  delay(200);
  tone(Syrena,NOTE_E5,100);
  
  }
}

