#include "gainFactors.h"
#include "TCut.h"

using namespace std;

GAIN_FACTOR::GAIN_FACTOR(Char_t* t, Char_t* f, Int_t n, Int_t emin, Int_t emax)
{
  type = t;
  nrings = n;
  Emin = emin;
  Emax = emax;
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

void GAIN_FACTOR::calc_Q_MPV(GRID& g)
{
  Char_t temp[200];
  if (*type=='a')
    {
      east = new TH1F("east", "east", 100, 0., 2500.);
      west = new TH1F("west", "west", 100, 0., 2500.);
      f1 = new TF1("f1", "landau", 10., 1500.);
      f2 = new TF1("f2", "landau", 10., 1500.);
    }
  else if (*type=='c')
    {
      east = new TH1F("east", "east", 100, 0., 80000.);
      west = new TH1F("west", "west", 100, 0., 80000.);
      f1 = new TF1("f1", "landau", 30., 40000.);
      f2 = new TF1("f2", "landau", 30., 40000.);
    }

  for (UInt_t ii=0; ii<runs.size();ii++)
    {
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
      east->Fit("f1", "RM");
      mpvEast[ii] = f1->GetParameter(1);
      f1->Delete();

      west->Fit("f2", "RM");
      mpvWest[ii] = f2->GetParameter(1);
      f2->Delete();
      

	  /*Double_t rad = 10.;
	  while (mpvE<0. || mpvE>5000.)
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
	
	  if (mpvW<100. || mpvW>1000.)
	    {
	      Tin->Draw("AnodeW>>west", "Type==0 && Side==1 && PID==1 && EvisW>400. && EvisW<600. && (xWmpm.center*xWmpm.center+yWmpm.center*yWmpm.center)<15.*15.");
	      west->Fit("f2","RM");
	      mpvWest[ii] = f2->GetParameter(1);
	    }
	  else {mpvWest[ii] = mpvW;}

	  cout << f1->GetParameter(1) << " " << f2->GetParameter(1) << endl;
	  }
      else if (*type=='c')
	{
	  
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

	  }*/
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
  if (Side==0 && EvisE>Emin && EvisE<Emax) east->Fill(val/etaEast[posBin]);
  else if (Side==1 && EvisW>Emin && EvisW<Emax) west->Fill(val/etaWest[posBin]);
}


void GAIN_FACTOR::loadEta(const GRID& g)
{
  Int_t XeRunBegin[9] = {16983, 17561, 18081, 18390, 18712, 19873, 21596, 21966, 22961};
  Int_t XeRunEnd[9] = {17078, 17734, 18090, 18413, 18744, 19898, 21605, 22003, 22979};
  Int_t XeSize = 9;
  Int_t XeMin, XeMax;

  etaEast.resize(0., g.numBins);
  etaWest.resize(0., g.numBins);
  
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
  sprintf(temp2, "%s/Sims/Beta_%i-%ikev_east.dat", CalDir,Emin,Emax);
  cout << "East Sim File: " << temp2 << endl;
  ifstream in1, in2;
  in1.open(temp2);
  Int_t emin, emax;
  in1 >> emin >> emax >> simEast;
  in1.close();
  
  sprintf(temp2, "%s/Sims/Beta_%i-%ikev_west.dat", CalDir,Emin,Emax);
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


int main(int argc, char *argv[])
{
  if (argc<6)
    {
      cout << "Usage: ./gainFactors.exe [nrings] [anode/cathode] [file w/ runs] [Emin] [Emax]" << endl;
    } 

  GRID grid(atoi(argv[1]));
  GAIN_FACTOR g(argv[2], argv[3], atoi(argv[1]), atoi(argv[4]), atoi(argv[5]));
  //g.loadData();
  g.calc_Q_MPV(grid);
  g.simMPV();
  g.calc_gain();
}
