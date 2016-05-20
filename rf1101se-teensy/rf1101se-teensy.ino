#include "EEPROM.h"
#include "cc1101.h"

CC1101 cc1101;

SPI spi;

// Texas Instruments CC1101 reference
// http://www.ti.com/lit/ds/symlink/cc1101.pdf (pdf)

// Pins are

// MOSI: 11
// MISO: 12
// CSN: 10
// SCK: 13
// GDO0: 2

// SETUP HERE
void setup()
{
  // Setting alternate SCK pin to 14 working but pin 13 (LED) works fine.
  // Set alternate SCK pin https://www.pjrc.com/teensy/td_libs_SPI.html#altpins
  // I am using pin 13 
  // SPI.setSCK(14);
  // spi.init(10,11,12,14);
  
  //pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // SyncWord
  uint8_t syncH = 0xEE; // 11101110 twice gives you a sync word of 1110111011101110
  uint8_t syncL = 0xEE;

  // Initialize the CC Chip
  cc1101.init();
  
  cc1101.setSyncWord(syncH, syncL, false);
  cc1101.setCarrierFreq(CFREQ_433);
  cc1101.disableAddressCheck();
  cc1101.setTxPowerAmp(PA_LowPower);
  
  // Messing with direct register changes below to then look at in Inspectrum
  // https://github.com/miek/inspectrum
  
  // MDMCFG4 - channel bandwidth and exponent for calculating data rate
  cc1101.writeReg(0x10, 0xE5);

  // MDMCFG3 - Data Rate
  // DRATE = 1000000.0 * MHZ * (256+drate_m) * powf(2,drate_e) / powf(2,28);
  cc1101.writeReg(0x11, 0xC3);

  // MDMCFG2 - Modulation type (OOK/ASK) / manchester / sync mode 
  // 00110010 - DC blocking enabled, OOK/ASK, No manchester, 16/16 syncword bits detected
  cc1101.writeReg(0x12, 0x32); // was 0x30

  // MDMCFG1 - FEC / preamble
  // 00000010 - No FEC, 2 bytes of preamble, reserved, two bit exponent of channel spacing
  cc1101.writeReg(0x13, 0x02); // Changed from 0x22

  // MDMCFG0 - Channel spacing
  cc1101.writeReg(0x14, 0xF8);
  
  delay(1000);

  Serial.println("Radio initialising\n");
  Serial.print("CC1101_PARTNUM ");
  Serial.println(cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_VERSION ");
  Serial.println(cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_MARCSTATE ");
  Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

  Serial.print("PKTCTRL0: Data whitening / ");
  Serial.println(cc1101.readReg(0x08, CC1101_CONFIG_REGISTER));
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

  CCPACKET data;

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
