#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>

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

class GRID
{
 public:
  GRID(Int_t m); 
  void construct();
  //Double_t find_ring(Double_t r);
  //Double_t find_phi(Double_t phi, Double_t r_min);
  Int_t find_ring(Double_t r);
  Int_t find_phi(Double_t phi, Double_t r_min);
  Int_t round_down(Double_t input);
  Int_t find_pos(Double_t x, Double_t y);
  Int_t numHists;
  Int_t nrings;
  vector <Int_t> ringSects;
  vector <vector <Int_t> > hists;
  //map <pair<double, double>, int> position;
  vector <vector <Double_t > > position1;
  vector <vector <Double_t > > position2;
  static const Double_t pi = 3.14159265359;
};

class ANALYZER
{
 public:
  ANALYZER(Int_t NumRings, Int_t start, Int_t end, Char_t* side, Int_t elow, Int_t ehigh, Char_t* type); //constructor
  void Print(const GRID&, Int_t hisNum, Double_t MPV);
  void Create_hists(const GRID&);
  void Read_events(GRID&, Int_t file_begin, Int_t file_end, Char_t* Side_selected, Int_t elow, Int_t ehigh);
  void Fill_histo(Int_t hist, Float_t bin);
  void Fit_histos(const GRID&);
  ~ANALYZER();
  
 private:
  vector <TH1F*> hisList;
  Char_t* type;
  Int_t PID, Type, Side, file_begin, file_end;
  Float_t charge_cl, x_min, x_max, y_min, y_max;
  Float_t x_center, y_center, x_height, y_height, x_width, y_width, Evis, Anode;
  ofstream outfile;
};
