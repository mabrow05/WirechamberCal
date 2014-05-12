#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cmath>



using namespace std;

class posMap
{
 public:
  posMap(int nrings, int emin, int emax, char* type, int XeRunMin, int XeRunMax); //initializes variables and opens output file
  ~posMap(); //Destructor, closes output files
  //void DoEtaFactors();
  //void LoadSimData();
  void calcAverage();
  void OpenDataFiles(int EbinLow, int EbinHigh); //loads and saves Simulated data and opens Xe data files
  void calcEta(); //calculates eta factor, calls writeEta
  //void writeEta(Double_t rmin, Double_t rmax, Double_t phimin, Double_t phimax); 
  void CloseDataFiles(); //closes data files
  
  int Emin;
  int Emax;
  int nrings;
  double westAverage, eastAverage;
  char* type; //this is anode/cathode
  int XeRunMin, XeRunMax;
  //unsigned int numEnBins;
  ofstream eastEta, westEta;
  ifstream eastData, westData;
  //vector <int> East_EdepSim; //holds edep min of range of sim
  //vector <double> East_MWPCsim; //holds MWPC edep of sim
  //vector <int> West_EdepSim;
  //vector <double> West_MWPCsim;
};
