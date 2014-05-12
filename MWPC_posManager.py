#!/usr/bin/python
import os
import time
from math import *
from optparse import OptionParser

numRings = 6 
energyBinWidth = 800
nBins = 800/energyBinWidth
run = [18391,18409]
#run = [18081,18090,18391,18409,18712,18744,19873,19898,21596,21605,21966,22003,22961,22979]
Side = ["east", "west"]
anORcath = "anode"

elow = 0
ehigh = energyBinWidth
for i in range(0,nBins,1):
    
    for side in Side:

        hold = 0

        for i in range(0,len(run)/2,1):

        # syntax: program fileBegin fileEnd gridSize side directory
            os.system("./MWPC_pos_landau.exe %i %i %i %s %i %i %s" %(run[hold],run[hold+1],numRings,side,elow,ehigh, anORcath))

            print "Done with runs %i-%i" %(run[hold], run[hold+1])
            hold = hold + 2
    
            print 
            print "Done with %s side" %side
    
    print
    print "Done with energy bin %i-%i" %(elow, ehigh)
    elow+=energyBinWidth
    ehigh+=energyBinWidth
