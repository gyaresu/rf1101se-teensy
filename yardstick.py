from rflib import *
import sys

# Accessing RF registers
#hex(ord(d.peek(PKTCTRL1)))

def init(d):
    d.setMdmModulation(MOD_ASK_OOK)
    d.setFreq(433000000)
    d.setMdmSyncMode(2)
    d.setMdmSyncWord(0xEEEE)
    d.setMdmDRate(1500)
    #d.makePktFLEN(10)
    d.makePktVLEN(61)
    d.setRFRegister(PKTCTRL1, 0x01) # set packet filtering but no status
    d.setRFRegister(PKTCTRL0, 0x05) # set CRC & variable length
    #d.setRFRegister(PKTCTRL1, 0x06) # set packet filtering
    d.setPktAddr(0xdb)
    d.setMdmNumPreamble(2)
    #d.setPktPQT(3)
    #d.setMaxPower()


    #d.printRadioConfig()
    #d.RFlisten()

d = RfCat()
init(d)

# d.testTX('\xdb\x48\x69\x20\x50\x75\x6e\x6b')

