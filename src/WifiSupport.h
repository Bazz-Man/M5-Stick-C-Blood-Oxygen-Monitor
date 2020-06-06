#include <WiFiMulti.h>                //Support for defining multiple WiFi points (used at startup)
#include <ArduinoOTA.h>               //Over The Air updates
#ifdef M5CORE
  #include "XBMImages.h"                // needed for AlertImage xbm image
#endif
#include <ezTime.h>                   // Time support

String WiFiIP;
String WiFiSID;
WiFiMulti wifiMulti;

Timezone UK;


void SetupNTTPTime()
{
  setDebug(ERROR);
  UK.setLocation("Europe/London");
  UK.setDefault();
  BackGroundColor = TFT_BLUE;
  Serial.println("Retrieving Time from Network");

#ifdef M5StickC
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(4,2);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("Retrieving Time");
#endif 

  waitForSync(5);
  Serial.println("UTC: " + UTC.dateTime());

	Serial.println("UK: " + UK.dateTime());
  Serial.println(UK.dateTime("D j~-m~-y g:i a"));

#ifdef M5StickC
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(4,2);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(UK.dateTime("j~-m~-y"));
    M5.Lcd.setCursor(8,26);
    M5.Lcd.println(UK.dateTime("g:i a"));
#endif

  delay(STARTUPMSGDELAY);
}

void ConnectWiFi(int Count)   // if count is zero then it will loop forever
{
  BackGroundColor = TFT_BLUE;
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(Count);
    Serial.print(" - ");
#ifdef M5CORE
      M5.Lcd.drawXBitmap(280,0, WiFiErr, WiFiErrWidth, WiFiErrHeight, TFT_RED);
      M5.Lcd.setCursor(160,100);
      M5.Lcd.setTextSize(4);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println(Count);
#endif

#ifdef M5StickC
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,4);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println(Count);
#endif
    Count--;
    if ( Count == 0)  // its taken to long to connect so cycle the unit
    {
      Serial.println("Connection Failed - Rebooting");
      
#ifdef M5CORE
        M5.Lcd.fillScreen(BackGroundColor);
        M5.Lcd.setCursor(0,20);
        M5.Lcd.setTextSize(4);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("WIFI Connection Failed - Rebooting");
        delay(1000);
#endif
#ifdef M5StickC
        M5.Lcd.fillScreen(BackGroundColor);
        M5.Lcd.setCursor(0,4);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("WIFI Connection Failed - Rebooting");
#endif
      ESP.restart();
    }
  }
#ifdef M5CORE
  M5.Lcd.drawXBitmap(280,195, WiFiErr, WiFiErrWidth, WiFiErrHeight, TFT_BLACK);
#endif
}

void SetupMutiWifi()
{
  BackGroundColor = TFT_BLUE;
  Serial.println("Starting wifi");
#ifdef M5CORE
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,50);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("Starting wifi");
#endif
#ifdef M5StickC
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("Starting wifi");
#endif

  WiFi.setHostname(HOSTNAME);
  wifiMulti.addAP(NETWORKNAME1, NETWORKPASS1);
  wifiMulti.addAP(NETWORKNAME2, NETWORKPASS2);
  wifiMulti.addAP(NETWORKNAME3, NETWORKPASS3);
  wifiMulti.addAP(NETWORKNAME4, NETWORKPASS4);
    
  ConnectWiFi(9);        // restarts after 10 tries

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  WiFiIP = WiFi.localIP().toString().c_str();
  WiFiSID = WiFi.SSID();
  Serial.println(WiFiSID);
  Serial.println(WiFiIP);
#ifdef M5CORE
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(5,20);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(WiFiSID);
    M5.Lcd.setCursor(5,50);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println(WiFiIP);   
#endif
#ifdef M5StickC
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println(WiFiSID);
    M5.Lcd.setCursor(0,24);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println(WiFiIP);  
#endif

  delay(STARTUPMSGDELAY);

  //Now get the time
  SetupNTTPTime();

}


/** START - OAT Code **/
void SetupOTAFunctions()
{  
  BackGroundColor = TFT_BLUE;
  // Start of OTA section

  Serial.println("OTA Setup Starting");
#ifdef M5CORE
    BackGroundColor = TFT_BLACK;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(20,20);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("OTA Initialising");
#endif
#ifdef M5StickC
    BackGroundColor = TFT_BLACK;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("OTA Initialising");
#endif

    
  ArduinoOTA.setPort(OTAPort);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(HOSTNAME);

  // Hostname defaults to esp3232-[MAC]               // ArduinoOTA.setHostname("myesp32");
  // No authentication by default                     // ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]()
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
#ifdef M5CORE
      BackGroundColor = TFT_BLACK;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(50,40);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println("OTA updating");
#endif
#ifdef M5StickC
      BackGroundColor = TFT_BLACK;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,4);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println("OTA updating");
