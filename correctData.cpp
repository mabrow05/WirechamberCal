
#include "correctData.h"

using namespace std;


ANALYZER::ANALYZER(Int_t nrings, Int_t run, Int_t XeBegin, Int_t XeEnd, Int_t emin, Int_t emax, Char_t* t) //done
{
  XeFileBegin = XeBegin;
  XeFileEnd = XeEnd;
  Emin = emin;
  Emax = emax;
  numEnBins = 1;
  fileNum = run;
  type = t;
  ANALYZER::SetDirs();
  Char_t temp[300];
  sprintf(temp,"%s/correctedData/RunNum_%i_%irings_%i-%ikev_%s.root", CalDir,fileNum, nrings, Emin, Emax, type);
  f1 = new TFile(temp, "RECREATE");
  cout << temp << " is open" << endl;
}
ANALYZER::~ANALYZER(){ f1->Write(); f1->Close(); }

void ANALYZER::SetDirs()
{
  CalDir = getenv("UCNA_CAL_DIR");
  G4Dir = getenv("G4WORKDIR");
}

/*Int_t ANALYZER::find_Ebin(Double_t E)
{
  if (E<800. && E>0.)
    {
      Int_t bin = (int)(floor(E/EnBin));
      return bin;
    }
  else if(E>800.) {return numEnBins-1;}
  else {return 0;}
  }*/

void ANALYZER::Print(const GRID& g, Int_t hisNum, Double_t MPV)
{
  Double_t r_min = g.position1[hisNum][0];
  Double_t phi_min = g.position1[hisNum][1];
  Double_t r_max = g.position2[hisNum][0];
  Double_t phi_max = g.position2[hisNum][1];
  //outfile << r_min << " " << r_max << " " << phi_min << " " << phi_max << " " << MPV << endl;
}

void ANALYZER::etaFill(const GRID& g) //done
{
  Double_t rmin, rmax, phimin, phimax, etaE, etaW;
  ifstream etaInputE, etaInputW;
  Char_t temp[500];
  etaValsE.resize(g.numBins, 0.);
  etaValsW.resize(g.numBins, 0.);

  //Int_t elow = 0;
  //Int_t ehigh = EnBin;
  
  sprintf(temp, "%s/posMaps/%irings_%i-%ikeV_posMap_%i-%i_%s_east.dat",CalDir, g.nrings, Emin, Emax, XeFileBegin, XeFileEnd, type);
  etaInputE.open(temp);
  sprintf(temp, "%s/posMaps/%irings_%i-%ikeV_posMap_%i-%i_%s_west.dat",CalDir, g.nrings, Emin, Emax, XeFileBegin, XeFileEnd, type);
  etaInputW.open(temp);

  cout << endl << temp << endl;
  
  Int_t iter = 0;
  while (etaInputE >> rmin >> rmax >> phimin >> phimax >> etaE)
    {
      etaValsE[iter]=etaE;

      etaInputW >> rmin >> rmax >> phimin >> phimax >> etaW;
      etaValsW[iter]=etaW;

      cout << iter << " " << etaValsW[iter] << endl;
      iter++;
    }
      //elow+=EnBin;
      //ehigh+=EnBin;

  etaInputE.close();
  etaInputW.close();
      //}
  cout << "Eta values are loaded...\n\n\n";

  Char_t geometry[10];
  if (fileNum>20120) {sprintf(geometry,"2012-2013");}
  else {sprintf(geometry, "2011-2012");}

  sprintf(temp, "%s/gainFactors/gain_%s_%s.txt",CalDir, type, geometry); 
  Int_t run=0;
  Double_t gE, gW;
  ifstream gainsIn;
  gainsIn.open(temp);
  cout << "gain file opened\n\n";
 
  while (fileNum!=run)
    {
      gainsIn >> run >> gE >> gW;
      //cout << run << endl;
    }
  gainE = gE;
  gainW = gW;
	 	
  gainsIn.close();
  cout << "Run " << fileNum << " gain E=" << gainE << " and gain W=" << gainW << endl;

}
  

void ANALYZER::Create_hists(const GRID& g) //done
{
  finalHistoE = new TH1F("MWPCE", "East Corrected Histogram", 200, 0., 50.);
  finalHistoW = new TH1F("MWPCW", "West Corrected Histogram", 200, 0., 50.);

  cout << "Made the histograms.\n\n\n"; 
}

