
{
  
  Int_t run = 18270;
  Int_t nrings = 6;
  Double_t emin = 400., emax=600.;
  Char_t runType[5]="Beta";
  Char_t type[8]="cathode";
  Char_t file[12]="checker.txt";
  ofstream runs;
  runs.open(file);
  runs << run << endl;
  runs.close();
  GRID grid(nrings);
  GAIN_FACTOR gain(type, file, nrings, emin, emax, runType);
  gain.calc_Q_MPV(grid);
}
  
  
  /*Int_t XeRunBegin[9] = {17570, 18081, 18390, 18712, 19873, 21596, 21966, 22961};
  Int_t XeRunEnd[9] = {17610, 18090, 18413, 18744, 19898, 21605, 22003, 22979};
  Int_t XeSize = 9;
  Int_t XeMin, XeMax;
  vector <double> etaEast;
  vector <double> etaWest;

  etaEast.resize(grid.numBins, 0.);
  etaWest.resize(grid.numBins, 0.);
  
  if (run>=16983 && run<=18055)
    {
      XeMin=XeRunBegin[0];
      XeMax=XeRunEnd[0];
    }
  else if (run>=18081 && run<=18386)
    {
      XeMin=XeRunBegin[1];
      XeMax=XeRunEnd[1];
    }
  else if (run>=18390 && run<=18683)
    {
      XeMin=XeRunBegin[2];
      XeMax=XeRunEnd[2];
    }
  else if (run>=18712 && run<=19239)
    {
      XeMin=XeRunBegin[3];
      XeMax=XeRunEnd[3];
    }
  else if (run>=19347 && run<=20000)
    {
      XeMin=XeRunBegin[4];
      XeMax=XeRunEnd[4];
    }
  else if (run>=20121 && run<=21605)
    {
      XeMin = XeRunBegin[5];
      XeMax = XeRunEnd[5];
    }

  else if (run>=21607 && run<=22238)
    {
      XeMin = XeRunBegin[6];
      XeMax = XeRunEnd[6];
    }
  else if (run>=22294 && run<=23173)
    {
      XeMin = XeRunBegin[7];
      XeMax = XeRunEnd[7];
    }
  else
    {
      Int_t it = 0;
      while (it<XeSize && run>XeRunBegin[it]) it+=1;
      if (it==0)
	{
	  XeMin=XeRunBegin[0];
	  XeMax=XeRunEnd[0];
	}
      else 
	{
	  XeMin=XeRunBegin[it-1];
	  XeMax=XeRunEnd[it-1];
	}
    }

  Char_t *CalDir = getenv("UCNA_CAL_DIR");
  Char_t temp1[500];
  ifstream in1, in2;
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_east.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "East File: " << temp1 << endl;  
  in1.open(temp1); 
  sprintf(temp1, "%s/posMaps/%irings_400-600keV_posMap_%i-%i_%s_west.dat", CalDir, nrings, XeMin, XeMax, type);
  cout << "West File: " << temp1 << endl;
  in2.open(temp1);
  
  Double_t rmin, rmax, phimin, phimax;
  Int_t bin = 0;
  while (in1 >> rmin >> rmax >> phimin >> phimax >> etaEast[bin])
    {
      in2 >> rmin >> rmax >> phimin >> phimax >> etaWest[bin];
      //cout << rmin << rmax << phimin << phimax << etaWest[bin] << endl;
      bin++;
    }
  in1.close();
  in2.close();



  sprintf(temp, "/extern/mabrow05/ucna/replay/2011/02252013/hists/spec_%i.root", run);
  TFile *f = new TFile(temp, "READ");
  TTree *Tin = (TTree*)(f->Get("phys"));

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
  
  for (Int_t mm=0; mm<nevents; mm++)
    {
      Tin->GetEvent(mm);
      
      if (Type==0 && PID==1)
	{
	  Double_t r = sqrt(x_centerE*x_centerE + y_centerE*y_centerE);
	  if (r < 50. && r>1.)
	    {
	      Double_t phi = 0.;
	      if (y_centerE >= 0.) {phi = acos(x_centerE/r);}
	      else if (y_centerE<0.) phi = 2*g.pi-acos(x_centerE/r);
	      
	      Int_t binNum = g.find_pos(r, phi);
	      Float_t qValue = calcQ();
	      //Energy and side cut is taken care of in FillHisto...
	      FillHisto(qValue,binNum);
	    }
	  else if (r<1.)
	    {
	      Int_t binNum = g.find_pos(0., 0.);
	      Float_t qValue = calcQ();
	      FillHisto(qValue,binNum);
	    }
	}
    }*/

