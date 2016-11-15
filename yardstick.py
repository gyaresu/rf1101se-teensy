from rflib import *
import sys

def init(d):
    d.setMdmModulation(MOD_ASK_OOK)
    d.setFreq(433000000)
    d.setMdmSyncMode(0x02)
    d.setMdmSyncWord(0xEEEE)
    d.setMdmDRate(1394)
    d.makePktVLEN(20)
    d.setRFRegister(PKTCTRL1, 0x06) # set packet filtering
    # Packet length from TX is payload minus length byte
    d.setPktAddr(0xdb)
    d.setMdmNumPreamble(0)
    #d.setPktPQT(3)
    d.setMaxPower()


    #d.printRadioConfig()
    d.RFlisten()

d = RfCat()
init(d)

