///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN VALUES
/////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////    /////////////////////////    /////////////////////////
    // getters
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t SGEN_Values::getActive() {
      return activeRegulationOnValue;
    }
    /////////////////////////
    int8_t SGEN_Values::getTotalValues() {
      return this->actives;
    }

    /////////////////////////    /////////////////////////    /////////////////////////
    // setters
    /////////////////////////    /////////////////////////    /////////////////////////
    void SGEN_Values::onSet(int8_t what) {
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
    void SGEN_Values::setValue(int8_t what, int8_t value)
    {
      valuesArray[what].setValue(value);
      activeRegulationOnValue = what;
      onSet(what);
    }
    /////////////////////////
    void SGEN_Values::setActive(int8_t what)
    {
      this->activeRegulationOnValue = what;
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // output regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    void SGEN_Values::setOutputState(int8_t state) {
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
    int8_t SGEN_Values::amplitudeRegulation_returnVoltageforAtten(int8_t attenuatorId) {
      int8_t voltageTimesTen = MAXIMUM_OUTPUT_AMPLITUDE_PEAK_V * 10;
      if (attenuatorId > -1) {
        voltageTimesTen = MAXIMUM_OUTPUT_AMPLITUDE_PEAK_V * 10 / this->attenuators[attenuatorId][1];
      }
      return voltageTimesTen;
    }
    /////////////////////////
    int8_t SGEN_Values::amplitudeRegulation_pickCorrectAttenuatorPin(uint8_t expectedAmplitudePeak_V_timesTen) {
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
    void SGEN_Values::amplitudeRegulation_freeAttenuators() {
      for (uint8_t i = 0; i < this->registeredAttenuatorsNo; i++) {
        pinMode(attenuators[i][0], INPUT);
        digitalWrite(attenuators[i][0], LOW);
      }
    }
    /////////////////////////
    int32_t SGEN_Values::amplitudeRegulation_calculateSoftAttenuation(int32_t setAmplitudePeak_V_timesTen) {
      int8_t attenuatorId = this->amplitudeRegulation_pickCorrectAttenuatorPin(setAmplitudePeak_V_timesTen);
      return (setAmplitudePeak_V_timesTen * 100 / this->amplitudeRegulation_returnVoltageforAtten(attenuatorId));
    }
    /////////////////////////
    void SGEN_Values::amplitudeRegulationMaster(int32_t setAmplitudePeak_V_timesTen) {
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
    void SGEN_Values::offsetRegulationPWM(uint8_t dutyCycle) {
      uint8_t ocrValue = map(dutyCycle, 0, 100, 0, 255);
      //
      pinMode(15, OUTPUT);
      TCCR2 = 0;
      TCCR2  |= (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS20);
      OCR2 = ocrValue;
      //
    }
    /////////////////////////
    void SGEN_Values::offsetRegulationMaster(int32_t setOffset_V_timesTen) {
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
    int8_t SGEN_Values::activeValueCirculateSelectedDigitId(int8_t stepMove)
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
    byte SGEN_Values::activeValueCirculateSelectedDigit(int8_t stepMove)
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
    int8_t SGEN_Values::activeValueCirculate(int8_t value) {
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
