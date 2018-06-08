void NMTcontroll(){
  if (CANmessageID == 0) {
    if ((buf[1] == 0) || (buf[1] == CANnodeID)){
      if (HBstatus == 127) {
        Serial.println("From Pre-Operational");
        if ((buf[0] == 129) || (buf[0] == 130)) {
          Serial.println("To Reset");
          HBstatus = 0;
          Reset();
        } 
        else if (buf[0] == 1){
          Serial.println("To Operational");
          HBstatus = 5;
        } 
        else if (buf[0] == 2){
          Serial.println("To Stop");
          HBstatus = 4;
        }
      } 
      else if (HBstatus == 4){
        Serial.println("From Stoped");
        if ((buf[0] == 129) || (buf[0] == 130)) { 
          Serial.println("To Reset");
          HBstatus = 0;
          Reset();
        } 
        else if (buf[0] == 1){ 
          Serial.println("To Operational");
          HBstatus = 5;
        } 
        else if (buf[0] == 128){ 
          Serial.println("To Pre-operational");
          HBstatus = 127;
        }
      }
      else if (HBstatus == 5){
        Serial.println("From Operational");
        if ((buf[0] == 129) || (buf[0] == 130)) {
          Serial.println("To Reset");
          HBstatus = 0;
          Reset();
        }
        else if (buf[0] == 2){
          Serial.println("To Stop");
          HBstatus = 4;
        } 
        else if (buf[0] == 128){
          Serial.println("To Pre-operational");
          HBstatus = 127;
        }
      }  
    }
  }
}

void HBresponse(){
  if (HBflag == 1) {
    byte stmp[8]={HBstatus};
    CAN.sendMsgBuf(0x700+CANnodeID, 0, 1, stmp); //pouze jeden byte - state
    HBflag = 0;
  }
}

void CANmsgToSerial(){
      Serial.println("-----------------------------------------");
      Serial.print("can address: ");
      Serial.print(CANmessageID);
      Serial.print("; lenght: ");
      Serial.println(len);
      for (int i=0;i<len;i++){
        Serial.print(buf[i]);
        Serial.print("\t");
      }
      Serial.println("");
      Serial.println("-----------------------------------------");
}


/*------------------------ setup timeru -----------------------------------------*/
void timerSetup()
{
  Timer3.attachInterrupt(timer1Interrupt).start(1000000);      //timer pouzivany v DUE <DueTimer.h>  
}
void timer1Interrupt()
{                       // každých 1s nastavy timerFlag na HIGH
  HBflag = 1;   
}

/*------------------------ setup CANu -----------------------------------------*/

void CANSetup()
{
  Serial.println("Setup can..");
  while(CAN.begin(CAN_1000KBPS, MCP_8MHz) != CAN_OK)
  {
    digitalWrite(ledPin, HIGH);
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");
}
  
/*---------------------------Led indikace ----------------------------------------------------------------*/
void LEDIndicatorTest ()
{
    for (int i=0; i <= 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    //tone(buzzerPin,NOTE_C7,200);
  }
}

/*---------------------------buzzer ----------------------------------------------------------------*/
/*
void buzzer()
{
  if (buzzerFlag == 1) 
  {
    tone(buzzerPin,NOTE_C7,100);
    delay(200);
    tone(buzzerPin,NOTE_E5,100);
  }
}
*/
/*------------------------- Clear Buffer ------------------------------------------------------------------------*/

void ClearBuffer ()
{
  for (int i=0; i <= 7; i++) {
      buf[i] = 0;
  }
  CANmessageID = 2;
}

/*------------------------- RPDO -------------------------------------------------------------------------------*/

void RPDO ()
{
    if (HBstatus == 5){ 
    if (CANmessageID == 512+CANnodeID){        //RPDO   200+CANnodeID HEXA
      Serial.println("RPDO msg for this device'");
      if (buf[0] == 1){     //Led IS ON
         Serial.println(buf[0]);
         digitalWrite(ledPin,HIGH);
         digitalWrite(Blikacka,HIGH);
      }
      if (buf[0] == 0){     //LED IS OFF
        Serial.println(buf[0]);
        digitalWrite(ledPin,LOW);
        digitalWrite(Blikacka,LOW);
      }
      if (buf[1] == 1){     //Led IS ON
        Serial.println(buf[1]);
        buzzerFlag = 1;
      }
      if (buf[1] == 0){     //LED IS OFF
        Serial.println(buf[1]);
        buzzerFlag = 0;
      }
    }
  }
}

