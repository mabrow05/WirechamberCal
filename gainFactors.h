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

class GAIN_FACTOR
{
 public:
  GAIN_FACTOR(Char_t* type, Char_t* file, Int_t nrings); //loads runs in octet and opens output file
  void loadData(); //loads beta decay data from octet and constructs the chain. 
  void calc_Q_MPV(); //Calculates the MPV of the signal depending on whether it is anode or cathode method
  void findEta(); //finds the appropriate eta value from a set of Xe runs
  void simMPV(); //loads the simulated MPV values from beta sims
  void calc_gain(); //Calculates the gain and writes to file
  ~GAIN_FACTOR(); //closes output file

  Double_t etaEast, etaWest, simEast, simWest;
  TChain *chain;
  Char_t *type; // Anode/Cathode 
  vector <Int_t> runs; // holds the runs in the octet
  vector <Double_t> mpvWest;
  vector <Double_t> mpvEast;
  //vector <Double_t> etaWest;
  //vector <Double_t> etaEast;
  vector <Double_t> gWest;
  vector <Double_t> gEast;
  ofstream outfile, outfile1;
  Int_t nrings;

};
