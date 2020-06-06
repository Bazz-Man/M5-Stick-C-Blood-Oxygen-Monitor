void M5OutputBuildInfo()
{

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

#if defined M5CORE || defined M5StickC
  M5OutputBuildInfo();
#endif

}
