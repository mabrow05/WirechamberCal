#!/usr/bin/python
import os
from math import *

def fileExistsAndNotEmpty(fpath):
    return True if os.path.isfile(fpath) and os.path.getsize(fpath) > 0 else False


class WirechamberCalManager:

    def __init__(self, runNumber=18509, anORcath="anode", EnBin=100, Emin=400, Emax=600, nrings=6):
        self.runNumber = runNumber
        self.anORcath = anORcath
        self.EnBin = EnBin
        self.Emin = Emin
        self.Emax = Emax
        self.nbins = 800/self.EnBin
        self.nrings = nrings
        self.UCNACalDir = os.environ["UCNA_CAL_DIR"]
        self.Side = ["east", "west"]

    def setDirs(self):
        self.SimDir = self.UCNACalDir+"/"+"Sims"
        self.XeDir = self.UCNACalDir+"/"+"Xe_landauFitted"
        self.posMapDir = self.UCNACalDir+"/"+"posMaps"
        self.CorrDataDir = self.UCNACalDir+"/"+"correctedData"
	self.LogDir = self.UCNACalDir+"/"+"logs"
	self.DataDir="/extern/mabrow05/ucna/replay/2011/02252013/hists"
        self.GainDir = self.UCNACalDir+"/"+"gainFactors"
        self.betaRunMPVDir = self.UCNACalDir+"/"+"betaRunsMPV"
        if not os.path.exists(self.SimDir):
            os.system("mkdir %s"%(self.SimDir))
        if not os.path.exists(self.XeDir):
            os.system("mkdir %s"%(self.XeDir))
        if not os.path.exists(self.posMapDir):
            os.system("mkdir %s"%(self.posMapDir))
        if not os.path.exists(self.CorrDataDir):
            os.system("mkdir %s"%(self.CorrDataDir))
	if not os.path.exists(self.LogDir):
		os.system("mkdir %s"%(self.LogDir))
        if not os.path.exists(self.GainDir):
		os.system("mkdir %s"%(self.GainDir))
        if not os.path.exists(self.betaRunMPVDir):
            os.system("mkdir %s"%(self.betaRunMPVDir))


    def cleanDirsForNewRun(self, Xe=False, posMap=False, CalData=False, Sim=False, XeAll=False, CalDataAll=False):
        if Sim:
            os.system("cd %s ; rm *%i-%i*"%(self.SimDir, self.Emin, self.Emax))

        if Xe:
            if XeAll:
                os.system("cd %s ; rm *%irings%i-%i*"%(self.XeDir, self.nrings, self.Emin, self.Emax))
            else:
                os.system("cd %s ; rm *%s_%irings%i-%i*"%(self.XeDir, self.anORcath, self.nrings, self.Emin, self.Emax))   

        if posMap: #generated fast so remove them all every time
            os.system("cd %s ; rm *%i-%i*"%(self.posMapDir, self.Emin, self.Emax))
   
        if CalData:
            if CalDataAll:
                os.system("cd %s ; rm *%irings_%i-%ikev_%s*"%(self.XeDir, self.nrings, self.Emin, self.Emax, self.anORcath))
            else:
                os.system("cd %s ; rm *%i_%irings_%i-%ikev_%s*"%(self.XeDir, self.runNumber, self.nrings, self.Emin, self.Emax, self.anORcath))


    def runSimData(self):
        #first check that the simulated data for EnBin doesn't exist
        #if it does, then exit. If it doesn't, then run the simdata code
        if self.runNumber >= 20121:
            self.geometry = "2012-2013"
        else:
            self.geometry = "2011-2012"

        if not fileExistsAndNotEmpty(self.SimDir+"/"+"Beta_%i-%ikev_%s_east.dat"%(self.Emin, self.Emax, self.geometry)):
            	os.system("./simData.exe %i %i %i %s" %(10,self.Emin, self.Emax, self.geometry))
		print "Finished Simulated data....\n"	
	else:
		print "Simulated data already existed for this Energy Bin!\n"

        if not fileExistsAndNotEmpty(self.SimDir+"/Beta_%i-%ikev_%s_east.dat"%(self.Emin, self.Emax, self.geometry)):
            print "PROBLEM WITH OBTAINING SIMULATED DATA"
            exit(0)
	


    def findXeRun(self):#done
        #make a table of all the Xe runs, check where runNumber falls
        #in comparison, and choose the appropriate Xe run range   
        # note: changed 17562 -> 17684
	XeRunBegin = [16983, 17684, 18081, 18390, 18712, 19873, 21596, 21966, 22961]    
        self.XeRuns = {16983:17078, 17684:17734, 18081:18090, 18390:18413, 
                       18712:18744, 19873:19898, 21596:21605, 21966:22003, 22961:22979}

        self.XeRunLow = 18390 #default values chosen but just as a holder
        self.XeRunHigh = 18413
        
        if self.runNumber>=16983 and self.runNumber<=17358:
            self.XeRunLow = XeRunBegin[0]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=17359 and self.runNumber<=18055:
            self.XeRunLow = XeRunBegin[1]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=18081 and self.runNumber<=18386:
            self.XeRunLow = XeRunBegin[2]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=18390 and self.runNumber<=18683:
            self.XeRunLow = XeRunBegin[3]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=18712 and self.runNumber<=19239:
            self.XeRunLow = XeRunBegin[4]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=19347 and self.runNumber<=20000:
            self.XeRunLow = XeRunBegin[5]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=20000 and self.runNumber<=21605:
            self.XeRunLow = XeRunBegin[6]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=21607 and self.runNumber<=22238:
            self.XeRunLow = XeRunBegin[7]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        elif self.runNumber>=22294 and self.runNumber<=23173:
            self.XeRunLow = XeRunBegin[8]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

        else:
            it = 0
            while (it < len(XeRunBegin) and self.runNumber > XeRunBegin[it]):
                it+=1
            self.XeRunLow = XeRunBegin[it-1]
            self.XeRunHigh = self.XeRuns[self.XeRunLow]

	
	print "Xenon Run range: %i - %i\n"%(self.XeRunLow,self.XeRunHigh)


    def runXeData(self):#done
        #check to see if the Xe range data from findXeRun has been run for that 
        #energy bin
        
        for side in self.Side:
            if not fileExistsAndNotEmpty(self.XeDir+"/%i-%i_%s_%irings%i-%ikeV_%s.dat"%(self.XeRunLow,self.XeRunHigh,
                                                                             self.anORcath,self.nrings,self.Emin,self.Emax,side)):
                
                os.system("./MWPC_pos_landau.exe %i %i %i %s %i %i %s"%(self.XeRunLow,self.XeRunHigh,
                                                                            self.nrings,side,self.Emin,self.Emax,self.anORcath))
                    

    def runPosMap(self): #done
	print "Running Position Map.....\n"
	
        if not fileExistsAndNotEmpty(self.posMapDir+"/%irings_%i-%ikeV_posMap_%i-%i_%s_%s.dat"%(self.nrings,self.Emin,self.Emax,self.XeRunLow,
                                                                                      self.XeRunHigh,self.anORcath,"east")):
            
            os.system("./posMap.exe %i %i %i %i %i %s"%(self.XeRunLow,self.XeRunHigh,
                                                            self.nrings,self.Emin,self.Emax,self.anORcath))
	    print "Position Map is finished!\n"
	
	else:
            print "Position Map already Exists\n"


    def calcGainFactors(self, geometry="2011-2012", loadORcalc="calc"): 
        self.loadORcalc=loadORcalc
        if os.path.isfile(self.GainDir+"/gain_%s_%s.txt"%(self.anORcath, geometry)):
            os.system("rm %s"%(self.GainDir+"/gain_%s_%s.txt"%(self.anORcath, geometry)))
            os.system("rm %s"%(self.betaRunMPVDir+"/MPV_%s_%s.txt"%(self.anORcath, geometry)))
        octetFile = open("BetaRunsInOctet_%s.txt"%(geometry), "r")
        for line in octetFile:
            runFile = open("runHold.txt", "w")
            runs=line.split()
            for i in range(0, len(runs), 1):
                if os.path.isfile(self.DataDir+"/spec_%s.root"%(runs[i])):
                    runFile.write(runs[i]+'\n')
                else:
                    print "File %s doesn't exist..."%(runs[i])
            runFile.close()
            #if int(runs[0])>18432 and int(runs[0])<18682:
            os.system("./gainFactors.exe %i %s %s %i %i %s"%(self.nrings, self.anORcath,'runHold.txt', self.Emin, self.Emax, self.loadORcalc))
            
        octetFile.close()

                                          

    def correctData(self):
	print "correcting Data....\n"
	logFile = self.LogDir+"/%s_%s_Ebin%i-%i_log.txt"%(self.runNumber, self.anORcath, self.Emin, self.Emax)

	if os.path.isfile(self.DataDir+"/spec_%i.root"%(self.runNumber)):
        	#if not fileExistsAndNotEmpty(self.CorrDataDir+"/RunNum_%i_%irings_%i-%i_%s.root"%(self.runNumber, self.nrings, self.Emin, self.Emax, self.anORcath)):
			#os.system("./correctData.exe %i %i %i %i %i %s %i > %s "%(self.XeRunLow,self.XeRunHigh,self.nrings,
			#					 	self.Emin, self.Emax, self.anORcath,self.runNumber,logFile))
                        os.system("./correctData.exe %i %i %i %i %i %s %i "%(self.XeRunLow,self.XeRunHigh,self.nrings,
								 	self.Emin, self.Emax, self.anORcath,self.runNumber))
			print "Correction for run %i is finished!"%(self.runNumber)

		#else:
		#	print "This run was already completed using %s"%(self.anORcath)
	else:
		print "Run %i doesn't exist. Moving on....\n"%(self.runNumber)

