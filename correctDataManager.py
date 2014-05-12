#!/usr/bin/python
import os
import time
from math import *
from optparse import OptionParser

numRings = 6 
energyBinWidth = 800
nBins = 800/energyBinWidth
run = [18432,18451]#[18391,18409]
#run = [18081,18090,18391,18409,18712,18744,19873,19898,21596,21605,21966,22003,22961,22979]
Side = ["east", "west"]

elow = 0
ehigh = energyBinWidth
for i in range(0,nBins,1):
    
    for side in Side:

    #directory1 = "/tmp_mnt/home62/grads/mabrow05/UCNA/Calibrations/position_dep_MWPC/cathode_analysis/%imm_grid/%s_MWPC" %(grid_size,side)
    #directory1 = "~/UCNA/Calibrations/%imm_grid/%s_MWPC" %(grid_size,side)
    #os.system("rm -r %s"%directory1)
    #os.system("mkdir %s"%directory1)
    #os.mkdir(directory1)

        hold = 0

        for i in range(0,len(run)/2,1):

        #directory2 = directory1 + "/runs%i-%i" %(run[hold],run[hold+1])
    
        #os.system("mkdir %s" %directory2)
        #os.system("cd %s" %directory2)
        #os.mkdir(directory2)


        # syntax: program fileBegin fileEnd gridSize side directory
            os.system("./correctData.exe %i %i %i %s %i %i" %(run[hold],run[hold+1],numRings,side,elow,ehigh))

            print "Done with runs %i-%i" %(run[hold], run[hold+1])
            hold = hold + 2
    
            print 
            print "Done with %s side" %side
    
    print
    print "Done with energy bin %i-%i" %(elow, ehigh)
    elow+=energyBinWidth
    ehigh+=energyBinWidth
