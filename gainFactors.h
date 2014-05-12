#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include "GRID.hh"

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
  GAIN_FACTOR(Char_t* type, Char_t* file, Int_t nrings,Int_t emin, Int_t emax ); //loads runs in octet and opens output file
  void loadEta(const GRID&); //loads the eta factors. 
  void calc_Q_MPV(GRID&); //Calculates the MPV of the signal depending on whether it is anode or cathode method
  //void findEta(); //finds the appropriate eta value from a set of Xe runs
  void simMPV(); //loads the simulated MPV values from beta sims
  void calc_gain(); //Calculates the gain and writes to file
  Float_t calcQ(); //calculates the appropriate charge deposition
  void FillHisto(Float_t val, Int_t posBin);
  ~GAIN_FACTOR(); //closes output file

  Double_t simEast, simWest;
  TChain *chain;
  Char_t *type; // Anode/Cathode 
  vector <Int_t> runs; // holds the runs in the octet
  vector <Double_t> mpvWest;
  vector <Double_t> mpvEast;
  vector <Double_t> etaWest; //loaded from the Xe runs
  vector <Double_t> etaEast; //loaded from the Xe runs
  vector <Double_t> gWest;
  vector <Double_t> gEast;
  ofstream outfile, outfile1;
  Int_t nrings;
  TH1F *east;
  TH1F *west;
  TF1 *f1, *f2;
  Int_t PID, Type, Side;
  Int_t Emin, Emax;
  Float_t x_centerE, y_centerE, x_heightE, y_heightE, x_widthE, y_widthE, EvisE, AnodeE;
  Float_t x_centerW, y_centerW, x_heightW, y_heightW, x_widthW, y_widthW, EvisW, AnodeW;

};
