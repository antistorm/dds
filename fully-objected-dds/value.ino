///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN VALUE
/////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////
    Value::Value(int32_t value, String unit, int32_t range[], uint8_t resolution, uint8_t minimalChange, bool isHighlited, uint8_t associatedDisplay, uint8_t lcdPosition[2]) {
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
    bool Value::setValue(int32_t value) {
      this->value = value;
      this->checkValueBounds();
      return true;
    }
    /////////////////////////
    byte Value::moveByValue(int8_t moveStep) {
      byte msgCode = 0;
      int32_t tmpVal = this->value + moveStep;
      if (this->range[0] <= tmpVal && this->range[1] >= tmpVal) {
        this->setValue(tmpVal);
      } else {
        msgCode = -2;
      }
    }
    /////////////////////////
    int32_t Value::returnBufferedStep() {
      int32_t bufferedStep = 1;
      for (byte i = 0; i < (this->maxLength - this->selectedDigitId) - 1; i++) {
        bufferedStep *= 10;
      }
      return bufferedStep;
    }
    /////////////////////////
    byte Value::moveByValueBuffered(int8_t moveStep) {
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
    void Value::setSelectedDigitId(int8_t digitId) {
      this->selectedDigitId = digitId;
    }
    /////////////////////////
    void Value::circulateSelectedDigitId(int8_t moveStep) {
      byte tmpDigitIdValue = this->selectedDigitId + moveStep;
      if (tmpDigitIdValue > this->maxLength - 1) tmpDigitIdValue = 0;
      this->selectedDigitId = tmpDigitIdValue;
    }
    /////////////////////////
    byte Value::setBufferMaxLength() {
      int32_t number = abs(this->range[1]) / 10;
      byte digits = 1;
      while (number > 0) {
        digits ++;
        number /= 10;
      }

      return (digits ); //
    }
    /////////////////////////
    void Value::checkValueBounds() {
      if (this->value > this->range[1]) this->value = this->range[1];
      if (this->value < this->range[0]) this->value = this->range[0];
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // getters
    /////////////////////////    /////////////////////////    /////////////////////////
    bool Value::isValueSigned() {
      if (this->range[0] < 0 && this->range[1] > 0) {
        return true;
      }
      return false;
    }
    /////////////////////////
    uint8_t Value::getBufferLength() {
      return this->maxLength;
    }
    /////////////////////////
    uint8_t Value::getSelectedDigitId() {
      return this->selectedDigitId;
    }
    /////////////////////////
    int8_t Value::getValueSign() {
      if (this->value < 0) return -1;
      return 1;
    }
    /////////////////////////
    int32_t Value::getValue() {
      return this->value;
    }
    /////////////////////////
    String Value::getUnit() {
      return this->unit;
    }
    /////////////////////////
    uint8_t Value::getResolution() {
      return this->resolution;
    }
    /////////////////////////
    uint8_t Value::getMinimalChange() {
      return this->minimalChange;
    }
    /////////////////////////
    bool Value::isHighlitable() {
      return this->isHighlited;
    }
    /////////////////////////
    uint8_t Value::getAssociatedDisplay() {
      return this->associatedDisplay;
    }
    /////////////////////////
    uint8_t* Value::getLcdPosition() {
      return this->lcdPosition;
    }
