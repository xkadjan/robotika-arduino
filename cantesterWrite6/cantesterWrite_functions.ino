void CANtoSerialMonitor(){
      Serial.print("can address: ");
      Serial.println(address);
      Serial.print("lenght: ");
      Serial.println(len);
      for (int i=0;i<len;i++){
        Serial.print(buf[i]);
        Serial.print("\t");
      }
      Serial.println("");
      Serial.println("");
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
  HB = 1;
}
