#include "gainFactors.h"
#include "TCut.h"

using namespace std;

GAIN_FACTOR::GAIN_FACTOR(Char_t* t, Char_t* f, Int_t n, Int_t emin, Int_t emax, Char_t* runtype)
{
  type = t;
  nrings = n;
  Emin = emin;
  Emax = emax;
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  sprintf(runType, "%s", runtype);
  Char_t temp[200];
  sprintf(temp,"%s",f);
  Int_t run;
  ifstream infile;
  
  infile.open(temp);
  while (infile >> run) {runs.push_back(run); cout << run << endl;}
  infile.close();

  mpvWest.resize(runs.size(), 0.);
  mpvEast.resize(runs.size(), 0.);
  gWest.resize(runs.size(), 0.);
  gEast.resize(runs.size(), 0.);

  cout << endl;
 
  if (run>20120) {sprintf(geometry,"2012-2013");}
  else {sprintf(geometry,"2011-2012");}
  //sprintf(temp,"%s/gainFactors/gain_%s_%s_%s.txt", CalDir, t, geometry, runType);
  //outfile.open(temp, ios::app);
  
  //sprintf(temp,"%s/betaRunsMPV/MPV_%s_%s_%s.txt", CalDir, t, geometry, runType);
  //outfile1.open(temp, ios::app);
}

GAIN_FACTOR::~GAIN_FACTOR()
{
  // outfile.close();
  //outfile1.close();
}

