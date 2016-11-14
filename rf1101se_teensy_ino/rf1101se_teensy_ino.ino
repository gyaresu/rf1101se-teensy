#include "EEPROM.h"
#include "cc1101.h"

/*
  DON'T PANIC

  Seriously... All this stuff is annoyingly difficult to learn because the data sheets are
  close enough to incomprehensible and the examples are obtuse and...

  Working with microcontrollers doesn't need to be so difficult. It's their fault, not yours.

  Better writers, better social advocates and better engagement would make this a lot more fun ffs...

  IT'S NOT YOU, that's the problem...

  Good luck

  @gareth__
*/

CC1101 cc1101;

SPI spi;

// Texas Instruments CC1101 reference
// http://www.ti.com/lit/ds/symlink/cc1101.pdf (pdf)

/* 
 *  Pins require 7 wires.
 *  MOSI: 11
 *  MISO: 12
 *  CSN: 10
 *  SCK: 13
 *  GDO0: 2
 *  GND 
 *  VCC
 */

// SETUP HERE
void setup()
{
  // Setting alternate SCK pin to 14 does not work but pin 13 (LED) works fine.
  // Set alternate SCK pin https://www.pjrc.com/teensy/td_libs_SPI.html#altpins
  // **I am using pin 13 in this sketch**
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
  
  // PKTLEN - (includes length byte)
  cc1101.writeReg(0x06, 0x10);
  
  // PKTCTRL1 - Packet Automation Control
  cc1101.writeReg(0x07, 0x06); // Enable packet filtering and status bytes
  
  // PKTCTRL0 - Packet Automation Control
  cc1101.writeReg(0x08, 0x00); // 0x00 for no CRC check and fixed packet length
  
  // ADDR - An arbitrary address used to filter out messages intended for this radio
  cc1101.writeReg(0x09, 0xbd);
  
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


  // FREND0 - Select PATABLE index to use when sending a '1'
  cc1101.writeReg(0x22, 0x11);

  // Define what a strong '1' signal is in the second byte of the PATABLE
  set_patable();

  delay(1000);

  Serial.println("Radio initialising\n");
  Serial.print("CC1101_PARTNUM ");
  Serial.println(cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_VERSION ");
  Serial.println(cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
  Serial.print("CC1101_MARCSTATE ");
  Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);
  Serial.print("PKTCTRL1: PQT / RSSI, LQI & CRC OK - ");
  Serial.println(cc1101.readReg(0x07, CC1101_CONFIG_REGISTER));
  Serial.print("PKTCTRL0: Data whitening / Packet format / CRC Check / Packet length - ");
  Serial.println(cc1101.readReg(0x08, CC1101_CONFIG_REGISTER));
  Serial.print("MDMCFG4: Channel BW - ");
  Serial.println(cc1101.readReg(0x10, CC1101_CONFIG_REGISTER));
  Serial.print("MDMCFG3: Data Rate (Baud) - ");
  Serial.println(cc1101.readReg(0x11, CC1101_CONFIG_REGISTER));
  Serial.print("MDMCFG2: Modulation / Manchester / Sync Mode - ");
  Serial.println(cc1101.readReg(0x12, CC1101_CONFIG_REGISTER));
  Serial.println("device initialized");
}

// SEND DATA TO cc1101
void send_data() {

  Serial.println("\n");
  Serial.println("sending data");

  CCPACKET data;

  byte thing[10] = {0x09, 0xdb, 0x62, 0x65, 0x65, 0x66, 0x64, 0x65, 0x61, 0x64};
  
  memcpy(data.data, thing, sizeof(data.data));
  
  data.length = sizeof(thing);
  Serial.println(data.length);

  if (cc1101.sendData(data)) {
    Serial.println("Packet sent ok :)");
    Serial.print("Packet length is: ");
    Serial.print(data.length);
    Serial.println(" bytes.");
    //Serial.print(data.data[0], HEX);
    for (int i = 0; i < data.length; i++) {
      Serial.print(data.data[i], HEX);
      Serial.print(" ");
    }
  } else {
    Serial.println("sent failed :(");
  }
}

void loop()
{
  send_data();
  delay(2000);
}


// Set the PATABLE[1] with signal strength of a binary '1'. In OOK mode it will use PATABLE[0] for '0' strength.
// Table 30 on page 50 shows the optimal power setting per frequency. 0xC0 is highest, 0x60 is default.
void set_patable()
{
  byte PA_TABLE[] = {0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  cc1101.writeBurstReg(0x3E, PA_TABLE, 8);
}


