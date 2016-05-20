from rflib import *
import sys

def init(device):
    d.setMdmModulation(MOD_ASK_OOK)
    d.setFreq(433000000)
    d.setMdmSyncMode(2)
    d.setMdmSyncWord(0xEEEE)
    d.setMdmDRate(115200)
    d.makePktFLEN(5)
    d.setMdmNumPreamble(16)
    d.setPktPQT(2)


    d.printRadioConfig()
    #d.RFlisten()
