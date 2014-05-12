#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>
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


using namespace std;

class ANALYZER
{
 public:
  ANALYZER(Int_t NumRings, Int_t file_num, Int_t XeBegin, Int_t XeEnd, Int_t emin, Int_t emax, Char_t* t); //constructor
  void SetDirs();
  //Int_t find_Ebin(Double_t E);
  void Print(const GRID&, Int_t hisNum, Double_t MPV);
  void etaFill(const GRID&); //get position map values and read in the gain values
  void Create_hists(const GRID&);
  void Read_events(GRID&);
  void Fill_histo(Int_t side, Float_t bin);
  void Fit_histo(const GRID&);
  ~ANALYZER();
  
 private:
  //vector <TH1F*> hisList;
  TH1F *finalHistoE;
  TH1F *finalHistoW;
  Char_t* side; //This is the selected side
  Char_t* type;
  Int_t PID, Type, Side, XeFileBegin, XeFileEnd, Emin, Emax, numEnBins, fileNum;
  Float_t charge_cl, ChClCorrected, AnodeCorrected;
  Float_t x_centerE, y_centerE, x_heightE, y_heightE, x_widthE, y_widthE, EvisE, AnodeE;
  Float_t x_centerW, y_centerW, x_heightW, y_heightW, x_widthW, y_widthW, EvisW, AnodeW;
  Double_t gainE, gainW;
  //ofstream outfile;
  TFile *f1;
  vector <Double_t> etaValsE;
  vector  <Double_t> etaValsW;
  Char_t *CalDir;
  Char_t *G4Dir;
};
