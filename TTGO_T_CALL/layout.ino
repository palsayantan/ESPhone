
int iH = 50, ix1 = 5, ix2 = 65, ix3 = 125, ix4 = 185;
int iV = 50, iy1 = 35, iy2 = 100, iy3 = 165, iy4 = 230;
int menuX[4] = {5, 65, 125, 185};
int menuY[4] = {35, 100, 165, 230};

int kH = 70, kV = 40, r = 5;
int keyX[3] = {5, 85, 165};
int keyY[4] = {60, 110, 160, 210};

uint16_t red, green, blue;

int pos = 0;
String phNum;
String num1, num2, netNum;
char op = ' ';

char *ico1[] = {"/calculator.bmp", "/calender.bmp", "/camera.bmp", "/clock.bmp",
                "/compass.bmp", "/gyro.bmp", "/weather.bmp", "/gps.bmp",
                "/browser.bmp", "/rgb.bmp", "/gesture.bmp", "/info.bmp",
                "/file.bmp", "/terminal.bmp", "/cpu.bmp", "/sim.bmp",
                "/gallary.bmp", "/music.bmp", "/video.bmp"
               };

char *ico2[] = {"/data.bmp", "/wifi.bmp", "/bluetooth.bmp",
                "/bri2.bmp", "/sou2.bmp", "/nvb.bmp",
                "/aero.bmp", "/sleep.bmp", "/restart.bmp",
                "/bri0.bmp", "/bri1.bmp", "/sou0.bmp", "/sou1.bmp", "/vb.bmp"
               };

char *ico3[] = {"/callConnect.bmp", "/delete.bmp", "/callAccept.bmp", "/callDecline.bmp", "/callDisconnect.bmp", "/id.bmp", "/add.bmp", "/del.bmp", "/edit.bmp", "/cancel.bmp"};

