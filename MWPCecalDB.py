#!/usr/bin/python                                                               

import sys
import os
import numpy
#sys.path.append(os.environ["G4BINDIR"]+"../../Scripts")
#from ucnacore.EncalDB import *
from EncalDB import *

class WirechamberDBfill:

    def __init__(self, anORcath="cathode"):
        self.anORcath=anORcath
        self.priority=None
        self.charge_meas="anode"
        if self.anORcath=="cathode":
            self.charge_meas="ccloud"

        self.CalDir=os.environ["UCNA_CAL_DIR"]
        
    def SetPriority(self, run, side):
        anodePreferredE=[18270,18275,18606,18916,19148,20679, 20803,22199] #list of runs where anode is preferred over ccloud
        anodePreferredW=[20991]
        if self.charge_meas=="anode":
            self.priority=0
            if side=="East":
                for i in range(0,len(anodePreferredE),1):
                    if run==anodePreferredE[i]:
                        self.priority=10
            elif side=="West":
                for i in range(0,len(anodePreferredW),1):
                    if run==anodePreferredW[i]:
                        self.priority=10

        if self.charge_meas=="ccloud":
            self.priority=10
            if side=="East":
                for i in range(0,len(anodePreferredE),1):
                    if run==anodePreferredE[i]:
                        self.priority=0
            elif side=="West":
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
            self.gainE[int(nums[0])]=float(nums[1])
            self.gainW[int(nums[0])]=float(nums[2])
        gainFile.close()
        for run in self.runs:
            self.UploadGainToDB(conn, run, "East", self.gainE[run])
            self.UploadGainToDB(conn, run, "West", self.gainW[run])
            
    #Function to upload gain to database for a single run on a single side
    def UploadGainToDB(self, conn, run, side, gain, priority=None):
        pmid=self.GetPosmapID(run, self.charge_meas)
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

    def GetPosmapID(self, run, c_meas):
        XeRunBegin = [17570, 18081, 18390, 18712, 19873, 21596, 21966, 22961]
        XeRunEnd = [17610, 18090, 18413, 18744, 19898, 21605, 22003, 22979]
        pmapIDcc = [243,244,245,246,247,248,249,250]
        pmapIDan = [251,252,253,254,255,256,257,258] 
        it=0
        pindex=None
        if run>=16983 and run<=18055:
            pindex=0
        elif run>=18081 and run<=18386:
            pindex=1
        elif run>=18390 and run<=18683:
            pindex=2
        elif run>=18712 and run<=19239:
            pindex=3
        elif run>=19347 and run<=20000:
            pindex=4
        elif run>=20121 and run<=21605:
            pindex=5
        elif run>=21607 and run<=22238:
            pindex=6
        elif run>=22294 and run<=23173:
            pindex=7

        if c_meas=="anode":
            return pmapIDan[pindex]
        else:
            return pmapIDcc[pindex]

        

    def LoadMap(self, conn, nrings, runmin, runmax, emin, emax ):
        radius=50
        descrip="%s %i-%i"%(self.charge_meas, runmin, runmax)
        cmd="INSERT INTO posmap_set (descrip, n_rings, radius) "
        cmd+="VALUES ('%s',%i,%i)"%(descrip,nrings,radius)
        print cmd
        conn.execute(cmd)
        conn.execute("SELECT LAST_INSERT_ID()")
        pmid=int(conn.fetchone()[0])
        #pmid=10000

        
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

            fin=self.CalDir+"/posMaps/%irings_%i-%ikeV_posMap_%i-%i_%s_%s.dat"%(nrings, emin, emax, runmin, runmax,self.anORcath, side) 
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
                    r = (rmax+rmin)/2.
                    phi = (phimax+phimin)/2.
                    x = r*numpy.cos(phi)
                    y = r*numpy.sin(phi)
                    x0.append(x)
                    y0.append(y)
                    mapval.append(m)

            for i in range(0, len(pixelID), 1):
                #print pixelID[i], x0[i], y0[i], mapval[i]
                
                cmd2="INSERT INTO posmap_points (posmap_set_id,side,quadrant,pixel_id,signal,center_x,center_y,norm) "
                cmd2+="VALUES (%i,'%s',0,%i,%f,%f,%f,1)"%(pmid,Side,pixelID[i],mapval[i],x0[i],y0[i])
                print cmd2
                conn.execute(cmd2)

if __name__ == "__main__":

    if 1:
        conn=open_connection()
        db = WirechamberDBfill(anORcath="anode")
        db.LoadGainFromFile(conn, "2011-2012","Beta")

    XeRunBegin = [17570, 18081, 18390, 18712, 19873, 21596, 21966, 22961]
    XeRunEnd = [17610, 18090, 18413, 18744, 19898, 21605, 22003, 22979]
    nrings=6

    if 0:
        for i in range(0, len(XeRunBegin),1):
            rmin=XeRunBegin[i]
            rmax=XeRunEnd[i]
            emin=400
            emax=600

            conn = open_connection()
            db=WirechamberDBfill(anORcath="cathode")
            db.LoadMap(conn,nrings,rmin,rmax,emin,emax)

            print "Finished Loading runs",rmin,"-",rmax

    if 0:
        for i in range(0,len(XeRunBegin),1):
            db=WirechamberDBfill()
            pid=db.GetPosmapID(XeRunEnd[i],"anode")
            print pid

