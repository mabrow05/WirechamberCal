#!/usr/bin/python                                                               

import sys
import os
import numpy
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
        

    def LoadMap(self, conn, nrings, rmin, rmax, emin, emax ):
        radius=50
        descrip="%s %i %i"%(charge_meas, rmin, rmax)
        cmd="INSERT INTO posmap_set (descrip, n_rings, radius) "
        cmd+="VALUES ('%s',%i,%i)"%(descrip,nrings,radius)
        print cmd
        #conn.execute(cmd)
        #conn.execute("SELECT LAST_INSERT_ID()")
        #pmid=int(conn.fetchone()[0])

        
        for side in ["east", "west"]:
            if side=="east":
                Side="East"
            else:
                Side="West"
                
            mapval=[]
            pixelID=[]
            x0=[]
            y0=[]
            rmin=None
            rmax=None
            phimin=None
            phimax=None
            Side=None

            fin="%irings_%i-%ikeV_posMap_%i-%i_%s_%s.dat"%(nrings, emin, emax, rmin, rmax,self.anORcath, side) 
            infile = open(fin, "r")
            pid=0
            for line in infile:
                vals=line.split()
                rmin=float(vals[0])
                rmax=float(vals[1])
                phimin=float(vals[2])
                phimax=float(vals[3])
                m=float(vals[4])
                pixelID.append(pid)
                pid=pid+1

                if rmin<0.001:
                    x0.append(0.)
                    y0.append(0.)
                    mapval.append(m)
                    
                else:
                    r = (rmax-rmin)/2.
                    phi = (phimax-phimin)/2.
                    x = r*numpy.cos(phi)
                    y = r*numpy.sin(phi)
                    x0.append(x)
                    y0.append(y)
                    mapval.append(m)

            for i in range(0, len(pixelID), 1):
                print pixelID[i], x0[i], y0[i], mapval[i]
                
                #cmd2="INSERT INTO posmap_points (posmap_set_id,side,quadrant,pixel_id,signal,center_x,center_y,norm) "
                #cmd2+="VALUES (%i,'%s',0,%i,%f,%f,%f,1)"%(pmid,Side,pixelID[i],mapval[i],x0[i],y0[i])
                #print cmd2
                #conn.execute(cmd2)

if __name__ == "__main__":

    nrings=6
    rmin=18081
    rmax=18090
    emin=400
    emax=600

    conn = open_connection()
    db=WirechamberDBfill()
    db.LoadMaps(conn,nrings,rmin,rmax,emin,emax)

