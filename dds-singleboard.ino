#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include "defines.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN LDC INIT
/////////////////////////////////////////////////////////////////////////////////////
LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_EN, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN VALUE
/////////////////////////////////////////////////////////////////////////////////////
class Value {
  private:
    volatile int32_t value;
    volatile char selectedDigitId = 0;
    char maxLength;
  public:

    String unit;
    int32_t range[2];
    uint8_t resolution;
    uint8_t minimalChange;
    bool isHighlited;
    uint8_t associatedDisplay;
    uint8_t lcdPosition[2];
    /////////////////////////
    Value(int32_t value, String unit, int32_t range[], uint8_t resolution, uint8_t minimalChange, bool isHighlited, uint8_t associatedDisplay, uint8_t lcdPosition[2]) {
      this->value = value;
      this->unit = unit;

      this->range[0] = range[0];
      this->range[1] = range[1];

      this->resolution = resolution;
      this->minimalChange = minimalChange;
      this->isHighlited = isHighlited;
      this->associatedDisplay = associatedDisplay;

      this->lcdPosition[0] = lcdPosition[0];
      this->lcdPosition[1] = lcdPosition[1];

      this->maxLength = this->setBufferMaxLength();
    };
    /////////////////////////    /////////////////////////    /////////////////////////
    // setters
    /////////////////////////    /////////////////////////    /////////////////////////
    /////////////////////////
    bool setValue(int32_t value) {
      this->value = value;
      this->checkValueBounds();
      return true;
    }
    /////////////////////////
    byte moveByValue(int8_t moveStep) {
      byte msgCode = 0;
      int32_t tmpVal = this->value + moveStep;
      if (this->range[0] <= tmpVal && this->range[1] >= tmpVal) {
        this->setValue(tmpVal);
      } else {
        msgCode = -2;
      }
    }
    /////////////////////////
    int32_t returnBufferedStep() {
      int32_t bufferedStep = 1;
      for (byte i = 0; i < (this->maxLength - this->selectedDigitId) - 1; i++) {
        bufferedStep *= 10;
      }
      return bufferedStep;
    }
    /////////////////////////
    byte moveByValueBuffered(int8_t moveStep) {
      byte msgCode = 0;
      int32_t moveValue = moveStep * this->returnBufferedStep();
      int32_t tmpVal = this->value + moveValue;
      if (this->range[0] <= tmpVal && this->range[1] >= tmpVal) {
        this->setValue(tmpVal);
      } else {
        msgCode = -2;
      }
    }
    /////////////////////////
    void setSelectedDigitId(int8_t digitId) {
      this->selectedDigitId = digitId;
    }
    /////////////////////////
    void circulateSelectedDigitId(int8_t moveStep) {
      byte tmpDigitIdValue = this->selectedDigitId + moveStep;
      if (tmpDigitIdValue > this->maxLength - 1) tmpDigitIdValue = 0;
      this->selectedDigitId = tmpDigitIdValue;
    }
    /////////////////////////
    byte setBufferMaxLength() {
      int32_t number = abs(this->range[1]) / 10;
      byte digits = 1;
      while (number > 0) {
        digits ++;
        number /= 10;
      }

      return (digits ); //
    }
    /////////////////////////
    void checkValueBounds() {
      if (this->value > this->range[1]) this->value = this->range[1];
      if (this->value < this->range[0]) this->value = this->range[0];
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // getters
    /////////////////////////    /////////////////////////    /////////////////////////
    bool isValueSigned() {
      if (this->range[0] < 0 && this->range[1] > 0) {
        return true;
      }
      return false;
    }
    /////////////////////////
    uint8_t getBufferLength() {
      return this->maxLength;
    }
    /////////////////////////
    uint8_t getSelectedDigitId() {
      return this->selectedDigitId;
    }
    /////////////////////////
    int8_t getValueSign() {
      if (this->value < 0) return -1;
      return 1;
    }
    /////////////////////////
    int32_t getValue() {
      return this->value;
    }
    /////////////////////////
    String getUnit() {
      return this->unit;
    }
    /////////////////////////
    uint8_t getResolution() {
      return this->resolution;
    }
    /////////////////////////
    uint8_t getMinimalChange() {
      return this->minimalChange;
    }
    /////////////////////////
    bool isHighlitable() {
      return this->isHighlited;
    }
    /////////////////////////
    uint8_t getAssociatedDisplay() {
      return this->associatedDisplay;
    }
    /////////////////////////
    uint8_t* getLcdPosition() {
      return this->lcdPosition;
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// SIGNALS GENERATION AND STORAGE
/////////////////////////////////////////////////////////////////////////////////////
class SGEN_Waveforms {
  public:
    uint8_t waveform[256];
    volatile uint16_t stepV;
    //uint8_t currentAmplitude = 0;

    /////////////////////////    /////////////////////////    /////////////////////////
    // waveform generation
    /////////////////////////    /////////////////////////    /////////////////////////
    ///////////////////////////////////////////////////////////// above 22Hz
    void waveformDDSGenerate() {
      uint16_t register counter = 0;
      
      while (1) {
        counter += stepV;
        DDS_PORT = waveform[counter >> 8];
      }
    }
    /////////////////////////
    /*
    void waveformDDSGenerate2(uint32_t frequency) {
      uint16_t stepV = uint16_t(frequency / FREQUENCY_TO_STEP);
      uint16_t counter = 0;
      uint16_t cont = 0;

      while (1) {
        counter += stepV;
        DDS_PORT = this->waveform[counter >> 8];
        cont++;
        if (cont == 65535) {
          return;
        }
      }
    }
    */
    /////////////////////////
    uint16_t hardwareFrequencyToCounts(int32_t frequency) {
      uint16_t counts = uint16_t(float(123.457 / frequency * 65535 * ATMEGA_FREQUENCY_MHZ / 16));
      // 65535 counts = 8ms = 123.457Hz
      return counts;
    }
    /////////////////////////
    void waveformHardwarePwmGenerate(int32_t frequency) {
      pinMode(PWM_HS_PIN, OUTPUT);
      TCCR1A = 0;
      TCCR1B = 0;
      TCNT1  = 0;
      //
      OCR1A = this->hardwareFrequencyToCounts(frequency); // toggle after counting to X
      TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match.
      TCCR1B |= (1 << WGM12);    // CTC mode
      TCCR1B |= (1 << CS10) ;     // clock on, no pre-scaler

    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // DDS software waveform manipulations
    /////////////////////////    /////////////////////////    /////////////////////////
    void waveformRewriteToRAM(const uint8_t* waveform) {
      for (uint16_t i = 0; i < 256; i++) {
        this->waveform[i] = pgm_read_byte_near(waveform + i);
      }
    }
    /////////////////////////
    void waveformSoftwareAmplitudePercent(uint8_t percent) {
      for (uint16_t i = 0; i < 256; i++) {
        uint16_t tmpValue = uint16_t(this->waveform[i]) * percent / 100;
        this->waveform[i] = uint8_t(tmpValue);
      }
    }
    /////////////////////////
    void waveformSoftwarePWM(uint8_t dutyCycle) {
      for (uint16_t i = 0; i < 256; i++) {
        uint16_t bound = 256 * dutyCycle / 100;
        if (i < bound) {
          this->waveform[i] = 255;
        } else {
          this->waveform[i] = 0;
        }
      }
    }
    ///////////////////////// for dev purp.
    void readWaveform() {
      for (uint16_t i = 0; i < 256; i++) {
        //Serial.println(this->waveform[i]);
      }
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // waveform load
    /////////////////////////    /////////////////////////    /////////////////////////
    void waveformLoad(int32_t waveformID, uint8_t amplitude, uint8_t dutyCycle) {
      const uint8_t* waveform;
      // waveform select
      switch (waveformID) {
        case SINUS:
          waveform = SINE_WAVE;
          this->waveformRewriteToRAM(waveform);
          break;
        case SAWTOOTH:
          waveform = SAWTOOTH_WAVE;
          this->waveformRewriteToRAM(waveform);
          break;
        case SAWTOOTH_INVERSE:
          waveform = REV_SAWTOOTH_WAVE;
          this->waveformRewriteToRAM(waveform);
          break;
        case PWM_SOFTWARE:
          waveform = SQUARE_WAVE;
          this->waveformSoftwarePWM(dutyCycle);
          break;
        case ECG:
          waveform = ECG_WAVE;
          this->waveformRewriteToRAM(waveform);
          break;
        case NOISE:
          waveform = NOISE_SIGNAL;
          this->waveformRewriteToRAM(waveform);
          break;
        case TRIANG:
          waveform = TRIANGLE_WAVE;
          this->waveformRewriteToRAM(waveform);
          break;
          
      }

      // amplitude adjustment
      //if (amplitude != this->currentAmplitude) {
      this->waveformSoftwareAmplitudePercent(amplitude);
      //this->currentAmplitude = amplitude;
      //}
    }
};

SGEN_Waveforms mySGEN_Waveforms;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN VALUES
/////////////////////////////////////////////////////////////////////////////////////
class SGEN_Values {
  public:
    Value valuesArray[6] = {
      // [ value, unit, range[2], resolution, minimal change, is regulated?, screen, lcd position ]
      Value(0, "", new int32_t[2]{0, 6} , 1, 1, false, 0, new uint8_t[2]{1, 0}), // [0] type of waveform
      Value(1000, "", new int32_t[2]{20, 99999} , 1, 1, true, 0, new uint8_t[2]{6, 0}), // [1] frequency
      Value(0, "", new int32_t[2]{0, 1} , 1, 1, false, 0, new uint8_t[2]{15, 0}), // [2] output state
      Value(50, "", new int32_t[2]{0, 50}, 10, 1, false, 0, new uint8_t[2]{1, 1}), // [3] amplitude resistor network regulated after generator network 0 - 5 Volts
      //Value( 0, "", new int32_t[2]{ -50, 50}, 10, 1, false, 0, new uint8_t[2]{9, 1}), // [4] offset pwm regulated with respect to 2.5V
      Value(50, "", new int32_t[2]{ 1, 99}, 1, 1, false, 0, new uint8_t[2]{10, 1}), // [4] duty cycle 1 - 99 %
      Value(1000, "", new int32_t[2]{150, 11000000} , 1, 1, true, 1, new uint8_t[2]{5, 0}), // [5] frequency fast MHz
      
      //Value(0, "", new int32_t[2]{ -100000, 100000}, 1, 1, true, 1, new char[2]{11, 1}), // [7] sweep
    };
    volatile uint8_t activeRegulationOnValue = 0;
    uint8_t actives = 6;
    uint8_t attenuators[3][2] = {
      {27, 2},
      {26, 4},
      {25, 8},
    };
    uint8_t registeredAttenuatorsNo = 3;
  public:

    /////////////////////////    /////////////////////////    /////////////////////////
    // getters
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t getActive() {
      return activeRegulationOnValue;
    }
    /////////////////////////
    int8_t getTotalValues() {
      return this->actives;
    }

    /////////////////////////    /////////////////////////    /////////////////////////
    // setters
    /////////////////////////    /////////////////////////    /////////////////////////
    void onSet(int8_t what) {
      // waveform select and duty cycle requires waveform reload
      switch (what) {
        case WAVETYPE_IDX:
          {
            uint8_t softAttenuation = uint8_t(this->amplitudeRegulation_calculateSoftAttenuation(this->valuesArray[AMPLITUDE_IDX].getValue()));
            mySGEN_Waveforms.waveformLoad(this->valuesArray[WAVETYPE_IDX].getValue(), softAttenuation, this->valuesArray[DUTYCYCLE_IDX].getValue());
            break;
          }
        case FREQUENCY_IDX:
          mySGEN_Waveforms.stepV = this->valuesArray[what].getValue()/FREQUENCY_TO_STEP;
          break;
        case OUTPUTSTATE_IDX:
          this->setOutputState(this->valuesArray[what].getValue());
          break;
        case AMPLITUDE_IDX:
          this->amplitudeRegulationMaster(this->valuesArray[what].getValue());
          break;
        case OFFSET_IDX:
          this->offsetRegulationMaster(this->valuesArray[what].getValue());
          break;
        case FAST_PWM:
          mySGEN_Waveforms.waveformHardwarePwmGenerate(this->valuesArray[what].getValue());
          break;
        case DUTYCYCLE_IDX:
          {
            if (this->valuesArray[WAVETYPE_IDX].getValue() == PWM_SOFTWARE) {
             this->onSet(0);
            }
            break;
          }
      }
    }
    /////////////////////////
    void setValue(int8_t what, int8_t value)
    {
      valuesArray[what].setValue(value);
      activeRegulationOnValue = what;
      onSet(what);
    }
    /////////////////////////
    void setActive(int8_t what)
    {
      this->activeRegulationOnValue = what;
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // output regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    void setOutputState(int8_t state) {
      if (state == 1) {
        // enable hardware HS PWM
        mySGEN_Waveforms.waveformHardwarePwmGenerate(this->valuesArray[FAST_PWM].getValue());
        // enable software DDS
        pinMode(SGEN_OUTPUT_DISABLE_PIN, INPUT);
        digitalWrite(SGEN_OUTPUT_DISABLE_PIN, LOW);
        DDRC = 0xff;
      } else {
        // disable hardware HS PWM
        TCCR1A = 0;
        TCCR1B = 0;
        // disable software DDS
        DDRC = 0x00;
        pinMode(SGEN_OUTPUT_DISABLE_PIN, OUTPUT);
        digitalWrite(SGEN_OUTPUT_DISABLE_PIN, LOW);
      }
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // amplitude regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t amplitudeRegulation_returnVoltageforAtten(int8_t attenuatorId) {
      int8_t voltageTimesTen = MAXIMUM_OUTPUT_AMPLITUDE_PEAK_V * 10;
      if (attenuatorId > -1) {
        voltageTimesTen = MAXIMUM_OUTPUT_AMPLITUDE_PEAK_V * 10 / this->attenuators[attenuatorId][1];
      }
      return voltageTimesTen;
    }
    /////////////////////////
    int8_t amplitudeRegulation_pickCorrectAttenuatorPin(uint8_t expectedAmplitudePeak_V_timesTen) {
      //int8_t attenuatorMatchId = -1;
      // voltages deacreasing as "i" rises
      for (uint8_t i = 0; i < this->registeredAttenuatorsNo; i++) {
        if (this->amplitudeRegulation_returnVoltageforAtten(i) < expectedAmplitudePeak_V_timesTen) {
          return i - 1;
        }
      }
      //
      return (this->registeredAttenuatorsNo - 1);
    }
    /////////////////////////
    void amplitudeRegulation_freeAttenuators() {
      for (uint8_t i = 0; i < this->registeredAttenuatorsNo; i++) {
        pinMode(attenuators[i][0], INPUT);
        digitalWrite(attenuators[i][0], LOW);
      }
    }
    /////////////////////////
    int32_t amplitudeRegulation_calculateSoftAttenuation(int32_t setAmplitudePeak_V_timesTen) {
      int8_t attenuatorId = this->amplitudeRegulation_pickCorrectAttenuatorPin(setAmplitudePeak_V_timesTen);
      return (setAmplitudePeak_V_timesTen * 100 / this->amplitudeRegulation_returnVoltageforAtten(attenuatorId));
    }
    /////////////////////////
    void amplitudeRegulationMaster(int32_t setAmplitudePeak_V_timesTen) {
      int8_t attenuatorId = this->amplitudeRegulation_pickCorrectAttenuatorPin(setAmplitudePeak_V_timesTen);
      //
      this->amplitudeRegulation_freeAttenuators();
      if (attenuatorId > -1) {
        pinMode(this->attenuators[attenuatorId][0], OUTPUT);
        digitalWrite(this->attenuators[attenuatorId][0], LOW);
      }
      int32_t percentSoftAttenuation = this->amplitudeRegulation_calculateSoftAttenuation(setAmplitudePeak_V_timesTen);
      //
      mySGEN_Waveforms.waveformLoad(this->valuesArray[WAVETYPE_IDX].getValue(), percentSoftAttenuation, this->valuesArray[DUTYCYCLE_IDX].getValue());
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // offset regulation // OFFSET IS DISABLED IN PORTABLE DEVICE
    /////////////////////////    /////////////////////////    /////////////////////////
    void offsetRegulationPWM(uint8_t dutyCycle) {
      uint8_t ocrValue = map(dutyCycle, 0, 100, 0, 255);
      //
      pinMode(15, OUTPUT);
      TCCR2 = 0;
      TCCR2  |= (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS20);
      OCR2 = ocrValue;
      //
    }
    /////////////////////////
    void offsetRegulationMaster(int32_t setOffset_V_timesTen) {
      // tests
      // max    :               ( 50            +      5       *       10 )   *    100   /     5 * 2 * 10  = 100
      // middle :               (  0            +      5       *       10 )   *    100   /     5 * 2 * 10  =  50
      // min    :               (-50            +      5       *       10 )   *    100   /     5 * 2 * 10  =  50

      int32_t dutyCycle = (setOffset_V_timesTen + MAXIMUM_OFFSET_VOLTAGE * 10) * 100 / (MAXIMUM_OFFSET_VOLTAGE * 20);
      this->offsetRegulationPWM(uint8_t(dutyCycle));
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // input regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t activeValueCirculateSelectedDigitId(int8_t stepMove)
    {
      if (!this->valuesArray[activeRegulationOnValue].isHighlitable()) {
        return -1;
      }

      //
      switch (activeRegulationOnValue) {
        default: {
            this->valuesArray[activeRegulationOnValue].circulateSelectedDigitId(stepMove);
            break;
          }
      }

      return 1;
    }
    /////////////////////////
    byte activeValueCirculateSelectedDigit(int8_t stepMove)
    {
      if (this->valuesArray[activeRegulationOnValue].isHighlitable()) {
        valuesArray[activeRegulationOnValue].moveByValueBuffered(stepMove);
      } else {
        this->valuesArray[activeRegulationOnValue].moveByValue(stepMove);
      }
      this->onSet(activeRegulationOnValue);
      return 1;
    }
    /////////////////////////
    int8_t activeValueCirculate(int8_t value) {
      if (value > 0) {
        value = 1;
      } else {
        value = -1;
      }

      activeRegulationOnValue += value;


      if (activeRegulationOnValue > actives - 1) {
        activeRegulationOnValue = 0;
      }
      if (activeRegulationOnValue < 0) {
        activeRegulationOnValue = actives - 1;
      }
      return 1;
    }
    /////////////////////////
};
SGEN_Values mySGEN_Values;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN DISPLAY
/////////////////////////////////////////////////////////////////////////////////////
class SGEN_Display {

  private:
    byte* softFrequencyDigitsBuffer = new byte [mySGEN_Values.valuesArray[FREQUENCY_IDX].getBufferLength()];
    byte* hardwareFrequencyDigitsBuffer = new byte [mySGEN_Values.valuesArray[FAST_PWM].getBufferLength()];
    //
    //byte activeScreen = 0;
    byte lastScreen = -1;
    //
    int8_t lastActiveValue = 0;
    //
    volatile byte lastUserAction = ACTION_ENCODER_MOVED;
    String activeCharacter = "*";
    volatile bool digitHighlightActive = false;

  public:

    /////////////////////////    /////////////////////////    /////////////////////////
    // general transformations
    /////////////////////////    /////////////////////////    /////////////////////////
    String formatIntAsString(int32_t fullNumber, int divider) {
      String str_numb = "";
      if (fullNumber < 0) {
        fullNumber = abs(fullNumber);
        str_numb += "-";
      }

      int32_t tot = fullNumber / divider;
      str_numb += String(tot);

      if (divider > 1) {
        str_numb += ".";
        int32_t part = fullNumber - tot * divider;

        // how many zeroes has divider
        int divCopy = divider;
        int zerMax = 0;
        do {
          zerMax++;
          divCopy /= 10;
        } while (divCopy > 1);

        // how many zeroes has partial number
        int32_t partCpy = part;
        int zerPart = 0;
        do {
          zerPart++;
          partCpy /= 10;

        } while (partCpy > 0);

        // zeroes inject
        for (int8_t i = 0; i < zerMax - zerPart; i++) {
          str_numb += "0";
        }

        str_numb += String(part);
      }
      return str_numb;
    }
    ///////////////////
    void valueToBuffer(byte* digitsBuffer, uint8_t i) {
      // prefill buffer with zeros
      byte maxLength = mySGEN_Values.valuesArray[i].getBufferLength();
      //byte softFrequencyDigitsBuffer[maxLength];
      for (byte i = 0; i < maxLength; i++) {
        digitsBuffer[i] = 0;
      }

      // fill from the back
      int32_t number = mySGEN_Values.valuesArray[i].getValue();
      int digitPosition = maxLength - 1;
      while (number > 0) {
        digitsBuffer[digitPosition] = number % 10;
        number /= 10;
        digitPosition --;
      }
    }
    ///////////////////
    String codeToWavetype(int32_t wvCode) {
      switch (wvCode) {
        case SINUS:
          return "SIN";
          break;
        case SAWTOOTH:
          return "SAW";
          break;
        case SAWTOOTH_INVERSE:
          return "ISAW";
          break;
        case PWM_SOFTWARE:
          return "RCTG";
          break;
        case ECG:
          return "ECG";
          break;
        case NOISE:
          return "NOI";
          break;
        case TRIANG:
          return "TRNG";
          break; 
      }
    }

    /////////////////////////    /////////////////////////    /////////////////////////
    // screen 1
    /////////////////////////    /////////////////////////    /////////////////////////
    void displayWavetype() {
      int32_t val = mySGEN_Values.valuesArray[WAVETYPE_IDX].getValue();
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[WAVETYPE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String tmpString = this->codeToWavetype(mySGEN_Values.valuesArray[WAVETYPE_IDX].getValue());
      lcd.print(tmpString + " ");
    }
    /////////////////////////
    //
    void displaySoftFrequency() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[FREQUENCY_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[FREQUENCY_IDX]);
      String frequencyBuffer = " ";
      if (mySGEN_Values.valuesArray[FREQUENCY_IDX].getValueSign() == -1) {
        frequencyBuffer = "-";
      }
      this->valueToBuffer(softFrequencyDigitsBuffer, FREQUENCY_IDX);
      for (byte i = 0; i < mySGEN_Values.valuesArray[FREQUENCY_IDX].getBufferLength(); i++) {
        frequencyBuffer.concat(softFrequencyDigitsBuffer[i]);
      }
      //String tmpString = String(mySGEN_Values.valuesArray[1].getValue());
      lcd.print(frequencyBuffer+"Hz");

    }
    ///////////////////
    void displayStatus() {
      String enStat = "D";
      if (mySGEN_Values.valuesArray[OUTPUTSTATE_IDX].getValue() == 1) {
        enStat = "E";
      }
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[OUTPUTSTATE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      lcd.print(enStat);
    }
    ///////////////////
    void displayAmplitude() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[AMPLITUDE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "A:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[AMPLITUDE_IDX].getValue(), mySGEN_Values.valuesArray[AMPLITUDE_IDX].getResolution()));
      lcd.print(amplitudeString + "V ");
    }
    ///////////////////
    void displayOffset() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[4].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "O:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[OFFSET_IDX].getValue(), mySGEN_Values.valuesArray[OFFSET_IDX].getResolution()));
      lcd.print(amplitudeString + "V ");
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // screen 2
    /////////////////////////    /////////////////////////    /////////////////////////
    void displayHardFrequency() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[5].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String frequencyBuffer = " ";
      if (mySGEN_Values.valuesArray[5].getValueSign() == -1) {
        frequencyBuffer = "-";
      }
      this->valueToBuffer(hardwareFrequencyDigitsBuffer, FAST_PWM);
      for (byte i = 0; i < mySGEN_Values.valuesArray[FAST_PWM].getBufferLength(); i++) {
        frequencyBuffer.concat(hardwareFrequencyDigitsBuffer[i]);
      }
      lcd.print(frequencyBuffer+"Hz");

    }
    ///////////////////
    void displayDutyCycle() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "D:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getValue(), mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getResolution()));
      lcd.print(amplitudeString + "% ");
    }
    ///////////////////
    /*
      void SGEN_Display::clearActive() {
      for (int i = 0; i < sizeof(assocCoords); i++) {
        lcd.setCursor(assocCoords[i][0] - 1, assocCoords[i][1]);
        lcd.print(" ");
      }
      }
    */
    /////////////////////////    /////////////////////////    /////////////////////////
    // markers and selections
    /////////////////////////    /////////////////////////    /////////////////////////
    void setActiveCharacter(String charct) {
      activeCharacter = charct;
    }
    ///////////////////
    void setHighlight(bool highlight) {
      this->digitHighlightActive = highlight;
    }
    ///////////////////
    void setLastUserAction(byte action) {
      this->lastUserAction = action;
    }
    ///////////////////
    void clearLastSelected() {
      if (this->lastActiveValue > -1) {
        uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[lastActiveValue].getLcdPosition();
        lcd.setCursor(lcdPositionArr[0] - 1, lcdPositionArr[1]);
        lcd.print(" ");
      }
    }
    ///////////////////
    void displaySelected() {
      this->clearLastSelected();
      int thisActive = mySGEN_Values.getActive();
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[thisActive].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0] - 1, lcdPositionArr[1]);
      lcd.print(activeCharacter);
      lastActiveValue = thisActive;
    }
    ///////////////////
    void displayError(int errorCode) {
      lcd.setCursor(1, 0);
      //lcd.print("ERROR:" + errorCodesToStrings(errorCode));
      _delay_ms(1000);
    }
    ///////////////////
    void displayHighlighted() {
      uint8_t thisActive = mySGEN_Values.getActive();
      if (this->digitHighlightActive && mySGEN_Values.valuesArray[thisActive].isHighlited) {
        lcd.blink();
        uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[thisActive].getLcdPosition();
        uint8_t selectedId = mySGEN_Values.valuesArray[thisActive].getSelectedDigitId();
        lcd.setCursor(lcdPositionArr[0] + selectedId + 1, lcdPositionArr[1]); // 1 for sign
      } else {
        lcd.noBlink();
      }
    }

    /////////////////////////    /////////////////////////    /////////////////////////
    // selective display, static content
    /////////////////////////    /////////////////////////    /////////////////////////
    void displayBatteryStats() {
      float analogValue = 0;
      for(uint8_t i=0; i<ADC_BATTERY_SENSE_SMOOTH; i++) {
      analogValue += analogRead(ADC_BATTERY_SENSE_PIN); // 0 - 1023
      }
      analogValue /= ADC_BATTERY_SENSE_SMOOTH;
      float voltage = analogValue * ATMEGA_VOLTAGE / 1023;
      float voltagePercent = min(max(0, 100 * (voltage - BATTERY_VOLTAGE_LOWEST) / (BATTERY_VOLTAGE_TOP - BATTERY_VOLTAGE_LOWEST)), 100);
      //
      lcd.setCursor(1, 1); 
      lcd.print("BATTERY: " + String(int(voltagePercent)) + "%");
      
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // selective display
    /////////////////////////    /////////////////////////    /////////////////////////
    void displayStaticContents(uint8_t screenID) {
      switch (screenID) {
        case 0:
          break;
        case 1:
          lcd.setCursor(1, 0);
          lcd.print("HS");
          this->displayBatteryStats();
          break;
      }
    }
    ///////////////////
    void displayActive() {
      uint8_t thisActive = mySGEN_Values.getActive();
      this->displayByID(thisActive);
    }
    ///////////////////
    void displayByID(uint8_t id) {
      switch (id) {
        case WAVETYPE_IDX:
          this->displayWavetype();
          break;
        case FREQUENCY_IDX:
          this->displaySoftFrequency();
          break;
        case OUTPUTSTATE_IDX:
          this->displayStatus();
          break;
        case AMPLITUDE_IDX:
          this->displayAmplitude();
          break;
        case OFFSET_IDX:
          this->displayOffset();
          break;
        case FAST_PWM:
          this->displayHardFrequency();
          break;
        case DUTYCYCLE_IDX:
          this->displayDutyCycle();
          break;
        case SWEEP_IDX:
          break;
        default:
          //this->displayAll();
          break;
      }
    }
    /////////////////////////
    void fastLCDClear() {
      String clearRow;
      for (uint8_t i = 0; i < 16; i++ ) {
        clearRow.concat(" ");
      }
      lcd.setCursor(0, 0);
      lcd.print(clearRow);
      lcd.setCursor(0, 1);
      lcd.print(clearRow);
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // multi-screen support
    /////////////////////////    /////////////////////////    /////////////////////////
    uint8_t getActiveScreenId() {
      uint8_t activeValue = mySGEN_Values.getActive();
      return mySGEN_Values.valuesArray[activeValue].getAssociatedDisplay();
    }
    ///////////////////
    void displayCurrentScreenContents() {
      this->displayStaticContents(this->getActiveScreenId());
      for (uint8_t i = 0; i < mySGEN_Values.getTotalValues(); i++) {
        if ( mySGEN_Values.valuesArray[i].getAssociatedDisplay() == this->getActiveScreenId()) {
          this->displayByID(i);
        }
      }
    }
    ///////////////////
    uint8_t syncScreenContents() {
      uint8_t currentlyActiveScreen = this->getActiveScreenId();
      if (this->getActiveScreenId() != this->lastScreen) {
        this->lastActiveValue = -1;
        this->fastLCDClear();
        this->displayCurrentScreenContents();
        this->lastScreen = currentlyActiveScreen;
      }
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // main display governors
    /////////////////////////    /////////////////////////    /////////////////////////
    /* deprec
      void displayAll() {
      this->displayWavetype();
      this->displaySoftFrequency();
      this->displayStatus();
      this->displayAmplitude();
      this->displaySelected();
      this->displayHighlighted();
      this->displayOffset();
      lcdRedrawFlag = false;
      }
    */
    ///////////////////
    void displaySelective() {
      this->syncScreenContents();
      switch (lastUserAction) {
        case ACTION_ENCODER_MOVED:
          this->displayActive();
          break;
        case ACTION_ENCODER_SET:
          break;
      }
      this->displaySelected();
      this->displayHighlighted();
      //lcdRedrawFlag = false;
    }
    //String fillBlank(int startEndCoord[], String data);
};
SGEN_Display mySGEN_Display;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN ENCODER CONTROL
/////////////////////////////////////////////////////////////////////////////////////
class SGEN_Regulation {
  private:
    volatile uint8_t menuLevel = 0;
    uint8_t menuLevelBounds[2] = {0, 2};
  public:
    /////////////////////////    /////////////////////////    /////////////////////////
    // getters & setters
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t getMenuLevel() {
      return menuLevel;
    }
    /////////////////////////
    void setMenuLevel(uint8_t level) {
      this->menuLevel = level;
      this->doActionEncoderSet(0);
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t circulateMenuLevels(int8_t value) {
      menuLevel += value;
      //
      if (menuLevel > menuLevelBounds[1]) {
        menuLevel = menuLevelBounds[0];
      }
      if (menuLevel < menuLevelBounds[0]) {
        menuLevel = menuLevelBounds[1];
      }
      return menuLevel;
    }
    /////////////////////////
    void doActionEncoderMove(int8_t value) {
      int8_t response = 1;
      mySGEN_Display.setLastUserAction(ACTION_ENCODER_MOVED);
      //
      switch (menuLevel) {
        case 2:
          response = mySGEN_Values.activeValueCirculateSelectedDigit(value);
          break;
        case 1:
          response = mySGEN_Values.activeValueCirculateSelectedDigitId(value);
          if (response == -1) {
            this->setMenuLevel(2);
          }
          break;
        case 0:
          response = mySGEN_Values.activeValueCirculate(value);
          break;
      }
      //
      mySGEN_Display.displaySelective();
      //lcdRedrawFlag = true;
    }
    /////////////////////////
    void doActionEncoderSet(int8_t value) {
      //
      mySGEN_Display.setLastUserAction(ACTION_ENCODER_SET);
      //
      int menuLevel = circulateMenuLevels(value);
      switch (menuLevel) {
        case 0:
          mySGEN_Display.setActiveCharacter("*");
          mySGEN_Display.setHighlight(false);
          break;
        case 1:
          mySGEN_Display.setActiveCharacter(">");
          mySGEN_Display.setHighlight(true);
          break;
        case 2:
          mySGEN_Display.setActiveCharacter("?");
          mySGEN_Display.setHighlight(true);
          break;
      }
      mySGEN_Display.displaySelective();
      //lcdRedrawFlag = true;
    }
    /////////////////////////
};
SGEN_Regulation mySGEN_Regulation;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN SETUP
/////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //DDRC = 0xff;
  TCCR0 = 0; // arduino time interrupt timer disable
  lcd.begin(16, 2);
  //Serial.begin(9600);
  //Serial.println("Serial ready");
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