#endif
    //sprintf(MQTTMsg, "%s: OTA update starting", HOSTNAME);  // Construct MQTT message
    delay(100);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    int DProgress = round((progress / (total / 100)));
    Serial.printf("%i%%\n", DProgress);
#ifdef M5CORE
      BackGroundColor = TFT_BLACK;
      M5.Lcd.setCursor(120,100);
      M5.Lcd.setTextSize(6);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.print(DProgress);
      M5.Lcd.print("%");
#endif
#ifdef M5StickC
      BackGroundColor = TFT_BLACK;
      M5.Lcd.setCursor(50,30);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.print(DProgress);
      M5.Lcd.print("%");
#endif
  });
  ArduinoOTA.onEnd([]()
  {
    Serial.println("\nOTA Download Completed");
    //sprintf(MQTTMsg, "%s: OTA update completed successfully", HOSTNAME);  // Construct MQTT message
    //client.publish(StatusTopic, MQTTMsg);
#ifdef M5CORE
      BackGroundColor = TFT_GREEN;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,20);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(BLACK, BackGroundColor);
      M5.Lcd.println("OTA Download Completed");
#endif
#ifdef M5StickC
      BackGroundColor = TFT_GREEN;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,4);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(BLACK, BackGroundColor);
      M5.Lcd.println("OTA Download Completed");
#endif
    delay(STARTUPMSGDELAY);
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    //sprintf(MQTTMsg, "%s: OTA update FAILED", HOSTNAME);  // Construct MQTT message
    //client.publish(StatusTopic, MQTTMsg);
#ifdef M5CORE
      BackGroundColor = TFT_RED;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,20);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println("OTA update FAILED");
#endif
#ifdef M5StickC
      BackGroundColor = TFT_RED;
      M5.Lcd.fillScreen(BackGroundColor);
      M5.Lcd.setCursor(0,4);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(WHITE, BackGroundColor);
      M5.Lcd.println("OTA update");
      M5.Lcd.setCursor(4,24);
      M5.Lcd.setTextSize(3);
      M5.Lcd.println("FAILED");
#endif
    
    delay(STARTUPMSGDELAY);

    if (error == OTA_AUTH_ERROR)         Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");
    
    delay(STARTUPMSGDELAY);
    ESP.restart();
  });
  
  ArduinoOTA.begin();
  
  Serial.println("OTA System Ready");
  Serial.println("Checking for OTA Update");

#ifdef M5CORE
    BackGroundColor = TFT_BLUE;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,20);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("OTA System Ready");
    M5.Lcd.println("Checking for OTA Update");
#endif
#ifdef M5StickC
    BackGroundColor = TFT_BLUE;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("OTA UP");
    M5.Lcd.println("Check Update");
#endif

  for (int i = 2; i < 13; i++)
  {
    if (i % 2) {  // if progess is even then turn on blue pixel, if odd then turn it off
      Serial.println("CHECK FOR OTA");
#ifdef M5CORE
        M5.Lcd.setCursor(10,60);
        M5.Lcd.setTextSize(3);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("XXXX");
#endif
#ifdef M5StickC
        M5.Lcd.setCursor(4,40);
        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("XXXX");
#endif

    }
    else
    {
      Serial.println("|||||||||||||");
#ifdef M5CORE
        M5.Lcd.setCursor(10,60);
        M5.Lcd.setTextSize(3);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("----");
#endif
#ifdef M5StickC
        M5.Lcd.setCursor(4,40);
        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(WHITE, BackGroundColor);
        M5.Lcd.println("----");
#endif
    }

    // Start of OTA check section
    ArduinoOTA.handle();
    yield();
    // End of OTA check section
    delay(750);       // delay checking loop by 500ms
  }
  
  Serial.println("OTA Setup Completed");
#ifdef M5CORE
    BackGroundColor = TFT_BLUE;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,20);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(WHITE, BackGroundColor);
    M5.Lcd.println("OTA System Ready");
    M5.Lcd.println("OTA Setup Completed");
#endif
#ifdef M5StickC
    BackGroundColor = TFT_GREEN;
    M5.Lcd.fillScreen(BackGroundColor);
    M5.Lcd.setCursor(0,4);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_BLACK, BackGroundColor);
    M5.Lcd.println("OTA Ready");
#endif

  delay(STARTUPMSGDELAY);    // delay so that no issues with ezM5 library occurs
  
  M5.Lcd.fillScreen(TFT_BLACK);   //Clear the screen

}
/** END - OAT Code **/

