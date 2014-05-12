#include "gainFactors.h"
#include "TCut.h"

using namespace std;

GAIN_FACTOR::GAIN_FACTOR(Char_t* t, Char_t* f, Int_t n)
{
  type = t;
  nrings = n;
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
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
  Char_t geometry[10];
  if (run>20120) {sprintf(geometry,"2012-2013");}
  else {sprintf(geometry,"2011-2012");}
  sprintf(temp,"%s/gainFactors/gain_%s_%s.txt", CalDir, t, geometry);
  outfile.open(temp, ios::app);
  
  sprintf(temp,"%s/betaRunsMPV/MPV_%s_%s.txt", CalDir, t, geometry);
  outfile1.open(temp, ios::app);
}

GAIN_FACTOR::~GAIN_FACTOR()
{
  outfile.close();
  outfile1.close();
}

void GAIN_FACTOR::loadData()
{
  chain = new TChain("phys");
  Char_t temp[200];

  for (unsigned int i=0; i<runs.size(); i++)
    {
      sprintf(temp, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root", runs[i]);
      chain->Add(temp);
      cout << "added " << temp << " to chain..." << endl;
    }
  cout << endl;
}

void GAIN_FACTOR::calc_Q_MPV()
{
  Char_t temp[200];
  for (UInt_t ii=0; ii<runs.size();ii++)
    {
      sprintf(temp, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root", runs[ii]);
      TFile *f1 = new TFile(temp, "READ");
      TTree *Tin = (TTree*)(f1->Get("phys"));

      if (*type=='a')
	{
	  TH1F *east = new TH1F("east", "east", 100, 0., 2500.);
	  TH1F *west = new TH1F("west", "west", 100, 0., 2500.);
	  Double_t mpvE = 0., mpvW = 0.;

	  Double_t rad = 10.;
	  while (mpvE<100. || mpvE>1000.)
	    {
	      sprintf(temp,"(xEmpm.center*xEmpm.center+yEmpm.center*yEmpm.center)<%.2f*%.2f", rad, rad);
	      TCut cut1 = temp;
	      Tin->Draw("AnodeE>>east", "Type==0 && Side==0 && PID==1 && EvisE>400. && EvisE<600."&&cut1);
	      TF1 *f1 = new TF1("f1", "landau", 10., 1500.);
	      east->Fit("f1", "RM");
	      mpvE = f1->GetParameter(1);
	      rad+=1;
	      f1->Delete();
	      if (rad>20.) break;
	    }
	  mpvEast[ii] = mpvE;

	  rad=10.;
	  while (mpvW<100. || mpvW>1000.)
	    {
	      sprintf(temp,"(xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<%.2f*%.2f", rad, rad);
	      TCut cut1 = temp;
	      Tin->Draw("AnodeW>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600."&&cut1);
	      TF1 *f2 = new TF1("f2", "landau", 10., 1500.);
	      west->Fit("f2", "RM");
	      mpvW = f2->GetParameter(1);
	      rad+=1;
	      f2->Delete();
	      if (rad>20.) break;
	    }
	  mpvWest[ii] = mpvW;
	}
	
	/*{
	  TH1F *east = new TH1F("east", "east", 100, 0., 2500.);
	  TH1F *west = new TH1F("west", "west", 100, 0., 2500.);
	  Double_t mpvW = 0., mpvE = 0.;
      
	  Tin->Draw("AnodeE>>east", "Type==0 && Side==0 && PID==1 && EvisE>400. && EvisE<600. && (xEmpm.center*xEmpm.center+yEmpm.center*yEmpm.center)<10.*10.");
	  TF1 *f1 = new TF1("f1", "landau", 0., 1500.);
	  east->Fit("f1", "RM");
	  mpvE = f1->GetParameter(1);
	  if (mpvE<100. || mpvE>1000.)
	    {
	      Tin->Draw("AnodeE>>east", "Type==0 && Side==0 && PID==1 && EvisE>400. && EvisE<600. && (xEmpm.center*xEmpm.center+yEmpm.center*yEmpm.center)<15.*15.");
	      east->Fit("f1","RM");
	      mpvEast[ii] = f1->GetParameter(1);
	    }
	  else {mpvEast[ii] = mpvE;}

	  Tin->Draw("AnodeW>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600. && (xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<10.*10.");	  
	  TF1 *f2 = new TF1("f2", "landau", 0., 1500.);
	  west->Fit("f2", "RM");
	  mpvW = f2->GetParameter(1);
	  if (mpvW<100. || mpvW>1000.)
	    {
	      Tin->Draw("AnodeW>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600. && (xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<15.*15.");
	      west->Fit("f2","RM");
	      mpvWest[ii] = f2->GetParameter(1);
	    }
	  else {mpvWest[ii] = mpvW;}

	  cout << f1->GetParameter(1) << " " << f2->GetParameter(1) << endl;
	  }*/
      else if (*type=='c')
	{
	  TH1F *east = new TH1F("east", "east", 100, 0., 80000.);
	  TH1F *west = new TH1F("west", "west", 100, 0., 80000.);
	  Double_t mpvE = 0., mpvW = 0.;

	  Double_t rad = 10.;
	  while (mpvE<3000. || mpvE>14000.)
	    {
	      sprintf(temp,"(xEmpm.center*xEmpm.center+yEmpm.center*yEmpm.center)<%.2f*%.2f", rad, rad);
	      TCut cut1 = temp;
	      Tin->Draw("(xEmpm.height*xEmpm.width+yEmpm.height*yEmpm.width)>>east", "Type==0 && Side==0 && PID==1 && EvisE>400. && EvisE<600."&&cut1);
	      TF1 *f1 = new TF1("f1", "landau", 30., 40000.);
	      east->Fit("f1", "RM");
	      mpvE = f1->GetParameter(1);
	      rad+=1;
	      f1->Delete();
	      if (rad>20.) break;
	    }
	  mpvEast[ii] = mpvE;
	      /*if (mpvE<4500. || mpvE>12000.)
	    {
	      Tin->Draw("(xEmpm.height*xEmpm.width+yEmpm.height*yEmpm.width)>>east", "Type==0 && Side==0 && PID==1 && EvisE>400. && EvisE<600. && (xEmpm.center*xEmpm.center+yEmpm.center*yEmpm.center)<15.*15.");
	      east->Fit("f1", "RM");
	      mpvEast[ii] = f1->GetParameter(1);
	    }
	    else {mpvEast[ii] = mpvE;}*/

	  rad=10.;
	  while (mpvW<4500. || mpvW>12000.)
	    {
	      sprintf(temp,"(xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<%.2f*%.2f", rad, rad);
	      TCut cut1 = temp;
	      Tin->Draw("(xWmpm.height*xWmpm.width+yWmpm.height*yWmpm.width)>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600."&&cut1);
	      TF1 *f2 = new TF1("f2", "landau", 30., 40000.);
	      west->Fit("f2", "RM");
	      mpvW = f2->GetParameter(1);
	      rad+=1;
	      f2->Delete();
	      if (rad>20.) break;
	    }
	  mpvWest[ii] = mpvW;

	      /*if (mpvW<4500. || mpvW>12000.)
	    {
	      Tin->Draw("(xWmpm.height*xWmpm.width+yWmpm.height*yWmpm.width)>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600. && (xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<15.*15.");
	      mpvWest[ii] = f2->GetParameter(1);
	    }
	    else {mpvWest[ii] = mpvW;}*/
	}
    }
}

void GAIN_FACTOR::findEta()
{
  Int_t XeRunBegin[9] = {16983, 17561, 18081, 18390, 18712, 19873, 21596, 21966, 22961};
  Int_t XeRunEnd[9] = {17078, 17734, 18090, 18413, 18744, 19898, 21605, 22003, 22979};
  Int_t XeSize = 9;
  Int_t XeMin, XeMax;

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
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  Char_t temp1[500];
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_east.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "East File: " << temp1 << endl;
  ifstream in1, in2;
  in1.open(temp1);
  Double_t rmin, rmax, phimin, phimax;
  in1 >> rmin >> rmax >> phimin >> phimax >> etaEast;
  in1.close();
  
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_west.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "West File: " << temp1 << endl;
  in2.open(temp1);
  in2 >> rmin >> rmax >> phimin >> phimax >> etaWest;
  in2.close();

  // cout << etaEast << " " << etaWest << endl << endl;
}

void GAIN_FACTOR::simMPV()
{
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  Char_t temp2[500];
  sprintf(temp2, "%s/Sims/Beta_400-600kev_east.dat", CalDir);
  cout << "East Sim File: " << temp2 << endl;
  ifstream in1, in2;
  in1.open(temp2);
  Int_t emin, emax;
  in1 >> emin >> emax >> simEast;
  in1.close();
  
  sprintf(temp2, "%s/Sims/Beta_400-600kev_west.dat", CalDir);
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
      gEast[jj] = simEast*etaEast/mpvEast[jj];
      gWest[jj] = simWest*etaWest/mpvWest[jj];

      outfile << runs[jj] << " " << gEast[jj] << " " << gWest[jj] << endl;
      outfile1 << runs[jj] << " " << mpvEast[jj] << " " << mpvWest[jj] << endl;
    }
}


int main(int argc, char *argv[])
{
  if (argc<3)
    {
      cout << "Usage: ./gainFactors.exe [nrings] [anode/cathode] [file w/ runs]" << endl;
    } 
  GAIN_FACTOR g(argv[2], argv[3], atoi(argv[1]));
  //g.loadData();
  g.calc_Q_MPV();
  g.findEta();
  g.simMPV();
  g.calc_gain();
}
