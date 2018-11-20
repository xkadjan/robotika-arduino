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
          Stop();
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
          Stop();
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


/*-------------------------- Probuzeni arduina od interniho preruseni  - HB ----------------*/

void TimerInterrupt () {
  Timer3.start(1000000); // Calls every 1 s
  Serial.println(HBstatus);
  HBflag = 1;
}

/*---------------------------Led indikace ----------------------------------------------------------------*/
void LEDIndicator ()
{
    for (int i=0; i <= 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    //tone(Syrena,NOTE_C7,200);
  }
}

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
      int pom;
      for (int i=3;i<=5;i++) {
        pom = i - 3;
        analogWrite(Zatez[i],buf[pom]);
      }
      analogWrite(Digital[0],buf[3]);
      analogWrite(Digital[1],buf[4]);
      analogWrite(Digital[2],buf[5]);  
      /*
      int pomDigit;
      for (int i=0; i<3; i=i++) {
        pomDigit=i+4;
        analogWrite(Digital[i],buf[pomDigit]);
        Serial.println(buf[pomDigit]);
      }
      */
    }
  }
}

/*------------------------- Seting pinout ----------------------------------------------------------------------*/

 void SetPinout () {

  for (int i=3;i<=5;i++) {
    Zatez[i] = i;
    pinMode(Zatez[i], OUTPUT);
    digitalWrite(Zatez[i], LOW);
  }
 }

/*-------------------------- Reset ------------------------------------------------------------------------------*/

void Reset () {
  digitalWrite(resetPin,LOW);
}

/*------------------------- Stop -------------------------------------------------------------------------------*/

void Stop() {
  int pom;
  for (int i=3;i<=5;i++) {
   analogWrite(Zatez[i],LOW);
   }
}

/*------------------------- Seting digital pinout ----------------------------------------------------------------------*/

 void SetDigitalPinout () {
  int j=22;
  for (int i=0;i<3;i++) { // piny 22,24,26
    Digital[i]=j;
    pinMode(Digital[i], OUTPUT);
    digitalWrite(Digital[i], LOW);
    j=j+2;
  }
 }

