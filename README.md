# Blood-Oxygen-Monitor
M5 Stick C blood pulse and oximeter monitor using MAX30105 sensor

/**********************************************************************************************************************
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
* Version 0.4 - add credentials.h file, add wifi, OTA and time support
* Version 0.5 - display shows period from last valid reading, updated formatting
**********************************************************************************************************************/