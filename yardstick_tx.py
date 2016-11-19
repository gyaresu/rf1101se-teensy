from rflib import *
import sys

def init(d):
    d.setMdmModulation(MOD_ASK_OOK)
    d.setFreq(433000000)
    d.setMdmSyncMode(2)
    d.setMdmSyncWord(0xEEEE)
    d.setMdmDRate(1394)
    #d.makePktFLEN(10)
    d.makePktVLEN(61)
    d.setRFRegister(PKTCTRL1, 0x01) # set packet filtering but no status
    #d.setRFRegister(PKTCTRL1, 0x06) # set packet filtering
    # Packet length from TX is payload minus length byte
    d.setPktAddr(0xdb)
    d.setMdmNumPreamble(2)
    #d.setPktPQT(3)
    #d.setMaxPower()


    #d.printRadioConfig()
    d.RFlisten()

d = RfCat()
init(d)