void GAIN_FACTOR::calc_Q_MPV(GRID& g)
{
  Char_t temp[200];
  

  for (UInt_t ii=0; ii<runs.size();ii++)
    {
      if (*type=='a')
	{
	  east = new TH1F("east", "east", 100, 0., 2500.);
	  west = new TH1F("west", "west", 100, 0., 2500.);
	  f1 = new TF1("f1", "landau", 0.5, 1500.);
	  //f1->SetParLimits(0, 0., 1.e6);
	  f1->SetParameter(0, 3.e3);
	  //f1->SetParLimits(1, 0., 1.e4);
	  f1->SetParameter(2, 36.);

	  f2 = new TF1("f2", "landau", 0.5, 1500.);
	  //f2->SetParLimits(0, 0., 1.e6);
	  f2->SetParameter(0, 3.e3);
	  //f2->SetParLimits(1, 0., 1.e4);
	  f2->SetParameter(2, 39.);
	}
      else if (*type=='c')
	{
	  east = new TH1F("east", "east", 100, 0., 100000.);
	  west = new TH1F("west", "west", 100, 0., 100000.);
	  f1 = new TF1("f1", "landau", 30., 40000.);
	  //f1->SetParLimits(0, 0., 1.e6);
	  f1->SetParameter(0, 3.e3);
	  //f1->SetParLimits(1, 0., 1.e6);
	  f1->SetParameter(2, 1200.);
	  
	  f2 = new TF1("f2", "landau", 30., 40000.);
	  //f2->SetParLimits(0, 0., 1.e6);
	  f2->SetParameter(0, 3.e3);
	  //f2->SetParLimits(1, 0., 1.e6);
	  f2->SetParameter(2, 1900.);
	}
      //Char_t *dataDir = getenv("UCNAOUTPUTDIR");
      sprintf(temp, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root", runs[ii]);
      TFile *f = new TFile(temp, "READ");
      TTree *Tin = (TTree*)(f->Get("phys"));

      Tin->SetBranchAddress("PID", &PID);
      Tin->SetBranchAddress("Type", &Type);
      Tin->SetBranchAddress("Side", &Side);
      Tin->GetBranch("xEmpm")->GetLeaf("center")->SetAddress(&x_centerE);
      Tin->GetBranch("yEmpm")->GetLeaf("center")->SetAddress(&y_centerE);
      Tin->GetBranch("xEmpm")->GetLeaf("height")->SetAddress(&x_heightE);
      Tin->GetBranch("yEmpm")->GetLeaf("height")->SetAddress(&y_heightE);
      Tin->GetBranch("xEmpm")->GetLeaf("width")->SetAddress(&x_widthE);
      Tin->GetBranch("yEmpm")->GetLeaf("width")->SetAddress(&y_widthE);
      Tin->GetBranch("xWmpm")->GetLeaf("center")->SetAddress(&x_centerW);
      Tin->GetBranch("yWmpm")->GetLeaf("center")->SetAddress(&y_centerW);
      Tin->GetBranch("xWmpm")->GetLeaf("height")->SetAddress(&x_heightW);
      Tin->GetBranch("yWmpm")->GetLeaf("height")->SetAddress(&y_heightW);
      Tin->GetBranch("xWmpm")->GetLeaf("width")->SetAddress(&x_widthW);
      Tin->GetBranch("yWmpm")->GetLeaf("width")->SetAddress(&y_widthW);
      Tin->SetBranchAddress("AnodeE", &AnodeE);
      Tin->SetBranchAddress("AnodeW", &AnodeW);
      Tin->SetBranchAddress("EvisE", &EvisE);
      Tin->SetBranchAddress("EvisW", &EvisW);

      Int_t nevents = Tin->GetEntriesFast();

      for (Int_t mm=0; mm<nevents; mm++)
	{
	  Tin->GetEvent(mm);
      
	  if (Type==0 && PID==1)
	    {
	      Double_t r = sqrt(x_centerE*x_centerE + y_centerE*y_centerE);
	      if (r < 50. && r>1.)
		{
		  Double_t phi = 0.;
		  if (y_centerE >= 0.) {phi = acos(x_centerE/r);}
		  else if (y_centerE<0.) phi = 2*g.pi-acos(x_centerE/r);
		  
		  Int_t binNum = g.find_pos(r, phi);
		  Float_t qValue = calcQ();
		  //Energy and side cut is taken care of in FillHisto...
		  FillHisto(qValue,binNum);
		}
	      else if (r<1.)
		{
		  Int_t binNum = g.find_pos(0., 0.);
		  Float_t qValue = calcQ();
		  FillHisto(qValue,binNum);
		}
	    }
	}
      TCanvas *c1 = new TCanvas("c1");
      
      Double_t max_bin_hold = east->GetBinCenter(east->GetMaximumBin());
      Double_t max_bin = max_bin_hold<80 ? max_bin_hold : east->GetBinCenter(east->GetBin(7));
      f1->Draw();
      f1->SetParameter(1, max_bin);
      east->Fit("f1", "RM");
      mpvEast[ii] = f1->GetParameter(1);
      //f1->Delete();

      TCanvas *c2 = new TCanvas("c2");
      max_bin_hold = west->GetBinCenter(west->GetMaximumBin());
      max_bin = max_bin_hold<80 ? max_bin_hold : west->GetBinCenter(west->GetBin(7));
      f2->Draw();
      f2->SetParameter(1, max_bin);
      west->Fit("f2", "RM");
      mpvWest[ii] = f2->GetParameter(1);
      //f2->Delete();
      cout << runs[ii] << " " << mpvEast[ii] << " " << mpvWest[ii] << endl;
      
    }
}

Float_t GAIN_FACTOR::calcQ()
{
  if (*type=='a' && Side==0) return AnodeE;
  else if (*type=='a' && Side==1) return AnodeW;
  else if (*type=='c' && Side==0) return x_widthE*x_heightE+y_widthE*y_heightE;
  else if (*type=='c' && Side==1) return x_widthW*x_heightW+y_widthW*y_heightW;
}

void GAIN_FACTOR::FillHisto(Float_t val, Int_t posBin)
{
  if (runs[0]>17387 && runs[0]<17863)
    {
      if (Side==0 && EvisE>475. && EvisE<675.) east->Fill(val/etaEast[posBin]);
      else if (Side==1 && EvisW>575. && EvisW<775.) west->Fill(val/etaWest[posBin]);
    }
  else
    {
       if (Side==0 && EvisE>Emin && EvisE<Emax) east->Fill(val/etaEast[posBin]);
      else if (Side==1 && EvisW>Emin && EvisW<Emax) west->Fill(val/etaWest[posBin]);
    }
}

void GAIN_FACTOR::load_Q_MPV(GRID& g)
{
  Char_t t[500];
  sprintf(t,"%s/betaRunsMPV/corrected_MPV_%s_%s.txt",getenv("UCNA_CAL_DIR"), type, geometry);
  ifstream Qin;
  Qin.open(t);
  cout << "Opened " << t << endl;
  Int_t runHold;
  Double_t Ehold, Whold;
  while (Qin >> runHold >> Ehold >> Whold)
    {
      cout << runHold << " " << Ehold << " " << Whold << endl;
      for (UInt_t i=0; i<runs.size(); i++)
	{
	  if (runHold==runs[i]) 
	    {
	      mpvEast[i]=Ehold;
	      mpvWest[i]=Whold;
	      
	    }
	}
    }
}



void GAIN_FACTOR::loadEta(const GRID& g)
{
  Int_t XeRunBegin[9] = {17570, 18081, 18390, 18712, 19873, 21596, 21966, 22961};
  Int_t XeRunEnd[9] = {17610, 18090, 18413, 18744, 19898, 21605, 22003, 22979};
  Int_t XeSize = 9;
  Int_t XeMin, XeMax;

  etaEast.resize(g.numBins, 0.);
  etaWest.resize(g.numBins, 0.);
  
  if (runs[0]>=16983 && runs[0]<=18055)
    {
      XeMin=XeRunBegin[0];
      XeMax=XeRunEnd[0];
    }
  else if (runs[0]>=18081 && runs[0]<=18386)
    {
      XeMin=XeRunBegin[1];
      XeMax=XeRunEnd[1];
    }
  else if (runs[0]>=18390 && runs[0]<=18683)
    {
      XeMin=XeRunBegin[2];
      XeMax=XeRunEnd[2];
    }
  else if (runs[0]>=18712 && runs[0]<=19239)
    {
      XeMin=XeRunBegin[3];
      XeMax=XeRunEnd[3];
    }
  else if (runs[0]>=19347 && runs[0]<=20000)
    {
      XeMin=XeRunBegin[4];
      XeMax=XeRunEnd[4];
    }
  else if (runs[0]>=20121 && runs[0]<=21605)
    {
      XeMin = XeRunBegin[5];
      XeMax = XeRunEnd[5];
    }

  else if (runs[0]>=21607 && runs[0]<=22238)
    {
      XeMin = XeRunBegin[6];
      XeMax = XeRunEnd[6];
    }
  else if (runs[0]>=22294 && runs[0]<=23173)
    {
      XeMin = XeRunBegin[7];
      XeMax = XeRunEnd[7];
    }
  else
    {
      Int_t it = 0;
      while (it<XeSize && runs[0]>XeRunBegin[it]) it+=1;
      if (it==0)
	{
	  XeMin=XeRunBegin[0];
	  XeMax=XeRunEnd[0];
	}
      else 
	{
	  XeMin=XeRunBegin[it-1];
	  XeMax=XeRunEnd[it-1];
	}
    }

  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  Char_t temp1[500];
  ifstream in1, in2;
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_east.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "East File: " << temp1 << endl;  
  in1.open(temp1); 
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_west.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "West File: " << temp1 << endl;
  in2.open(temp1);
  
  Double_t rmin, rmax, phimin, phimax;
  Int_t bin = 0;
  while (in1 >> rmin >> rmax >> phimin >> phimax >> etaEast[bin])
    {
      in2 >> rmin >> rmax >> phimin >> phimax >> etaWest[bin];
      bin++;
    }
  in1.close();
  in2.close();
  
  // cout << etaEast << " " << etaWest << endl << endl;
}

void GAIN_FACTOR::simMPV()
{
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  Char_t temp2[500];
  sprintf(temp2, "%s/Sims/%s_%i-%ikev_%s_east.dat", CalDir,runType,Emin,Emax,geometry);
  cout << "East Sim File: " << temp2 << endl;
  ifstream in1, in2;
  in1.open(temp2);
  Int_t emin, emax;
  in1 >> emin >> emax >> simEast;
  in1.close();
  
  sprintf(temp2, "%s/Sims/%s_%i-%ikev_%s_west.dat", CalDir,runType,Emin,Emax,geometry);
   cout << "West Sim File: " << temp2 << endl;
  in2.open(temp2);
  in2 >> emin >> emax >> simWest;
  in2.close();

  cout << simEast << " " << simWest << endl << endl;
}

void GAIN_FACTOR::calc_gain()
{
  for (UInt_t jj=0; jj<runs.size(); jj++)
    {
      gEast[jj] = simEast/mpvEast[jj];
      gWest[jj] = simWest/mpvWest[jj];

      outfile << runs[jj] << " " << gEast[jj] << " " << gWest[jj] << endl;
      outfile1 << runs[jj] << " " << mpvEast[jj] << " " << mpvWest[jj] << endl;
    }
}


