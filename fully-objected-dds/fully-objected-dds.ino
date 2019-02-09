#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include "defines.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN LDC INIT
/////////////////////////////////////////////////////////////////////////////////////
LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_EN, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7);
//bool volatile lcdRedrawFlag = true;


SGEN_Waveforms mySGEN_Waveforms;
SGEN_Values mySGEN_Values;
SGEN_Display mySGEN_Display;
SGEN_Regulation mySGEN_Regulation;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN SETUP
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
  mySGEN_Display.softFrequencyDigitsBuffer = new byte [mySGEN_Values.valuesArray[FREQUENCY_IDX].getBufferLength()];
  mySGEN_Display.hardwareFrequencyDigitsBuffer = new byte [mySGEN_Values.valuesArray[FAST_PWM].getBufferLength()];
  //DDRC = 0xff;
  TCCR0 = 0; // arduino time interrupt timer disable
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("Serial ready");
   Serial.println(mySGEN_Regulation.menuLevel);
  // display all
  mySGEN_Display.displaySelective();
  // set soft frequency
  mySGEN_Waveforms.stepV = uint16_t( mySGEN_Values.valuesArray[FREQUENCY_IDX].getValue()/FREQUENCY_TO_STEP);
  // set amplitude
  mySGEN_Values.amplitudeRegulationMaster(mySGEN_Values.valuesArray[AMPLITUDE_IDX].getValue());
  // io pins
  pinMode(ENCODER_PIN_1, INPUT_PULLUP);
  pinMode(ENCODER_PIN_2, INPUT_PULLUP);
  pinMode(ENCODER_SET_PIN_1, INPUT_PULLUP);
  // set output state
  mySGEN_Values.setOutputState(0);
  // interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_2), interrupt_encoderMoved, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SET_PIN_1), interrupt_encoderSet, FALLING);
  // set up the LCD's number of columns and rows
  //
  //mySGEN_Values.offsetRegulationMaster(mySGEN_Values.valuesArray[4].getValue());
}

