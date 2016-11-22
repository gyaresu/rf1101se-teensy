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
    Pins require 7 wires.
    MOSI: 11
    MISO: 12
    CSN: 10
    SCK: 13
    GDO0: 2
    GND
    VCC
*/

// Global variable to trigger GDO0 activity
volatile bool trigger = false;

// Packet count
uint32_t rxcount;
uint32_t txcount;

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

  // IOCFG0 - GDOx Signal Selection (x = 0, 1, or 2)
  cc1101.writeReg(0x02, 0x06);

  // FIFOTHR - RX FIFO and TX FIFO Thresholds
  //cc1101.writeReg(0x03, 0x02);

  // PKTLEN - (includes length byte)
  // If PKTCTRL0.LENGTH_CONFIG is fixed then this is packet length
  // Variable Mode packet length defined by first byte after sync word
  cc1101.writeReg(0x06, 0x3d); // Packet length of 20 bytes

  // PKTCTRL1 - Packet Automation Control
  // 0x66 Setting PQT to '3', enabling status, and checking address plus broadcast
  cc1101.writeReg(0x07, 0x01); // Disabled is 0x04, enabled with broadcast (0x00) is 0x06.

  // PKTCTRL0 - Packet Automation Control
  cc1101.writeReg(0x08, 0x05); // CRC and variable packet length

  // ADDR - An arbitrary address used to filter out messages intended for this radio
  cc1101.writeReg(0x09, 0xbd); // Effectively the Hostname

  // CHANNR - Channel Number
  cc1101.writeReg(0x0A, 0x00); // 0x00 is default

  // FSCTRL1 - Frequency Synthesizer Control
  // *note* rf1101se uses a 26MHz crystal
  cc1101.writeReg(0x0B, 0x0c);

  // FSCTRL0 - Frequency Synthesizer Control
  cc1101.writeReg(0x0C, 0x00); // 0x00 is default

  // FREQ2/1/0
  cc1101.writeReg(0x0D, 0x10);
  cc1101.writeReg(0x0E, 0xA7);
  cc1101.writeReg(0x0F, 0x62);

  // ------ Data rate ------
  // MDMCFG4 - channel bandwidth and exponent for calculating data rate
  cc1101.writeReg(0x10, 0xC6); // from 0xC5

  // MDMCFG3 - Data Rate
  // DRATE = 1000000.0 * MHZ * (256+drate_m) * powf(2,drate_e) / powf(2,28);
  cc1101.writeReg(0x11, 0x06);
  // -----------------------

  // MDMCFG2 - Modulation type (OOK/ASK) / manchester / sync mode
  cc1101.writeReg(0x12, 0x1a); // dc block, GFSK, manchester, 16/16

  // MDMCFG1 - FEC / preamble
  // 00000010 - No FEC, 2 bytes of preamble, reserved, two bit exponent of channel spacing
  cc1101.writeReg(0x13, 0x02); // 03 **

  // MDMCFG0 - Channel spacing
  cc1101.writeReg(0x14, 0x11);

  // FREND1 - Select PATABLE index to use when sending a '1'
  cc1101.writeReg(0x21, 0xb6);

  // FREND0 - Select PATABLE index to use when sending a '1'
  cc1101.writeReg(0x22, 0x11);

  // FREQ2/1/0
  /*
   * cc1101.writeReg(0x0D, 0x12);
   * cc1101.writeReg(0x0E, 0x0a);
   * cc1101.writeReg(0x0F, 0xaa);
   */
  
  // Define what a strong '1' signal is in the second byte of the PATABLE
  set_patable();

  delay(1000);
  Serial.println("Radio initialising\n");
  Serial.print("CC1101_PARTNUM");
  Serial.println(cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER & 0xF0));
  Serial.print("CC1101_VERSION - 0x");
  Serial.println(cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER), HEX);
  Serial.print("CC1101_MARCSTATE - 0x");
  Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1F, HEX);
  Serial.print("IOCFG2: GDO2 Output Pin Configuration - 0x");
  Serial.println(cc1101.readReg(0x00, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("IOCFG1: GDO1 Output Pin Configuration - 0x");
  Serial.println(cc1101.readReg(0x01, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("IOCFG0: GDO0 Output Pin Configuration - 0x");
  Serial.println(cc1101.readReg(0x02, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FIFOTHR - RX FIFO and TX FIFO Thresholds - 0x");
  Serial.println(cc1101.readReg(0x03, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("SYNC1: Sync Word High Byte - 0x");
  Serial.println(cc1101.readReg(0x04, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("SYNC0: Sync Word Low Byte - 0x");
  Serial.println(cc1101.readReg(0x05, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("PKTLEN: Fixed = Packet Length; Variable = Maximum allowed. — 0x");
  Serial.println(cc1101.readReg(0x06, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("PKTCTRL1: PQT / RSSI, LQI / Address Check / CRC OK - 0x");
  Serial.println(cc1101.readReg(0x07, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("PKTCTRL0: Data whitening / Packet format / CRC Check / Packet length - 0x");
  Serial.println(cc1101.readReg(0x08, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("ADDR - Device Address: 0x");
  Serial.println(cc1101.readReg(0x09, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FSCTRL1: 0x");
  Serial.println(cc1101.readReg(0x0B, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FSCTRL0: 0x");
  Serial.println(cc1101.readReg(0x0C, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FREQ2: 0x");
  Serial.println(cc1101.readReg(0x0D, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FREQ1: 0x");
  Serial.println(cc1101.readReg(0x0E, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FREQ0: 0x");
  Serial.println(cc1101.readReg(0x0F, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("MDMCFG4: Channel BW - 0x");
  Serial.println(cc1101.readReg(0x10, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("MDMCFG3: Data Rate (Baud) - 0x");
  Serial.println(cc1101.readReg(0x11, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("MDMCFG2: Modulation / Manchester / Sync Mode - 0x");
  Serial.println(cc1101.readReg(0x12, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("MDMCFG1: FEC / Num Preamble / Channel Spacing - 0x");
  Serial.println(cc1101.readReg(0x13, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("MDMCFG0: Mantissa of channel spacing - 0x");
  Serial.println(cc1101.readReg(0x14, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FREND1: Front end RX configuration - 0x");
  Serial.println(cc1101.readReg(0x21, CC1101_CONFIG_REGISTER), HEX);
  Serial.print("FREND0: Front end TX configuration - 0x");
  Serial.println(cc1101.readReg(0x22, CC1101_CONFIG_REGISTER), HEX);
  Serial.println("device initialized");
  // You can set an interupt or poll on GDO0, pin 2 (page 35)
  // If you get an error (digitalPinToInterrupt is not declared in this scope)
  // Then you can set the interrupt number manually with just a '0'
  //attachInterrupt(digitalPinToInterrupt(2), isr, FALLING);
}

void ReadLQI()
{
  byte lqi = 0;
  byte value = 0;
  lqi = (cc1101.readReg(CC1101_LQI, CC1101_STATUS_REGISTER));
  value = 0x3F - (lqi & 0x3F);
  Serial.print("CC1101_LQI ");
  Serial.println(value);
}

void ReadRSSI()
{
  byte rssi = 0;
  byte value = 0;

  rssi = (cc1101.readReg(CC1101_RSSI, CC1101_STATUS_REGISTER));

  if (rssi >= 128)
  {
    value = 255 - rssi;
    value /= 2;
    value += 74;
  }
  else
  {
    value = rssi / 2;
    value += 74;
  }
  Serial.print("CC1101_RSSI ");
  Serial.println(value);
}

void isr()
{
  Serial.println("Interrupt triggered.");
  trigger = true;
}

// SEND DATA TO cc1101
void send_data() {

  Serial.println("\n");

  CCPACKET data;

  //byte thing[9] = {0xdb, 0x62, 0x65, 0x65, 0x66, 0x64, 0x65, 0x61, 0x64};
  byte thing[9] = {0x09, 0xdb, 0x48, 0x69, 0x20, 0x50, 0x75, 0x6e, 0x6b};

  memcpy(data.data, thing, sizeof(data.data));

  data.length = sizeof(thing);

  if (cc1101.sendData(data)) {
    Serial.println("Packet sent ok :)");
    Serial.print("Packet length is: ");
    Serial.print(data.length);
    Serial.println(" bytes.");
    for (int i = 1; i < data.length; i++) {
      Serial.write(data.data[i]);
      Serial.print("");
    }
    Serial.println("");
    Serial.print("Packet count: ");
    Serial.println(txcount++);
  } else {
    Serial.println("sent failed :(");
  }
}

void receive_data()
{
  //Serial.println("packet received");
  // Disable wireless reception interrupt
  detachInterrupt(digitalPinToInterrupt(2));
  CCPACKET packet;

  // clear the flag
  trigger = false;

  // Yardstick One command:
  // d.testTX('\xdb\x48\x69\x20\x50\x75\x6e\x6b')

  if (cc1101.receiveData(&packet) > 0) {
    ReadRSSI();
    ReadLQI();
    Serial.print("packet: len ");
    Serial.println(packet.length);
    Serial.print(" data: ");
    for (int i = 1; i < packet.length; i++) {
      Serial.write(packet.data[i]);
      if (i == 50) {
        Serial.print(" ");
      }
    }
    Serial.println("");
    Serial.print("Packet count: ");
    Serial.println(rxcount++);
  }

  // Enable wireless reception interrupt
  attachInterrupt(digitalPinToInterrupt(2), isr, FALLING);
}

void loop()
{
  send_data();

  delay(1000);

  if (trigger) {
    receive_data();
  }
}


// Set the PATABLE[1] with signal strength of a binary '1'. In OOK mode it will use PATABLE[0] for '0' strength.
// Table 30 on page 50 shows the optimal power setting per frequency. 0xC0 is highest, 0x60 is default.
void set_patable()
{
  byte PA_TABLE[] = {0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  cc1101.writeBurstReg(0x3E, PA_TABLE, 8);
}


