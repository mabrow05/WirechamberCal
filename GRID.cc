#include "GRID.hh"

using namespace std;

GRID::GRID(Int_t m)
{
  nrings = m;
  GRID::construct();
}

void GRID::construct()
{
  Int_t sectsHold = 0, sects = 0;
  ringSects.resize(nrings, 0);
  binNum.resize(nrings, vector<Int_t>(1,0));
  for (int ll = 0; ll<nrings; ll++)
    {
      sectsHold = (int)floor(2.*pi*ll)+1;
      sects += sectsHold;
      ringSects[ll]=sectsHold;
      binNum[ll].resize(sectsHold, 0);
    }
  
  Double_t r_min = 0.; // Initialize r_min
  position1.resize(sects, vector<Double_t>(2,0.));
  position2.resize(sects, vector<Double_t>(2,0.));
  Int_t kk = 0;

  for (Int_t ii=0; ii<nrings; ii++)
    {
      Double_t phi_min = 0.;
      Double_t deltaPhi = 2.*pi/(double)ringSects[ii];
      
      for (Int_t jj=0; jj<ringSects[ii]; jj++)
	{
	  //cout << r_min << " " << phi_min << " " << kk << endl;
	  //pair <double, double> pos (r_min, phi_min); //create pair
	  //position[pos] = kk; //add position to map with index kk
	  binNum[ii][jj] = kk;
	  position1[kk][0] = r_min;
	  position1[kk][1] = phi_min;	  
	  phi_min+=deltaPhi;
	  position2[kk][1] = phi_min;
	  position2[kk][0] = 50.*((double)ii+0.5)/((double)nrings-0.5);
	  kk++;
	}
      r_min = 50.*((double)ii+0.5)/((double)nrings-0.5);
    }

  numBins = position2.size();
  cout << "Number of points in second grid: " << numBins << endl;
  cout << "Grid has been constructed" << endl;
  //cout << kk << " " << binNum[3][0] << endl;
  }



int GRID::round_down(Double_t input)
{      
  Int_t val = (int)floor(input);
  return val;
}

Int_t GRID::find_ring(Double_t r)
{
  Int_t index = (int)floor(r/50.*((double)nrings-0.5)+0.5);
  return index;
}

Int_t GRID::find_phi(Double_t phi, Double_t ring_hold)
{
  
  Int_t numSects = ringSects[(int)ring_hold];
  Double_t deltaPhi = 2.*pi/(double)numSects;
  Int_t phiMinIndex = (int)floor(phi/deltaPhi);
  //cout << "number of sectors in ring " << ring_hold << ": " << numSects << endl;
  if (phi>(2*pi-deltaPhi)) return numSects-1;
  else return phiMinIndex;
}

Int_t GRID::find_pos(Double_t r, Double_t phi)
{
  
  Int_t ring = find_ring(r);
  //Double_t r_hold = ((double)ring-.5)/((double)nrings-0.5)*50.;
  Int_t phiIndex = find_phi(phi, ring);
  //if (ring_hold==0.) {r_hold=0.; phi_hold=0.;}

  //pair <double, double> pos(r_hold, phi_hold);

  //cout << "Ring " << ring << ", sector " << phiIndex << " at " << r_hold << endl;
  //cout << binNum[ring][phiIndex] << endl;
  return(binNum[ring][phiIndex]);
}
