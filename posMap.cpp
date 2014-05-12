
#include "posMap.h"

using namespace std;

posMap::posMap(int n, int emin, int emax, char* t, int XeMin, int XeMax)
{
  Emin = emin;
  Emax = emax;
  nrings = n;
  type = t;
  XeRunMin = XeMin;
  XeRunMax = XeMax;

  posMap::OpenDataFiles(Emin, Emax);
  posMap::calcAverage();
  posMap::CloseDataFiles();
  posMap::OpenDataFiles(Emin, Emax);
  posMap::calcEta();
  posMap::CloseDataFiles();

}

posMap::~posMap(){}

void posMap::calcAverage()
{
  double rMin=0., rMax=0., phiMin=0., phiMax=0., eastHold=0., westHold=0.;
  double anodeVal=0.;
  westAverage=0.;
  eastAverage=0.;
  int n = 0.;

  while (eastData >> rMin >> rMax >> phiMin >> phiMax >> anodeVal)
    {
      eastHold+=anodeVal;

      westData >> rMin >> rMax >> phiMin >> phiMax >> anodeVal;

      westHold+=anodeVal;
	      
      n++;
    }
  eastAverage = eastHold/(double)n;
  westAverage = westHold/(double)n;

  cout << "East Average = " << eastAverage << endl;

}

void posMap::OpenDataFiles(int EbinLow, int EbinHigh)
{
  char temp[500];
  char* CalDB = getenv("UCNA_CAL_DIR");
  sprintf(temp, "%s/posMaps/%irings_%i-%ikeV_posMap_%i-%i_%s_east.dat",CalDB, nrings, EbinLow, EbinHigh, XeRunMin, XeRunMax, type);
  eastEta.open(temp);
  sprintf(temp, "%s/posMaps/%irings_%i-%ikeV_posMap_%i-%i_%s_west.dat",CalDB, nrings, EbinLow, EbinHigh, XeRunMin, XeRunMax, type);
  westEta.open(temp);

  sprintf(temp, "%s/Xe_landauFitted/%i-%i_%s_%irings%i-%ikeV_east.dat", CalDB, XeRunMin, XeRunMax, type, nrings, EbinLow, EbinHigh);
  eastData.open(temp);
  sprintf(temp, "%s/Xe_landauFitted/%i-%i_%s_%irings%i-%ikeV_west.dat", CalDB, XeRunMin, XeRunMax, type, nrings, EbinLow, EbinHigh);
  westData.open(temp);
}



void posMap::CloseDataFiles()
{
  eastData.close();
  westData.close();
  eastEta.close();
  westEta.close();
}

void posMap::calcEta()
{
  double rMin=0., rMax=0., phiMin=0., phiMax=0.;
  double anodeVal=0. , eta = 0.;

  while (eastData >> rMin >> rMax >> phiMin >> phiMax >> anodeVal)
    {
      eta = anodeVal/eastAverage;
	      
      eastEta << rMin << " " << rMax << " " << phiMin << " " << phiMax << " " << eta << endl;
      cout << "east posMap value = " << eta << endl;

      westData >> rMin >> rMax >> phiMin >> phiMax >> anodeVal;

      eta = anodeVal/westAverage;
	      
      westEta << rMin << " " << rMax << " " << phiMin << " " << phiMax << " " << eta << endl;
    }
  
}

////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{

  if (argc < 7)
    {
      cout << "Usage: ./posMap [XeBegin] [XeEnd] [numRings] [Elow] [Ehigh] [anode/cathode]\n";
      return -1;
    }

  int XeBegin = atoi(argv[1]);
  int XeEnd = atoi(argv[2]);
  int nrings = atoi(argv[3]); 
  int Emin = atoi(argv[4]);
  int Emax = atoi(argv[5]);
  char * type = argv[6];
  
  posMap factorizer(nrings, Emin, Emax, type, XeBegin, XeEnd);

}

  
