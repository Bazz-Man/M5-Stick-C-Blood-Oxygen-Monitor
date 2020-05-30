/*******************************************************
* 
* Updated to work on M5StickC - B.Chubb 10th May 2020
* Checked/works with pimironi MAX30105 sensor
* ### NOTE ### INT pin on sensor connected to pin26 on M5StickC, Other pins are standard SCL/SDA I2C connection
* -------------------------------------------------------------------------
*
* Credit to Fraczkiewicz, R. https://github.com/aromring/MAX30102_by_RF, accessed 10/30/2019 for the improved algorithm
*
* Version 0.1 - Based on MAX example updated by Molecular Descriptor (aromring) user
* Version 0.2 - Removed unnecessary #ifdefs
* Version 0.3 - add LCD display
*******************************************************/

#include <M5StickC.h>

const char* BuildTime = __TIME__;
const char* BuildDate = __DATE__;
const char* Version = "V0.3";
#define SKETCH "Blood Oxygen Monitor"
#define HOSTNAME "M5StickC-1"
#define STARTUPMSGDELAY 3000           // ms to wait to display startup messages
#define SHORTVERSION false             // output just the version no on screen at startup
#define STARTUPSCREENBRIGHTNESS 200    // startup brightness
const char* M5TYPE = "M5StickC";       //Define M5 unit
int StdScreenBrightness = 100;
int BackGroundColor = BLACK;
int TextColor = WHITE;
char Build[30];
int SecondsToInvalid = 30;          // Number of seconds before we say values are invalid
int LastValidTime = 0;              // Last time a valid reading was made 




#include <algorithm_by_RF.h>        // more reliable than the MAX provided library - https://github.com/aromring/MAX30102_by_RF/blob/master/algorithm_by_RF.h
#include "max30102.h"
#include "Credentials.h"          // needed for all network and security keys
#include "BuildInfo.h"              // Displays build info
#include "M5WifiSupport.h"          // Sets up Wifi and OTA support for M5 units

//#define OUTPUTON // Uncomment for output to the Serial stream
//#define DEBUG // Uncomment for debug output to the Serial stream
//#define USE_ADALOGGER // Comment out if you don't have ADALOGGER itself but your MCU still can handle this code
//#define TEST_MAXIM_ALGORITHM // Uncomment if you want to include results returned by the original MAXIM algorithm
//#define SAVE_RAW_DATA // Uncomment if you want raw data coming out of the sensor saved to SD card. Red signal first, IR second.

#ifdef TEST_MAXIM_ALGORITHM
  #include "algorithm.h" 
#endif

const byte oxiInt = 26; // Interrupt pin connected to MAX30102 INT
uint32_t elapsedTime,timeStart;
uint32_t aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE];  //red LED sensor data
float old_n_spo2;  // Previous SPO2 value
uint8_t uch_dummy,k;

// #################
// ### Routines  ################################################################################
// #################

void M5DisplayHRSPO(String MSG , float Spo2, int32_t Hrate )
{
  
  if ( MSG == "Valid")
  {
    M5.Lcd.fillScreen( BackGroundColor );
    M5.Lcd.setTextColor(TextColor);  
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(82, 4);
    M5.Lcd.print("Spo2");
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(2, 4);
    M5.Lcd.print(Spo2);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(80, 44);
    M5.Lcd.print("BPM");
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(40, 44);
    M5.Lcd.print(Hrate);
    M5.Lcd.setCursor(2, 50);
    M5.Lcd.print("H");
  }
  else
  {
    M5.Lcd.setTextColor(BackGroundColor);  
    M5.Lcd.setTextColor(TextColor);  
    M5.Lcd.setCursor(2, 50);
    M5.Lcd.print("H");
  }
  
}


void millis_to_hours(uint32_t ms, char* hr_str)
{
  char istr[6];
  uint32_t secs,mins,hrs;
  secs=ms/1000; // time in seconds
  mins=secs/60; // time in minutes
  secs-=60*mins; // leftover seconds
  hrs=mins/60; // time in hours
  mins-=60*hrs; // leftover minutes
  itoa(hrs,hr_str,10);
  strcat(hr_str,":");
  itoa(mins,istr,10);
  strcat(hr_str,istr);
  strcat(hr_str,":");
  itoa(secs,istr,10);
  strcat(hr_str,istr);
}

// ################
// ### SETUP ###################################################################################
// ################


void setup()
{

  if (M5TYPE)
  {
    M5.begin();
  }
  if ( M5TYPE == (char*)"M5StickC")
  {
    M5.Lcd.setRotation(3);
  }

  OutputBuildInfo();
  
  SetupMutiWifi();

  SetupOTAFunctions();

  Wire.begin(32,33);
  
  pinMode(oxiInt, INPUT);  //pin D10 connects to the interrupt output pin of the MAX30102

  maxim_max30102_reset(); //resets the MAX30102
  delay(1000);

  maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy);  //Reads/clears the interrupt status register
  maxim_max30102_init();  //initialize the MAX30102
  old_n_spo2=0.0;

  M5DisplayHRSPO("Scanning",0 , 0);