void menuSelect() {
  tft.fillRect(0, 0, 240, 20, TFT_WHITE);                           // TopBar
  tft.fillRect(0, 300, 240, 20, TFT_WHITE);                         // bottomBar
  tft.fillRoundRect(30, 305, 10, 10, 2, TFT_BLACK);                 // menuButton
  tft.fillCircle(115, 310, 5, TFT_BLACK);                           // homeButton
  tft.fillTriangle(200, 315, 200, 305, 207, 310, TFT_BLACK);        // backButton
  // battery,network,wifi,bt
  tft.drawRoundRect(210, 6, 24, 8, 4, TFT_BLACK);                   // battery
  // battery
  int batLvl = map(vBat, 0, 100, 0, 24);
  tft.fillRoundRect(210, 6, batLvl, 8, 4, TFT_BLACK);
  // Network
  if (csq > 0) {
    tft.fillRoundRect(5, 12, 3, 6, 2, TFT_BLACK);
    if (csq > 5) {
      tft.fillRoundRect(10, 9, 3, 9, 2, TFT_BLACK);
      if (csq > 10) {
        tft.fillRoundRect(15, 6, 3, 12, 2, TFT_BLACK);
        if (csq > 20) {
          tft.fillRoundRect(20, 3, 3, 15, 2, TFT_BLACK);
        }
      }
    }
  }

  // Homepage
  if (Home) {
    Clock.update();
    drawBmp("/call.bmp", ix1, 240);
    drawBmp("/chat.bmp", ix2, 240);
    drawBmp("/contact.bmp", ix3, 240);
    drawBmp("/setting.bmp", ix4, 240);
    drawBmp("/logo.bmp", 30, 125);
  }
  // app page
  if (Menu) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        int val = (i * 4 + j);
        drawBmp(ico1[val], menuX[j], menuY[i]);
      }
    }
  }

  if (panel) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int val = (i * 3 + j);
        drawBmp(ico2[val], keyX[j] + 25, keyY[i] - 15);
      }
    }
  }

  if (call) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        tft.drawRoundRect(keyX[j], keyY[i], kH, kV, r, TFT_WHITE);
        drawBmp(ico3[0], 100, 258);
        drawBmp(ico3[1], 205, 28);
        String val = String(i * 3 + j + 1);
        if (val == "11") val = "0";
        if (val == "10") val = "*";
        if (val == "12") val = "#";
        tft.drawString(val, keyX[j] + 27, keyY[i] + 8, 4);
        uint16_t x, y;
        if (tft.getTouch(&x, &y)) {
          if ((x > keyX[j] && x < keyX[j] + 70) && (y > keyY[i] && y < keyY[i] + 40)) {
            tft.drawString(val, 20 + pos * 15, 30, 4);
            phNum += val;
            pos++;
            if (pos > 10) {
              phNum = "";
              pos = 0;
              tft.fillRect(20, 30, 180, 25, TFT_BLACK);
            }
          }
          if ((x > 205 && x < 235) && (y > 25 && y < 50)) {
            tft.fillRect(20 + (pos - 1) * 15, 30, 15, 25, TFT_BLACK);
            phNum = phNum.substring(0, pos - 1);
            pos--;
            delay(200);
          }
          if ((x > 100 && x < 135) && (y > 255 && y < 290)) {
            bool calling = true;
            tft.fillScreen(TFT_BLACK);
            SerialAT.println("ATD+ +91" + phNum + ";");
            tft.drawString("+91" + phNum, 30, 90, 4);
            drawBmp(ico3[5], 100, 30);
            drawBmp(ico3[4], 90, 220);
            while (calling) {
              delay(500);
              uint16_t x, y;
              if (tft.getTouch(&x, &y)) {
                if ((x > 90 && x < 150) && (y > 240 && y < 300)) {
                  calling = false;
                  phNum = "";
                  pos = 0;
                  SerialAT.println("ATH");
                  delay(500);
                  tft.fillScreen(TFT_BLACK);
                }
              }
            }
          }
        }
      }
    }
  }
  if (message) {
    //SerialAT.println("AT+CMGL=\"ALL\"\r");
    while (SerialAT.available()) {
      Serial.println(SerialAT.read());
    }
  }
  if (contact) {

  }
  if (settings) {

  }
  if (calculator) {
    char keys[4][4] = {
      {'7', '8', '9', '/'},
      {'4', '5', '6', '*'},
      {'1', '2', '3', '-'},
      {'C', '0', '=', '+'}
    };
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        char ch = keys[i][j];
        int kx = 5 + j * 60;
        int ky = 120 + i * 45;
        tft.drawRoundRect(kx, ky, 55, 40, r, TFT_WHITE);
        tft.drawString(String(ch), 20 + kx, 8 + ky, 4);
        uint16_t x, y;
        if (tft.getTouch(&x, &y)) {
          if ((x > kx && x < kx + 55) && (y > ky && y < ky + 40)) {
            if (ch != 61) tft.drawString(String(ch), 5 + pos * 15, 30, 4);
            pos++;
            if (ch == 43) op = '+';
            if (ch == 45) op = '-';
            if (ch == 42) op = '*';
            if (ch == 47) op = '/';
            if (ch > 47 && ch < 58) {
              if (op == ' ') num1 += String(ch);
              else num2 += String(ch);
            }
            if (ch == 61) {
              if (op == '+') netNum = String(num1.toInt() + num2.toInt());
              if (op == '-') netNum = String(num1.toInt() - num2.toInt());
              if (op == '*') netNum = String(num1.toInt() * num2.toInt());
              if (op == '/') netNum = String(num1.toInt() / num2.toInt());
              tft.drawString("= " + netNum, 5, 60, 4);
            }
            if (ch == 67) {
              pos = 0; op = ' ';
              num1 = num2 = netNum = "";
              tft.fillScreen(TFT_BLACK);
            }
          }
        }
      }
    }
  }

  if (calender) {
    int monthLength;
    if (MM == 2) {
      if (YY % 400 == 0 || (YY % 4 == 0 && YY % 100 != 0)) monthLength = 29;
      else monthLength = 28;
    }
    if (MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10 || MM == 12) monthLength = 31;
    else monthLength = 30;

    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    YY -= MM < 3;
    int offset = (YY + YY / 4 - YY / 100 + YY / 400 + t[MM - 1] + 1) % 7;
    String dayName[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    String months[13] = {" ", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    tft.drawCentreString(months[MM] + ", " + YY, tft.width() / 2, 30, 4);

    for (int i = 0; i < 7; i++) tft.drawCentreString(dayName[i], 15 + i * 35, 70, 2);

    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 6; j++) {
        int kx = 5 + i * 33;
        int ky = 90 + j * 35;
        if (i == 0)tft.drawRect(kx, ky, 30, 30, TFT_RED);
        else tft.drawRect(kx, ky, 30, 30, TFT_WHITE);

        String dt = String(j * 7 + i + 1 - offset);
        if (dt.toInt() > monthLength || dt.toInt() <= 0) dt = "";
        if (dt.toInt() == DD) tft.drawRect(kx, ky, 30, 30, TFT_BLUE);
        tft.drawString(String(dt), kx + 7, ky + 7, 2);
      }
    }
    tft.drawString("<", 5, 30, 4);
    tft.drawString(">", 220, 30, 4);
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      if ((y > 30 && y < 60) && (x > 5 && x < 25)) MM--;
      if ((y > 30 && y < 60) && (x > 220 && x < 240)) MM++;
      delay(500);
      tft.fillScreen(TFT_BLACK);
    }
  }

  if (clocks) {

  }



  if (rgb) {
    tft.drawString("NeoPixel LED", 40, 40, 4);
    tft.drawRoundRect(40, 100, 128, 7, 3, TFT_RED);
    tft.drawRoundRect(40, 150, 128, 7, 3, TFT_GREEN);
    tft.drawRoundRect(40, 200, 128, 7, 3, TFT_BLUE);
    tft.drawString("R", 10, 90, 4);
    tft.drawString("G", 10, 140, 4);
    tft.drawString("B", 10, 190, 4);

    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      if (x > 40 && x < 170) {
        if (y > 90 && y < 110) {
          red = map(x, 41, 169, 0, 255);
          tft.fillRoundRect(40, 100, 128, 7, 3, TFT_BLACK);
          tft.fillRoundRect(40, 100, red / 2, 7, 3, TFT_RED);
          tft.drawString("           ", 180, 90, 4);
          tft.drawString(String(red), 180, 90, 4);
        }
        if (y > 140 && y < 160) {
          green = map(x, 41, 169, 0, 255);
          tft.fillRoundRect(40, 150, 128, 7, 3, TFT_BLACK);
          tft.fillRoundRect(40, 150, green / 2, 7, 3, TFT_GREEN);
          tft.drawString("           ", 180, 140, 4);
          tft.drawString(String(green), 180, 140, 4);
        }
        if (y > 190 && y < 210) {
          blue = map(x, 41, 169, 0, 255);
          tft.fillRoundRect(40, 200, 128, 7, 3, TFT_BLACK);
          tft.fillRoundRect(40, 200, blue / 2, 7, 3, TFT_BLUE);
          tft.drawString("           ", 180, 190, 4);
          tft.drawString(String(blue), 180, 190, 4);
        }
      }
      leds[0] = CRGB(red, green, blue);
      FastLED.show();
    }
  }

}
