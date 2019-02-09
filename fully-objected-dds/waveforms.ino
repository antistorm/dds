///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// SIGNALS GENERATION AND STORAGE
/////////////////////////////////////////////////////////////////////////////////////
    void SGEN_Waveforms::waveformDDSGenerate() {
      uint16_t register counter = 0;
      
      while (1) {
        counter += stepV;
        DDS_PORT = waveform[counter >> 8];
      }
    }
    /////////////////////////
    uint16_t SGEN_Waveforms::hardwareFrequencyToCounts(int32_t frequency) {
      uint16_t counts = uint16_t(float(123.457 / frequency * 65535 * ATMEGA_FREQUENCY_MHZ / 16));
      // 65535 counts = 8ms = 123.457Hz
      return counts;
    }
    /////////////////////////
    void SGEN_Waveforms::waveformHardwarePwmGenerate(int32_t frequency) {
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
    void SGEN_Waveforms::waveformRewriteToRAM(const uint8_t* waveform) {
      for (uint16_t i = 0; i < 256; i++) {
        this->waveform[i] = pgm_read_byte_near(waveform + i);
      }
    }
    /////////////////////////
    void SGEN_Waveforms::waveformSoftwareAmplitudePercent(uint8_t percent) {
      for (uint16_t i = 0; i < 256; i++) {
        uint16_t tmpValue = uint16_t(this->waveform[i]) * percent / 100;
        this->waveform[i] = uint8_t(tmpValue);
      }
    }
    /////////////////////////
    void SGEN_Waveforms::waveformSoftwarePWM(uint8_t dutyCycle) {
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
    void SGEN_Waveforms::readWaveform() {
      for (uint16_t i = 0; i < 256; i++) {
        //Serial.println(this->waveform[i]);
      }
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // waveform load
    /////////////////////////    /////////////////////////    /////////////////////////
    void SGEN_Waveforms::waveformLoad(int32_t waveformID, uint8_t amplitude, uint8_t dutyCycle) {
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

      // software amplitude adjustment
      this->waveformSoftwareAmplitudePercent(amplitude);
  
    }
    
