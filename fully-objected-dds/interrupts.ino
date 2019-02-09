///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////
// INTERRUPTS
/////////////////////////////////////////////////////////////
void interrupt_encoderMoved() {
  if (digitalRead(ENCODER_PIN_1)) {
    mySGEN_Regulation.doActionEncoderMove(1);
  } else {
    mySGEN_Regulation.doActionEncoderMove(-1);
  }
}

void interrupt_encoderSet() {
   mySGEN_Regulation.doActionEncoderSet(1);
}
