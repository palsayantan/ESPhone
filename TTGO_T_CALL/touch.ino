void touch() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
  {
    //tft.fillCircle(x, y, 3, TFT_BLACK);
    if ((y > 0) && (y < 20)) {
      if ((x > 60) && (x < 180)) {
        Serial.println("Top Panel");
        clrscr();
        panel = !panel;
      }
    }
    if ((y > 300) && (y < 320)) {
      if ((x > 20) && (x < 50)) {
        Serial.println("Recent");
      }
      if ((x > 100) && (x < 140)) {
        Serial.println("Home");
        if (Home) {
          clrscr();
          Menu = true;
        }
        else {
          clrscr();
          Home = true;
          updateClock();
        }
      }
      if ((x > 190) && (x < 220)) {
        Serial.println("Back");
        if (!Home) {
          clrscr();
          Menu = true;
        }
      }
    }
    if (Home) {
      if (y > 45 && y < 120) {
        if (x > 45 && x < 200) {
          setTimerFunc();
        }
      }
      if ((y > 240) && (y < (240 + iV))) {
        if ((x > ix1) && (x < (ix1 + iH))) {
          Serial.println("Call");
          clrscr();
          phNum = "";
          pos = 0;
          call = true;
        }
        if ((x > ix2) && (x < (ix2 + iH))) {
          Serial.println("Message");
          clrscr();
          message = true;
        }
        if ((x > ix3) && (x < (ix3 + iH))) {
          Serial.println("Contact");
          clrscr();
          contact = true;
        }
        if ((x > ix4) && (x < (ix4 + iH))) {
          Serial.println("settings");
          clrscr();
          settings = true;
        }
      }
    }
    if (panel) {

    }
    if (Menu) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          if ((y > menuY[i]) && (y < menuY[i] + iV)) {
            if ((x > menuX[j]) && (x < menuX[j] + iH)) {
              int val = (i * 4 + j);
              Serial.println(val);
              clrscr();
              menuSel(val);
            }
          }
        }
      }
    }
  }
}

void menuSel(int val) {
  if (val == 0) {
    calculator = true;
  }
  if (val == 1) {
    calender = true;
  }
  if (val == 2) {
    camera = true;
  }
  if (val == 3) {
    clocks = true;
  }
  if (val == 4) {
    compass = true;
  }
  if (val == 5) {
    gyro = true;
  }
  if (val == 6) {
    weather = true;
  }
  if (val == 7) {
    gps = true;
  }
  if (val == 8) {
    browser = true;
  }
  if (val == 9) {
    rgb = true;
  }
  if (val == 10) {
    gesture = true;
  }
  if (val == 11) {
    info = true;
  }
  if (val == 12) {
    files = true;
  }
  if (val == 13) {
    terminal = true;
  }
  if (val == 14) {
    cpu = true;
  }
  if (val == 15) {
    sim = true;
  }
}
