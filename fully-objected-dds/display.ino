    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN DISPLAY
/////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////    /////////////////////////    /////////////////////////
    // general transformations
    /////////////////////////    /////////////////////////    /////////////////////////
    String SGEN_Display::formatIntAsString(int32_t fullNumber, int divider) {
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
    void SGEN_Display::valueToBuffer(byte* digitsBuffer, uint8_t i) {
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
    String SGEN_Display::codeToWavetype(int32_t wvCode) {
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
    void SGEN_Display::displayWavetype() {
      int32_t val = mySGEN_Values.valuesArray[WAVETYPE_IDX].getValue();
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[WAVETYPE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String tmpString = this->codeToWavetype(mySGEN_Values.valuesArray[WAVETYPE_IDX].getValue());
      lcd.print(tmpString + " ");
    }
    /////////////////////////
    //
    void SGEN_Display::displaySoftFrequency() {
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
    void SGEN_Display::displayStatus() {
      String enStat = "D";
      if (mySGEN_Values.valuesArray[OUTPUTSTATE_IDX].getValue() == 1) {
        enStat = "E";
      }
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[OUTPUTSTATE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      lcd.print(enStat);
    }
    ///////////////////
    void SGEN_Display::displayAmplitude() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[AMPLITUDE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "A:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[AMPLITUDE_IDX].getValue(), mySGEN_Values.valuesArray[AMPLITUDE_IDX].getResolution()));
      lcd.print(amplitudeString + "V ");
    }
    ///////////////////
    void SGEN_Display::displayOffset() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[4].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "O:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[OFFSET_IDX].getValue(), mySGEN_Values.valuesArray[OFFSET_IDX].getResolution()));
      lcd.print(amplitudeString + "V ");
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // screen 2
    /////////////////////////    /////////////////////////    /////////////////////////
    void SGEN_Display::displayHardFrequency() {
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
    void SGEN_Display::displayDutyCycle() {
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0], lcdPositionArr[1]);
      String amplitudeString = "D:";
      amplitudeString.concat(this->formatIntAsString(mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getValue(), mySGEN_Values.valuesArray[DUTYCYCLE_IDX].getResolution()));
      lcd.print(amplitudeString + "% ");
    }
    ///////////////////
    /////////////////////////    /////////////////////////    /////////////////////////
    // markers and selections
    /////////////////////////    /////////////////////////    /////////////////////////
    void SGEN_Display::setActiveCharacter(String charct) {
      activeCharacter = charct;
    }
    ///////////////////
    void SGEN_Display::setHighlight(bool highlight) {
      this->digitHighlightActive = highlight;
    }
    ///////////////////
    void SGEN_Display::setLastUserAction(byte action) {
      this->lastUserAction = action;
    }
    ///////////////////
    void SGEN_Display::clearLastSelected() {
      if (this->lastActiveValue > -1) {
        uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[lastActiveValue].getLcdPosition();
        lcd.setCursor(lcdPositionArr[0] - 1, lcdPositionArr[1]);
        lcd.print(" ");
      }
    }
    ///////////////////
    void SGEN_Display::displaySelected() {
      this->clearLastSelected();
      int thisActive = mySGEN_Values.getActive();
      uint8_t *lcdPositionArr = mySGEN_Values.valuesArray[thisActive].getLcdPosition();
      lcd.setCursor(lcdPositionArr[0] - 1, lcdPositionArr[1]);
      lcd.print(activeCharacter);
      lastActiveValue = thisActive;
    }
    ///////////////////
    void SGEN_Display::displayError(int errorCode) {
      lcd.setCursor(1, 0);
      //lcd.print("ERROR:" + errorCodesToStrings(errorCode));
      _delay_ms(1000);
    }
    ///////////////////
    void SGEN_Display::displayHighlighted() {
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
    void SGEN_Display::displayBatteryStats() {
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
    void SGEN_Display::displayStaticContents(uint8_t screenID) {
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
    void SGEN_Display::displayActive() {
      uint8_t thisActive = mySGEN_Values.getActive();
      this->displayByID(thisActive);
    }
    ///////////////////
    void SGEN_Display::displayByID(uint8_t id) {
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
    void SGEN_Display::fastLCDClear() {
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
    uint8_t SGEN_Display::getActiveScreenId() {
      uint8_t activeValue = mySGEN_Values.getActive();
      return mySGEN_Values.valuesArray[activeValue].getAssociatedDisplay();
    }
    ///////////////////
    void SGEN_Display::displayCurrentScreenContents() {
      this->displayStaticContents(this->getActiveScreenId());
      for (uint8_t i = 0; i < mySGEN_Values.getTotalValues(); i++) {
        if ( mySGEN_Values.valuesArray[i].getAssociatedDisplay() == this->getActiveScreenId()) {
          this->displayByID(i);
        }
      }
    }
    ///////////////////
    uint8_t SGEN_Display::syncScreenContents() {
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
    ///////////////////
    void SGEN_Display::displaySelective() {
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
