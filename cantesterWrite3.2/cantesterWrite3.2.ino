/*https://www.can-cia.org/standardization/specifications/
Pro stahovani specifikaci je potreba zaregistrovat se, ale melo by stacit vyplnit email. To hlavni je v CiA 301.
Bylo by dobre implementovat minimalne:
 - 301_canopen.pdf - str. 75
NMT stavovy automat (CAN-ID=0)
Heartbeat (CAN-ID=0x700+NodeID)
pro majacek staci 1x asynchroni RPDO (CAN-ID=0x200+NodeID)

Jinak je to CANopen. OpenCAN je nejaky, asi uz mrtvy, opensource projekt.*/

#include <mcp_can.h>

#define Monitor(x) Serial.println(x)
const int ledPin =  13;
MCP_CAN CAN(5);

byte i;

void setup() {

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  if(CAN.begin(CAN_1000KBPS, MCP_8MHz) == CAN_OK){Serial.print("CAN init ok!!\r\n");}
    else {Serial.print("Can init fail!!\r\n");} // Pridat upozorneni, ze can nefunguje a zastavit to
  Serial.println("setup can");
}

void loop() {
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
                  for (i=0;i<len;i++){
                    Serial.print(buf[i]);
                    Serial.print("\t");
                  }
                  Serial.println("");
                  Serial.println("");
                }
                          
        }
}
