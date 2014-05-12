#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>

#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TH1.h>
#include <TApplication.h>

#include "MWPC_pos_landau.h"

using namespace std;

bool fexists(const Char_t *filename)
{
  ifstream ifile(filename);
  return ifile;
}

ANALYZER::ANALYZER(Int_t nrings, Int_t start, Int_t end, Char_t* side, Int_t elow, Int_t ehigh, Char_t* t)
{
  Char_t temp[500];
  type = t;
  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  sprintf(temp,"%s/Xe_landauFitted/%i-%i_%s_%irings%i-%ikeV_%s.dat", CalDir, start, end, t, nrings,elow, ehigh, side);
  outfile.open(temp);
  cout << temp << " is open" << endl;
}
ANALYZER::~ANALYZER(){ outfile.close(); }
  
void ANALYZER::Print(const GRID& g, Int_t hisNum, Double_t MPV)
{
  Double_t r_min = g.position1[hisNum][0];
  Double_t phi_min = g.position1[hisNum][1];
  Double_t r_max = g.position2[hisNum][0];
  Double_t phi_max = g.position2[hisNum][1];
  
  outfile << r_min << " " << r_max << " " << phi_min << " " << phi_max << " " << MPV << endl;
}

void ANALYZER::Create_hists(const GRID& g)
{
  
  Int_t nHists = g.numBins;
  Char_t temp3[10];
  Int_t nbins = 500;
  Float_t low = 0., high = 150000.;

  if (*type=='a') {high = 2500.;}
  hisList.resize(nHists, 0);

  for (Int_t ii=0; ii<nHists; ii++)
    {
      sprintf(temp3, "his%i", ii);
      hisList[ii] = new TH1F(temp3, temp3, nbins, low, high);
    }

  cout << "Made all " << hisList.size() << " histograms.\n\n\n";
  
}

