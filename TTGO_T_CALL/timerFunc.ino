
#include <ESP32Time.h>
ESP32Time rtc;

const int timer1 = 60 * 1000;

void setTimerFunc() {
  SerialAT.println("AT+CCLK?");
  while (SerialAT.available()) {
    String response = SerialAT.readString();
    int index = response.indexOf("+CCLK");
    String Date = response.substring(index + 8, index + 16);
    String Time = response.substring(index + 17, index + 25);
    hh = Time.substring(0, 2).toInt();
    mm = Time.substring(3, 5).toInt();
    ss = Time.substring(6, 8).toInt();

    YY = Date.substring(0, 2).toInt() + 2000;
    MM = Date.substring(3, 5).toInt();
    DD = Date.substring(6, 8).toInt();
    rtc.setTime(ss, mm, hh, DD, MM, YY);
  }
  Serial.println(String(hh) + ":" + String(mm) + ":" + String(ss));
  Serial.println(String(DD) + "/" + String(MM) + "/" + String(YY));
  Clock.set(timer1, updateClock);
  updateClock();
}

void updateClock() {
  if (Home || clocks) tft.setTextColor(TFT_CYAN, TFT_BLACK);
  else  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.drawString(rtc.getTime("%HH:%MM"), 45, 45, 7);

  if (Home || clocks) tft.setTextColor(TFT_GREEN, TFT_BLACK);
  else  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.drawString(rtc.getDate(), 70, 100, 2);

  if (Home) tft.setTextColor(TFT_RED, TFT_BLACK);
  else  tft.setTextColor(TFT_BLACK, TFT_BLACK);
  tft.drawString("Vi India", 100, 25, 2);

  //SerialAT.println("AT+CBC");     // vBat
  //SerialAT.println("AT+CSQ");     // strength

}


void incall(String response) {
  if (Serial.available()) {
    SerialAT.write(Serial.read());
  }
  int index = response.indexOf("CLIP");
  String num = response.substring(index + 7, index + 20);
  Serial.println(num);
  tft.drawString(num, 30, 90, 4);
  drawBmp(ico3[5], 100, 30);
  drawBmp(ico3[2], 30, 220);
  drawBmp(ico3[3], 150, 220);

  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    if (y > 220 && y < 292) {
      if (x > 30 && x < 90) {
        Serial.print("Accept");
        SerialAT.println("ATA");
        tft.fillRect(30, 220, 210, 72, TFT_BLACK);
        bool accepted = true;
        while (accepted) {
          drawBmp(ico3[4], 90, 240);
          uint16_t x, y;
          if (tft.getTouch(&x, &y)) {
            if ((x > 90 && x < 150) && (y > 240 && y < 300)) {
              SerialAT.println("ATH");
              accepted = false;
              clrscr();
              Home = true;
            }
          }
        }
      }
      if (x > 150 && x < 210) {
        Serial.print("Reject");
        SerialAT.println("ATH");
        clrscr();
        Home = true;
      }
    }
  }
}