#############################################################################################################

if __name__ == "__main__":


    #/////////////////////////////////// For CLEANUP PURPOSES only ///////////////////////////////////////////#
    if 1:

        if 1: 
            t = ["anode","cathode"]
            for poo in t:
                #os.system("rm *2011-2012_%s.dat"%poo)
                os.system("rm *2012-2013_%s.dat"%poo)

        #new calibrated data
        if 0:
            t = ["anode","cathode"]
            run = 18333;
            for poo in t:
                clean = WirechamberCalManager(anORcath=t, runNumber=run)
                clean.CleanDirsForNewRun(CalData=True, CalDataAll=False)    
        
        # To clean both Xe fitted data and posMaps for new Maps
        if 0:
            clean = WirechamberCalManager()
            clean.CleanDirsForNewRun(Xe=True, XeAll=True, posMap=True)

        #Cleaning either cathode or anode data for Xe fitted data and all posmaps
        if 0:
            t = "anode"             
            clean = WirechamberCalManager(anORcath=t)
            clean.CleanDirsForNewRun(Xe=True)
#//////////////////////////////////////////////////////////////////////////////////////////////////////

        

    if 1:
        #print '1' 2011-2012 [17080, 17735, 18091, 18433, 18745, 19899] 2012-2013 [21000, 22000, 23000]
        geo = "2012-2013"
        if geo[3]=='1':
            runs = [17080, 17735, 18091, 18433, 18745, 19899]
        else:
            runs = [21000, 22000, 23000]
        print runs
        for run in runs:
            cal = WirechamberCalManager(runNumber=run, anORcath = "cathode")
            cal.setDirs()
            cal.runSimData()
            cal.findXeRun()
            cal.runXeData()
            cal.runPosMap()

        gain = WirechamberCalManager(anORcath = "cathode")
        gain.setDirs()
        gain.calcGainFactors(geometry=geo, loadORcalc="calc")

    if 1:
        #print '1'
        geo = "2012-2013"
        if geo[3]=='1':
            runs = [17080, 17735, 18091, 18433, 18745, 19899]
        else:
            runs = [21000, 22000, 23000]
        for run in runs:
            cal = WirechamberCalManager(runNumber=run, anORcath = "anode")
            cal.setDirs()
            cal.runSimData()
            cal.findXeRun()
            cal.runXeData()
            cal.runPosMap()

        gain = WirechamberCalManager(anORcath = "anode")
        gain.setDirs()
        gain.calcGainFactors(geometry=geo, loadORcalc="calc")


    #To calibrate data using cathode
    if 1:
        #print '1'
        geo = "2012-2013"
        gainFile = open(os.environ["UCNA_CAL_DIR"]+"/gainFactors/gain_cathode_%s.txt"%(geo), "r")
        for line in gainFile:
            nums=line.split()
            print nums[0]
            
            cal = WirechamberCalManager(runNumber=int(nums[0]), anORcath = "cathode")
            cal.setDirs()
            cal.runSimData()
            cal.findXeRun()
            cal.runXeData()
            cal.runPosMap()
            cal.correctData()
        gainFile.close()

    #to calibrate data using anode
    if 1:
        #print '1'
        geo = "2012-2013"
        gainFile = open(os.environ["UCNA_CAL_DIR"]+"/gainFactors/gain_anode_%s.txt"%(geo), "r")
        for line in gainFile:
            nums=line.split()
            print nums[0]
            
            cal = WirechamberCalManager(runNumber=int(nums[0]), anORcath = "anode")
            cal.setDirs()
            cal.runSimData()
            cal.findXeRun()
            cal.runXeData()
            cal.runPosMap()
            cal.correctData()
        gainFile.close()