//  while(Serial.available()==0)  //wait until user presses a key
//  {
//    Serial.println(F("Press any key to start conversion"));
//    delay(1000);
//  }
//  uch_dummy=Serial.read();
#ifdef TEST_MAXIM_ALGORITHM
  Serial.println(F("Time[s]\tSpO2\tHR\tSpO2_MX\tHR_MX\tClock\tRatio\tCorr"));
#else // TEST_MAXIM_ALGORITHM
  Serial.println(F("Time[s]\tSpO2\tHR\tClock\tRatio\tCorr"));
#endif // TEST_MAXIM_ALGORITHM

  timeStart=millis();
}

// ################
// ### LOOP #####################################################################################
// ################

//Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every ST seconds
void loop()
{
  float n_spo2,ratio,correl;  //SPO2 value
  int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
  int32_t n_heart_rate; //heart rate value
  int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
  int32_t i;
  char hr_str[10];

  // Start of OTA check section
  ArduinoOTA.handle();
  yield();
  // End of OTA check section
     
  //buffer length of BUFFER_SIZE stores ST seconds of samples running at FS sps
  //read BUFFER_SIZE samples, and determine the signal range
  for(i=0;i<BUFFER_SIZE;i++)
  {
    while(digitalRead(oxiInt)==1);  //wait until the interrupt pin asserts
    maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
#ifdef DEBUG
    Serial.print(i, DEC);
    Serial.print(F("\t"));
    Serial.print(aun_red_buffer[i], DEC);
    Serial.print(F("\t"));
    Serial.print(aun_ir_buffer[i], DEC);    
    Serial.println("");
#endif // DEBUG
  }

  //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using Robert's method
  rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl); 
  elapsedTime=millis()-timeStart;
  millis_to_hours(elapsedTime,hr_str); // Time in hh:mm:ss format
  elapsedTime/=1000; // Time in seconds

#ifdef OUTPUTON
  Serial.println("--RF--");
  Serial.print(elapsedTime);
  Serial.print("\t");
  Serial.print(n_spo2);
  Serial.print("\t");
  Serial.print(n_heart_rate, DEC);
  Serial.print("\t");
  Serial.println(hr_str);
  Serial.println("------");
#endif // OUTPUTON

#ifdef TEST_MAXIM_ALGORITHM
  //calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using MAXIM's method
  float n_spo2_maxim;  //SPO2 value
  int8_t ch_spo2_valid_maxim;  //indicator to show if the SPO2 calculation is valid
  int32_t n_heart_rate_maxim; //heart rate value
  int8_t  ch_hr_valid_maxim;  //indicator to show if the heart rate calculation is valid
  maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2_maxim, &ch_spo2_valid_maxim, &n_heart_rate_maxim, &ch_hr_valid_maxim); 
#ifdef OUTPUTON
  Serial.println("--MX--");
  Serial.print(elapsedTime);
  Serial.print("\t");
  Serial.print(n_spo2_maxim);
  Serial.print("\t");
  Serial.print(n_heart_rate_maxim, DEC);
  Serial.print("\t");
  Serial.println(hr_str);
  Serial.println("------");
#endif // OUTPUTON
#endif // TEST_MAXIM_ALGORITHM

  //save samples and calculation result to SD card
#ifdef TEST_MAXIM_ALGORITHM
  if(ch_hr_valid && ch_spo2_valid || ch_hr_valid_maxim && ch_spo2_valid_maxim) {
#else   // TEST_MAXIM_ALGORITHM
  if(ch_hr_valid && ch_spo2_valid)
  { 
#endif // TEST_MAXIM_ALGORITHM
    Serial.print(elapsedTime);
    Serial.print("\t");
    Serial.print(n_spo2);
    Serial.print("\t");
    Serial.print(n_heart_rate, DEC);
    Serial.print("\t");
  #ifdef TEST_MAXIM_ALGORITHM
    Serial.print(n_spo2_maxim);
    Serial.print("\t");
    Serial.print(n_heart_rate_maxim, DEC);
    Serial.print("\t");
  #endif //TEST_MAXIM_ALGORITHM
    Serial.print(hr_str);
    Serial.print("\t");
    Serial.print(ratio);
    Serial.print("\t");
    Serial.println(correl);
    Serial.print("");
    old_n_spo2=n_spo2;

    LastValidTime = millis();         // reset last valid reading time
    M5DisplayHRSPO("Valid",n_spo2 , n_heart_rate);
  }
  else
  {
    Serial.printf("Heart Rate Valid = %i Oxygen Valid = %i\n", ch_hr_valid , ch_spo2_valid );
    M5DisplayHRSPO("INValid",n_spo2 , n_heart_rate);
  }

  

}
