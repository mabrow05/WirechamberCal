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
#include <TChain.h>

using namespace std;


int main(int argc, char *argv[])
{
  if (argc < 4)
    {
      cout << "Usage: ./simData [numfiles] [Elow] [Ehigh] [geometry]\n";
      return -1;
    }

  Char_t temp2[500];
  ofstream outfile0, outfile1;
  
  Int_t num_files = atoi(argv[1]);
  Int_t Emin = atoi(argv[2]);
  Int_t Emax = atoi(argv[3]);
  Char_t *geometry = argv[4];  

  //Int_t EnergyRange = 800;
  //Int_t nbins = EnergyRange/binWidthEnergy;
  Int_t nbins = 1; //this is just remnant for now.
  //Double_t Energy_min = 0., Energy_max = (double)binWidthEnergy;
  Char_t temp[200], temp1[100], file0[100], file1[100];
  Char_t *calDir = getenv("UCNA_CAL_DIR"); 
  Char_t *G4Dir = getenv("G4WORKDIR");
  sprintf(file0, "%s/Sims/Beta_%i-%ikev_east.dat", calDir, Emin, Emax);
  sprintf(file1, "%s/Sims/Beta_%i-%ikev_west.dat", calDir, Emin, Emax);
  cout << file0 << endl;
  outfile0.open(file0);
  outfile1.open(file1);


  TChain *chain = new TChain("anaTree");

  
      
  for (Int_t file_number = 0; file_number<num_files; file_number++)
    {
	  
      sprintf(temp2, "%s/output/%sgeo_n1_f_n/analyzed_%i.root", G4Dir, geometry, file_number);
      chain->Add(temp2);
    }


  TH1F *east = new TH1F("east", "East Side", 200, 0., 50.);
  TH1F *west = new TH1F("west", "West Side", 200, 0., 50.); 

  for (int i = 0; i<nbins; i++)
    {
      east->Reset();
      west->Reset();
	  

      sprintf(temp, "MWPCEnergy.MWPCEnergyE>1e-5 && MWPCEnergy.MWPCEnergyE<50. && MWPCEnergy.MWPCEnergyW<0.01 && Edep.EdepW<0.1 && Edep.EdepE>%f && Edep.EdepE<%f",(double)Emin,(double)Emax);
      sprintf(temp1, "MWPCEnergy.MWPCEnergyE>>east");
      chain->Draw(temp1, temp);
	  

      sprintf(temp, "MWPCEnergy.MWPCEnergyW>1e-5 && MWPCEnergy.MWPCEnergyW<50. && MWPCEnergy.MWPCEnergyE<0.01 && Edep.EdepE<0.1 && Edep.EdepW>%f && Edep.EdepW<%f",(double)Emin,(double)Emax);
      sprintf(temp1, "MWPCEnergy.MWPCEnergyW>>west");
      chain->Draw(temp1, temp);

	 
      TF1 *f1 = new TF1("f1", "landau", 0.05, 20.);
      f1->SetParLimits(0, 0., 1.e10);
      //f1->SetParLimits(1, 0., 1.e7);
      
      Double_t max_bin = east->GetBinCenter(east->GetMaximumBin());
      f1->SetParameters(5.e4,max_bin, 0.1);
      cout << endl << endl << "For Energy Range: " << Emin << " to " << Emax << endl;
      
      //sprintf(num, "his%i", i)     
      east->Fit("f1","RMBI");
      outfile0 << Emin << " " << Emax << " " << f1->GetParameter(1) << endl;
      delete f1;
      f1=NULL;

      TF1 *f2 = new TF1("f2", "landau", 0.05, 20.);
      f2->SetParLimits(0, 0., 1.e10);
      //f2->SetParLimits(1, 0., 1.e7);

      max_bin = west->GetBinCenter(west->GetMaximumBin());
      f2->SetParameters(5.e4,max_bin,0.1);
      cout << endl << endl << "For Energy Range: " << Emin << " to " << Emax << endl;
      
      //sprintf(num, "his%i", i)     
      west->Fit("f2","RMBI");
      outfile1 << Emin << " " << Emax << " " << f2->GetParameter(1) << endl;
      

      //Energy_min=Energy_min + (double)binWidthEnergy;
      //Energy_max=Energy_max + (double)binWidthEnergy;
      
    }
  outfile0.close();
  outfile1.close();

}    
      
