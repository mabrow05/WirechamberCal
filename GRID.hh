#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <string>
#include <cstdlib>

#include <TH1F.h>

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
  Int_t numBins;
  Int_t nrings;
  vector <Int_t> ringSects;
  vector <vector <Int_t> > binNum; //number of bin at ring and sector
  //map <pair<double, double>, int> position;
  vector <vector <Double_t > > position1; //stores min position
  vector <vector <Double_t > > position2; // stores max position
  static const Double_t pi = 3.14159265359;
};
