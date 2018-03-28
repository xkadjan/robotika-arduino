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
  Serial.println("  wd");
  //int pom = 33;
  //String pom2 = decToHex(pom,2);
 // Serial.println(pom2);
  
//  CAN.sendMsgBuf(0x700+CANnodeID, 0, 1, CANstatus); //pouze jeden byte - state
}
//------------------------------------------------------------------------------------------------
// Converting from Decimal to Hex:

// NOTE: This function can handle a positive decimal value from 0 - 255, and it will pad it
//       with 0's (on the left) if it is less than the desired string length.
//       For larger/longer values, change "byte" to "unsigned int" or "long" for the decValue parameter.


String decToHex(byte decValue, byte desiredStringLength) {
  
  String hexString = String(decValue, HEX);
  while (hexString.length() < desiredStringLength) hexString = "0" + hexString;
  
  return hexString;
}
