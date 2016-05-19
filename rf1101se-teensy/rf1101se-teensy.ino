#include "EEPROM.h"
//#include "rf1101.h"

#include "cc1101.h"
//#include "i2c_t3.h"

CC1101 cc1101;

SPI spi;

//uint8_t sckPIN = 14;
uint8_t counter = 0;
uint8_t b;

//uint8_t ledPin = 13;
//byte syncWord = 0x55;
//byte syncWord[] = {0x55, 0x55};

// SETUP HERE
void setup()
{
  // Set alternate SCK pin https://www.pjrc.com/teensy/td_libs_SPI.html#altpins
  // Not working - probably to do with local library spi.h
  //SPI.setSCK(sckPIN);
  //spi.init(10,11,12,14);
  //spi.init();
  
  //pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // SyncWord
  uint8_t syncH = 0xEE;
  uint8_t syncL = 0xEE;

  // Initialize the CC Chip
  cc1101.init();
  
  cc1101.setSyncWord(syncH, syncL, false);
  //cc1101.setSyncWord(syncWord, false);
  cc1101.setCarrierFreq(CFREQ_433);
  cc1101.disableAddressCheck();
  cc1101.setTxPowerAmp(PA_LowPower);
  
  
  // MDMCFG4 - channel bandwidth and exponent for calculating data rate
  cc1101.writeReg(0x10, 0xE5);

  // MDMCFG3 - Data Rate
  //DRATE = 1000000.0 * MHZ * (256+drate_m) * powf(2,drate_e) / powf(2,28);
  cc1101.writeReg(0x11, 0xC3);

  // MDMCFG2 - Modem configuration
  cc1101.writeReg(0x12, 0x30);  

  cc1101.writeReg(0x13, 0x22);

  cc1101.writeReg(0x14, 0xF8);
  
  delay(1000);

  Serial.println("Radio initialising\n");
  Serial.print("CC1101_PARTNUM ");
  Serial.println(cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_VERSION ");
  Serial.println(cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_MARCSTATE ");
  Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

  Serial.print("MDMCFG4: Channel BW ");
  Serial.println(cc1101.readReg(0x10, CC1101_CONFIG_REGISTER));
  Serial.print("MDMCFG3: Data Rate (Baud) ");
  Serial.println(cc1101.readReg(0x11, CC1101_CONFIG_REGISTER));
  Serial.print("MDMCFG2: Modulation / Manchester / Sync Mode ");
  Serial.println(cc1101.readReg(0x12, CC1101_CONFIG_REGISTER));
  
  Serial.println("device initialized");
}

// SEND DATA TO RF1101SE
void send_data() {

  Serial.println("sending data");
  //cc1101.flushTxFifo();

  CCPACKET data;
  //byte blinkCount = counter++;

  data.length = 5;

  data.data[0] = 5;
  data.data[1] = 4;
  data.data[2] = 3;
  data.data[3] = 2;
  data.data[4] = 1;

  if (cc1101.sendData(data)) {
    Serial.println(" sent ok :)");
  } else {
    Serial.println("sent failed :(");
  }
}

void loop()
{
  send_data();
  delay(2000);
}
