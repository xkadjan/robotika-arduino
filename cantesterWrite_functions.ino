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


/*------------------------ setup watchdogu -----------------------------------------*/

void WatchDog()
{

  MCUSR &= ~(1 << WDRF);                           // reset watch dog
  WDTCSR |= (1 << WDCE) | (1 << WDE);              // povolenÃ­ konfigurace watch dog
  WDTCSR = (1 << WDP1) | (1 << WDP2) ;             // nastavenÃ­ watch dog na 1 s
  WDTCSR |= (1 << WDIE);                           // enable interrupt mode

}

/*-------------------------- Probuzeni arduina od interniho preruseni od Watchdog  - HB ----------------*/

ISR( WDT_vect ) {
  MCUSR &= ~(1 << WDRF);// reset watch dog
  Serial.println(HBstatus);
  HBflag = 1;
}
