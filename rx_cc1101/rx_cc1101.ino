#include "EEPROM.h"
#include "cc1101.h"


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
  Serial.begin(9600);

  // SyncWord
  uint8_t syncH = 0xEE; // 11101110 twice gives you a sync word of 1110111011101110
  uint8_t syncL = 0xEE;

  // Initialize the CC Chip
  cc1101.init();

  cc1101.setSyncWord(syncH, syncL, false);
  cc1101.setCarrierFreq(CFREQ_433);

  // PKTLEN - Packet Length
  cc1101.writeReg(0x06, 0x08); // 8 bytes of payload(?)

  // Address checking will enable us to speak only to specific devices
  // PKTCTRL1 (page 67) enables the 'address check'.
  // The library has shorthands instead of writing directly to the register
  // cc1101.disableAddressCheck();
  // cc1101.enableAddressCheck();
  
  // PKTCTRL1 - Packet Automation Control
  cc1101.writeReg(0x07, 0x04); // Disabled is 0x04, enabled with broadcast (0x00) is 0x06.

  // PKTCTRL0 - Packet Automation Control
  cc1101.writeReg(0x08, 0x07);

  // ADDR - Device Address
  cc1101.writeReg(0x09, 0xdb); // 0b11011011 
  cc1101.writeReg(0x09, 0x00);

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

  // Set RX only
  cc1101.setRxState(); // unnecessary?
  
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

  // You can set an interupt or poll on GDO0, pin 2 (page 35)
  attachInterrupt(digitalPinToInterrupt(2), isr, FALLING);
}



void isr()
{
  Serial.println("Interrupt triggered.");
    
    CCPACKET p;
    
    if (cc1101.receiveData(&p) > 0) {
      if (p.length > 0) {
        Serial.print("Packet length: ");
        Serial.println(p.length);
        
        for (int i = 0; i < p.length; i++) {
          Serial.print(p.data[i], HEX);
        }
      }
    }
}

void loop()
{
  Serial.println("Waiting for packets to interrupt...");
  delay(2000);
}



