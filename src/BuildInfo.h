void OutputBuildInfo()
{
  sprintf(Build, "%s %s", BuildTime, BuildDate );
  Serial.print(SKETCH);
  Serial.print(" ");
  Serial.println(Version);
  Serial.print("Build ");
  Serial.println(Build);
  Serial.print("ESP Name ");
  Serial.println(HOSTNAME);

#ifdef ESP32Eink
  display.eraseDisplay();
  display.update();
  //     delay(STARTUPMSGDELAY);
  // display.setRotation(1);
  // display.fillScreen(GxEPD_WHITE);
  // display.update();
  //     delay(STARTUPMSGDELAY);
  // display.fillScreen(GxEPD_BLACK);
  // display.update();
  //     delay(STARTUPMSGDELAY);
  // display.fillScreen(GxEPD_WHITE);
  // display.update();
  //     delay(STARTUPMSGDELAY);
  // display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(0, 0);
  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 260;
  uint16_t box_h = 60;
  if (SHORTVERSION == true )
  {
    uint16_t cursor_y = box_y + 32;
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print("Version:"); display.print(Version);
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    delay(STARTUPMSGDELAY);
  }
  else
  {
    sprintf(Build, "%s %s", BuildTime, BuildDate );
    uint16_t cursor_y = box_y + 32;
    display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    display.setCursor(box_x, cursor_y);
    display.print("Version:"); display.print(Version);
    display.setCursor(box_x, cursor_y + 20);
    display.print("Sketch:"); display.print(SKETCH);
    display.updateWindow(box_x, box_y, box_w, box_h, true);

    delay(STARTUPMSGDELAY);
  }
#endif

#ifdef M5StickC
  BackGroundColor = TFT_DARKGREEN;
  //M5.Lcd.setBrightness(StdScreenBrightness);
  M5.Lcd.fillScreen(BackGroundColor);
  if (SHORTVERSION == true )
  {
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(Version);
    delay(STARTUPMSGDELAY);
  }
  else
  {
    sprintf(Build, "%s %s", BuildTime, BuildDate );
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(Version);
    M5.Lcd.println(SKETCH);
    M5.Lcd.println(BuildTime);
    M5.Lcd.println(BuildDate);
    M5.Lcd.println(HOSTNAME);
    delay(STARTUPMSGDELAY);
  }

  M5.Lcd.fillScreen(BackGroundColor);
  //M5.Axp.SetSleep();                        // turn off screen
  //M5.Lcd.setBrightness(0);                // does not work on a M5 stickC
  delay(STARTUPMSGDELAY);
#endif

#ifdef M5CORE
  M5.Lcd.setBrightness(StdScreenBrightness);
  M5.Lcd.fillScreen(BackGroundColor);
  if (SHORTVERSION == "Yes" )
  {
    M5.Lcd.setCursor(80,90);
    M5.Lcd.setTextSize(10);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(Version);
    delay(STARTUPMSGDELAY);
  }
  else
  {
    sprintf(Build, "%s %s", BuildTime, BuildDate );
    M5.Lcd.setCursor(0,20);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(Version);
    M5.Lcd.println(SKETCH);
    M5.Lcd.println(BuildTime);
    M5.Lcd.println(BuildDate);
    M5.Lcd.println(HOSTNAME);
    delay(STARTUPMSGDELAY);
  }
#endif

}