void ANALYZER::Read_events(GRID& grid, Int_t file_begin, Int_t file_end, Char_t* Side_selected, Int_t eLow, Int_t eHigh)
{
  //Int_t hisNum = 0;
  Char_t temp2[100];

  for (Int_t file_number = file_begin; file_number < file_end+1; file_number++)
    {

      cout << "reading file " << file_number << endl;

      sprintf(temp2, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root",file_number);
      
      if (fexists(temp2))
	{

	  TFile *filein = new TFile(temp2, "READ");
	  TTree *Tin = (TTree*)(filein->Get("phys"));
	  Int_t sideHold = 0;

	  Tin->SetBranchAddress("PID", &PID);
	  Tin->SetBranchAddress("Type", &Type);
	  Tin->SetBranchAddress("Side", &Side);
	  
	  if (*Side_selected == 'e')
	    {
	      Tin->GetBranch("xEmpm")->GetLeaf("center")->SetAddress(&x_center);
	      Tin->GetBranch("yEmpm")->GetLeaf("center")->SetAddress(&y_center);
	      Tin->GetBranch("xEmpm")->GetLeaf("height")->SetAddress(&x_height);
	      Tin->GetBranch("yEmpm")->GetLeaf("height")->SetAddress(&y_height);
	      Tin->GetBranch("xEmpm")->GetLeaf("width")->SetAddress(&x_width);
	      Tin->GetBranch("yEmpm")->GetLeaf("width")->SetAddress(&y_width);
	      Tin->SetBranchAddress("EvisE", &Evis);
	      Tin->SetBranchAddress("AnodeE", &Anode);
	    }
	  else 
	    {
	      Tin->GetBranch("xWmpm")->GetLeaf("center")->SetAddress(&x_center);
	      Tin->GetBranch("yWmpm")->GetLeaf("center")->SetAddress(&y_center);
	      Tin->GetBranch("xWmpm")->GetLeaf("height")->SetAddress(&x_height);
	      Tin->GetBranch("yWmpm")->GetLeaf("height")->SetAddress(&y_height);
	      Tin->GetBranch("xWmpm")->GetLeaf("width")->SetAddress(&x_width);
	      Tin->GetBranch("yWmpm")->GetLeaf("width")->SetAddress(&y_width);
	      Tin->SetBranchAddress("EvisW", &Evis);
	      Tin->SetBranchAddress("AnodeW", &Anode);
	      sideHold = 1;
	}
	  Int_t nevents = Tin->GetEntriesFast();
	  
	  //cout << "Got number of events\n\n";
	  
	  for (Int_t mm=0; mm<nevents; mm++)
	    {
	      Tin->GetEvent(mm);
	      
	      if (Type==0 && PID==1 && Side==sideHold && Evis<eHigh && Evis>eLow)
		{
		  Double_t r = sqrt(x_center*x_center + y_center*y_center);
		  if (r < 50. && r>1.)
		    {
		      Double_t phi = 0.;
		      if (y_center >= 0.) {phi = acos(x_center/r);}
		      else if (y_center<0.) phi = 2.*grid.pi-acos(x_center/r);
		      
		      Int_t hisNum = grid.find_pos(r, phi);
		      charge_cl = x_width*x_height+y_width*y_height;
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "x = " << x_center << " y = " << y_center << " r = " << r << " phi = " << phi << endl;
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      if (*type=='c') Fill_histo(hisNum, charge_cl);
		      else Fill_histo(hisNum, Anode);
		    }
		  else if (r<1.)
		    {
		      Int_t hisNum = grid.find_pos(0., 0.);
		      charge_cl = x_width*x_height+y_width*y_height;
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      if (*type=='c') Fill_histo(hisNum, charge_cl);
		      else Fill_histo(hisNum, Anode);
		    }
		}
	    }
	}
    }
}

void ANALYZER::Fill_histo(Int_t hist, Float_t value)
{
  hisList[hist]->Fill(value);
}

void ANALYZER::Fit_histos(const GRID& grid)
{
  //here I want to read in the map values with their keys, 
  //then fit each histogram, and call print for the app
  //appropriate values.

  Int_t numHists = grid.numBins;
  Double_t max_bin = 0.;
  
  if (*type=='c')
    {
      for (int ll = 0; ll<numHists; ll++)
	{
	  if (hisList[ll]->GetEntries() > 0)
	    {
	      if (hisList[ll]->GetMaximumBin() > 2 || hisList[ll]->GetMaximumBin()<134)
		{
		  max_bin = hisList[ll]->GetBinCenter(hisList[ll]->GetMaximumBin());
		  
		  TF1 *f1 = new TF1("f1", "landau", 500., 40000.);
		  //f1->SetParLimits(1, 0.0, 18000.);
		  f1->SetParLimits(0, 0.0, 2.5E5);
		  //f1->SetParLimits(1, 0.0, 40000.);
		  f1->SetParameter(1,max_bin);
		  f1->SetParameter(2,2000.);
		  
		  hisList[ll]->Fit("f1", "RMBQ"); 
	      
		  Double_t mpv = f1->GetParameter(1);
	  
		  if (mpv < 4000.)
		    {
		      mpv = hisList[ll]->GetBinCenter(hisList[ll]->GetMaximumBin());
		    }
      
		  Print(grid,ll, mpv);
		}
	      else
		{
		  //cout << grid.position2[ll][0] << " " << grid.position2[ll][1] << " had low bin max!" << endl;
		  TF1 *f2 = new TF1("f2", "landau", 500., 40000.);

		  f2->SetParameter(1, 10000.);
		  f2->SetParameter(2, 2000.);
	      
		  hisList[ll]->Fit("f2","RMBQ");
	      
		  Double_t mpv = f2->GetParameter(1);

		  Print(grid, ll, mpv);
		}
	    }
	}
    }

  else 
    {
      for (int ll = 0; ll<numHists; ll++)
	{
	  if (hisList[ll]->GetEntries() > 0)
	    {
	      if (hisList[ll]->GetMaximumBin() > 2 || hisList[ll]->GetMaximumBin()<134)
		{
		  max_bin = hisList[ll]->GetBinCenter(hisList[ll]->GetMaximumBin());
		  
		  TF1 *f1 = new TF1("f1", "landau", 10., 1500.);		  
		  f1->SetParLimits(0, 0.0, 2.5E5);
		  //f1->SetParLimits(1, 0.0, 40000.);
		  f1->SetParameter(1,max_bin);
		  f1->SetParameter(2,40.);
		  
		  hisList[ll]->Fit("f1", "RMBQ"); 
	      
		  Double_t mpv = f1->GetParameter(1);
	  
		  if (mpv < 10. || mpv > 1300.)
		    {
		      mpv = hisList[ll]->GetBinCenter(hisList[ll]->GetMaximumBin());
		    }
      
		  Print(grid,ll, mpv);
		}
	      else
		{
		  //cout << grid.position2[ll][0] << " " << grid.position2[ll][1] << " had low bin max!" << endl;
		  TF1 *f2 = new TF1("f2", "landau", 10., 1500.);

		  f2->SetParameter(1, 200.);
		  f2->SetParameter(2, 40.);
	      
		  hisList[ll]->Fit("f2","RMBQ");
	      
		  Double_t mpv = f2->GetParameter(1);

		  Print(grid, ll, mpv);
		}
	    }
	}
    }
}
  
  

int main(int argc, char *argv[])
{

  if (argc < 8)
    {
      cout << "Usage: ./MWPC_pos_landau [fileBegin] [fileEnd] [numRings] [Side] [Elow] [Ehigh] [anode/cathode]\n";
      return -1;
    }
  Char_t t[100];
  for (int i = 0; i<argc; i++)
    {
      sprintf(t, "Argument %i: ", i);
      cout << t << argv[i] << endl;
    }

  TApplication app("App", &argc, argv);
  Int_t numRings = atoi(argv[3]);
  Int_t fileBegin = atoi(argv[1]);
  Int_t fileEnd = atoi(argv[2]);
  Int_t elow = atoi(argv[5]);
  Int_t ehigh = atoi(argv[6]);
  Char_t* side = argv[4];
  Char_t* type = argv[7];

  GRID g(numRings);
  //ANALYZER analyzer(argv[5], gridSize);
  ANALYZER analyzer(numRings, fileBegin, fileEnd, side, elow, ehigh, type);
  
  analyzer.Create_hists(g);
  analyzer.Read_events(g, fileBegin, fileEnd, side, elow, ehigh);
  analyzer.Fit_histos(g);

}


  

