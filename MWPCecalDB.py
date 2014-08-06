#!/usr/bin/python                                                               

import sys
import os
sys.path.append(os.environ("G4BINDIR")+"../../Scripts")
from ucnacore.EncalDB import *

class WirechamberDBfill(anORcath="cathode"):
    def __init__(self):
        self.anORcath=anORcath
        self.priority=None
        self.charge_meas="anode"
        if self.anORcath=="cathode":
            self.charge_meas="ccloud"

        self.CalDir=sys.environ("UCNA_CAL_DIR")
        
    def SetPriority(self, run, side):
        anodePreferredE=[18270,18275,18606,18916,19148,20679, 20803,22199] #list of runs where anode is preferred over ccloud
        anodePreferredW=[20991]
        if self.charge_meas=="anode":
            self.priority=0
            if side=="East":
                for i in range(0,len(anodePreferredE),1):
                    if run==anodePreferredE[i]:
                        self.priority=10
            else if side=="West":
                for i in range(0,len(anodePreferredW),1):
                    if run==anodePreferredW[i]:
                        self.priority=10

        if self.charge_meas="ccloud":
            self.priority=10
            if side=="East":
                for i in range(0,len(anodePreferredE),1):
                    if run==anodePreferredE[i]:
                        self.priority=0
            else if side=="West":
                for i in range(0,len(anodePreferredW),1):
                    if run==anodePreferredW[i]:
                        self.priority=0

    def LoadGainFromFile(self, conn, geometry, runType):
        self.runs=[]
        self.gainE = {}
        self.gainW = {}
        
        gainFile = open(self.CalDir+"/gainFactors/gain_%s_%s_%s.txt"%(self.anORcath,geometry,runType), 'r')
        for line in gainFile:
            nums = line.split()
            self.runs.append(int(nums[0]))
            self.gainE[int(nums[0])]=int(nums[1])
            self.gainW[int(nums[0])]=int(nums[2])
        gainFile.close()
        for run in runs:
            self.UploadGainToDB(conn, run, "East", self.gainE[run])
            self.UploadGainToDB(conn, run, "West", self.gainW[run])
            
    #Function to upload gain to database for a single run on a single side
    def UploadGainToDB(self, conn, run, side, gain, priority=None):
        pmid=0; #add in function which determines the map number, once I have added the maps...
        if priority is None:
            self.SetPriority(run, side)
        else:
            self.priority=priority

        # delete old entries for same run range
        wh = "WHERE start_run = %i AND end_run = %i AND side = '%s' AND charge_meas='%s'"%(run,run,side,self.charge_meas)
        conn.execute("SELECT count(*) FROM mwpc_ecal "+wh)
        nold = conn.fetchone()[0]
        if nold:
            print "Deleting",nold,"previous entries."
            cmd = "DELETE FROM mwpc_ecal "+wh
            print cmd
            conn.execute(cmd)

        #Upload new gain
        cmd = "INSERT INTO mwpc_ecal (start_run,end_run,priority,side,charge_meas,gain_posmap_id,gain_factor) "
        cmd += "VALUES (%i,%i,%i,'%s','%s',%i,%g)"%(run, run ,self.priority,side,self.charge_meas,pmid,gain)
        print cmd
        conn.execute(cmd)
        print "Run %i loaded to database"%(run)
        

    def LoadMap(self, conn):
        hold


if __name__ == "__main__":

	conn = open_connection()


