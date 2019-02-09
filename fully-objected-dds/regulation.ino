///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    /////////////////////////    /////////////////////////    /////////////////////////
// S-GEN ENCODER CONTROL
/////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////    /////////////////////////    /////////////////////////
    // getters & setters
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t SGEN_Regulation::getMenuLevel() {
      return menuLevel;
    }
    /////////////////////////
    void SGEN_Regulation::setMenuLevel(uint8_t level) {
      this->menuLevel = level;
      this->doActionEncoderSet(0);
    }
    /////////////////////////    /////////////////////////    /////////////////////////
    // regulation
    /////////////////////////    /////////////////////////    /////////////////////////
    int8_t SGEN_Regulation::circulateMenuLevels(int8_t value) {
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
    void SGEN_Regulation::doActionEncoderMove(int8_t value) {
      
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
    void SGEN_Regulation::doActionEncoderSet(int8_t value) {
      Serial.println(menuLevel);
      //
      mySGEN_Display.setLastUserAction(ACTION_ENCODER_SET);
      //
      this->menuLevel = circulateMenuLevels(value);
      
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