void ANALYZER::Read_events(GRID& grid) //done
{
  //Int_t hisNum = 0;
  Char_t temp2[100];

  cout << "reading file " << fileNum << endl;

  sprintf(temp2, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root",fileNum);
  
  TFile *filein = new TFile(temp2, "READ");
  TTree *Tin = (TTree*)(filein->Get("phys"));

  Tin->SetBranchAddress("PID", &PID);
  Tin->SetBranchAddress("Type", &Type);
  Tin->SetBranchAddress("Side", &Side);
  Tin->GetBranch("xEmpm")->GetLeaf("center")->SetAddress(&x_centerE);
  Tin->GetBranch("yEmpm")->GetLeaf("center")->SetAddress(&y_centerE);
  Tin->GetBranch("xEmpm")->GetLeaf("height")->SetAddress(&x_heightE);
  Tin->GetBranch("yEmpm")->GetLeaf("height")->SetAddress(&y_heightE);
  Tin->GetBranch("xEmpm")->GetLeaf("width")->SetAddress(&x_widthE);
  Tin->GetBranch("yEmpm")->GetLeaf("width")->SetAddress(&y_widthE);
  Tin->GetBranch("xWmpm")->GetLeaf("center")->SetAddress(&x_centerW);
  Tin->GetBranch("yWmpm")->GetLeaf("center")->SetAddress(&y_centerW);
  Tin->GetBranch("xWmpm")->GetLeaf("height")->SetAddress(&x_heightW);
  Tin->GetBranch("yWmpm")->GetLeaf("height")->SetAddress(&y_heightW);
  Tin->GetBranch("xWmpm")->GetLeaf("width")->SetAddress(&x_widthW);
  Tin->GetBranch("yWmpm")->GetLeaf("width")->SetAddress(&y_widthW);
  Tin->SetBranchAddress("AnodeE", &AnodeE);
  Tin->SetBranchAddress("AnodeW", &AnodeW);
  Tin->SetBranchAddress("EvisE", &EvisE);
  Tin->SetBranchAddress("EvisW", &EvisW);
  
  
  Int_t nevents = Tin->GetEntriesFast();
  
  cout << "Got number of events\n\n";
  
  for (Int_t mm=0; mm<nevents; mm++)
    {
      Tin->GetEvent(mm);
      
      if (Side==0)
	{
	  //Int_t Ebin = find_Ebin(EvisE); //determines the integer value of the appropriate energy bin
	  //cout << mm << " " << Ebin << endl;
	  
	  if (Type==0 && PID==1)
	    {
	      Double_t r = sqrt(x_centerE*x_centerE + y_centerE*y_centerE);
	      if (r < 50. && r>1.)
		{
		  Double_t phi = 0.;
		  if (y_centerE >= 0.) {phi = acos(x_centerE/r);}
		  else if (y_centerE<0.) phi = 2*grid.pi-acos(x_centerE/r);
		  
		  Int_t binNum = grid.find_pos(r, phi);
		  
		  if (*type=='c')
		    {
		      charge_cl = x_widthE*x_heightE+y_widthE*y_heightE;
		      ChClCorrected = charge_cl*gainE/etaValsE[binNum]; 
		      
		      //if (mm<100) {cout << binNum << " " << charge_cl << " " << etaVals[binNum] << " " << ChClCorrected << endl;}
		      
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "x = " << x_center << " y = " << y_center << " r = " << r << " phi = " << phi << endl;
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      Fill_histo(0, ChClCorrected);
		    }
		  else
		    {
		      AnodeCorrected = AnodeE*gainE/etaValsE[binNum];
		      Fill_histo(0, AnodeCorrected);
		    }
		}
	      else if (r<1.)
		{
		  Int_t binNum = grid.find_pos(0., 0.);
		  if (*type=='c')
		    {
		      charge_cl = x_widthE*x_heightE+y_widthE*y_heightE;
		      ChClCorrected = charge_cl*gainE/etaValsE[binNum];
		      
		      //if (mm<100) {cout << binNum << " " << charge_cl << " " << etaVals[binNum] << " " << ChClCorrected << endl;}
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      Fill_histo(0,ChClCorrected);
		    }
		  else 
		    {
		      AnodeCorrected = AnodeE*gainE/etaValsE[binNum];
		      Fill_histo(0,AnodeCorrected);
		    }
		}
	    }
	}
      else if(Side==1)
	{
	  //Int_t Ebin = find_Ebin(EvisW); //determines the integer value of the appropriate energy bin
	  //cout << mm << " " << Ebin << endl;
	      
	  if (Type==0 && PID==1)
	    {
	      Double_t r = sqrt(x_centerW*x_centerW + y_centerW*y_centerW);
	      if (r < 50. && r>1.)
		{
		  Double_t phi = 0.;
		  if (y_centerW >= 0.) {phi = acos(x_centerW/r);}
		  else if (y_centerW<0.) phi = 2*grid.pi-acos(x_centerW/r);
		  
		  Int_t binNum = grid.find_pos(r, phi);
		  if (*type=='c')
		    {
		      charge_cl = x_widthW*x_heightW+y_widthW*y_heightW;
		      ChClCorrected = charge_cl*gainW/etaValsW[binNum];
		      
		      //if (mm<100) {cout << binNum << " " << charge_cl << " " << etaVals[binNum] << " " << ChClCorrected << endl;}
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      Fill_histo(1,ChClCorrected);
		    }
		  else 
		    {
		      AnodeCorrected = AnodeW*gainW/etaValsW[binNum];
		      Fill_histo(1,AnodeCorrected);
		    }		      
		}
	      else if (r<1.)
		{
		  Int_t binNum = grid.find_pos(0., 0.);
		  if (*type=='c')
		    {
		      charge_cl = x_widthW*x_heightW+y_widthW*y_heightW;
		      ChClCorrected = charge_cl*gainW/etaValsW[binNum];
		      
		      //if (mm<100) {cout << binNum << " " << charge_cl << " " << etaVals[binNum] << " " << ChClCorrected << endl;}
		      
		      //cout << "calculated charge cloud\n";
		      //cout << "Filling Histogram " << hisNum << endl << endl;
		      Fill_histo(1,ChClCorrected);
		    }
		  else 
		    {
		      AnodeCorrected = AnodeW*gainW/etaValsW[binNum];
		      Fill_histo(1,AnodeCorrected);
		    }
		}	  	      
	    }
	}
    }
}

void ANALYZER::Fill_histo(Int_t s, Float_t value) //done
{
  if (s==0){finalHistoE->Fill(value);}
  else { finalHistoW->Fill(value);}
}

void ANALYZER::Fit_histo(const GRID& grid) //done
{
  //here I want to read in the map values with their keys, 
  //then fit each histogram, and call print for the app
  //appropriate values.
  Double_t max_bin = finalHistoE->GetBinCenter(finalHistoE->GetMaximumBin());
  TF1 *f1 = new TF1("f1", "landau", 0.1, 20.);
  //f1->SetParLimits(1, 0.0, 18000.);
  f1->SetParLimits(0, 0.0, 2.5E5);
  //f1->SetParLimits(1, 0.0, 40000.);
  f1->SetParameters(3.e3,max_bin, 1.);
	  
  finalHistoE->Fit("f1", "RMB"); 
	      
  Double_t mpv = f1->GetParameter(1);

  ofstream ofileEast;
  Char_t temp[100];
  sprintf(temp,"East_mpvTrack_%s.dat", type);
  ofileEast.open(temp, ios::app);
  ofileEast << fileNum << " " << mpv << " " << f1->GetParError(1) << endl ;
  ofileEast.close();

  max_bin = finalHistoW->GetBinCenter(finalHistoW->GetMaximumBin());
  TF1 *f2 = new TF1("f2", "landau", 0.1, 20.);
  //f1->SetParLimits(1, 0.0, 18000.);
  f2->SetParLimits(0, 0.0, 2.5E5);
  //f1->SetParLimits(1, 0.0, 40000.);
  f2->SetParameters(3.e3,max_bin, 1.);
	  
  finalHistoW->Fit("f2", "RMB"); 
	      
  mpv = f2->GetParameter(1);

  ofstream ofileWest;
  sprintf(temp,"West_mpvTrack_%s.dat", type);
  ofileWest.open(temp, ios::app);
  ofileWest << fileNum << " " << mpv << " " << f2->GetParError(1) << endl ;
  ofileWest.close();
}
  
  

int main(int argc, char *argv[])
{

  if (argc < 7)
    {
      cout << "Usage: ./correctData [Xe fileBegin] [Xe fileEnd] [numRings] [Emin] [Emax] [anode/cathode] [RunNumber]\n";
      //I don't want this to take elow or ehigh anymore. Just the Enbin. 
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
  Int_t XefileBegin = atoi(argv[1]);
  Int_t XefileEnd = atoi(argv[2]);
  Int_t emin = atoi(argv[4]);
  Int_t emax = atoi(argv[5]);
  Char_t* type = argv[6];
  Int_t runNumber = atoi(argv[7]);

  cout << "I got to GRID\n\n";
  GRID g(numRings);
  cout << "I made the GRID\n\n";
  //ANALYZER analyzer(argv[5], gridSize);
  ANALYZER analyzer(numRings,runNumber, XefileBegin, XefileEnd, emin, emax, type);
  
  analyzer.Create_hists(g);
  analyzer.etaFill(g);
  analyzer.Read_events(g);
  analyzer.Fit_histo(g);

}


  

