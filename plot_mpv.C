{
  ifstream infileEast, infileWest;
  Char_t *t="cathode";
  //*t="anode";
  //*t="cathode";

  Char_t east[100], west[100];

  sprintf(east, "East_mpvTrack_%s.dat", t);
  sprintf(west, "West_mpvTrack_%s.dat", t);
    
  infileEast.open(east);
  infileWest.open(west);
  Double_t run, mpv, error;
  Int_t Wruns=0, Eruns=0;
  while (infileWest >> run >> mpv >> error)
    {
      if (mpv<10. && mpv>0. && error <2.)
	{
	  Wruns++;
	}
    }
  
  while (infileEast >> run >> mpv >> error)
    {
      if (mpv<10. && mpv>0. && error <2.)
	{
	  Eruns++;
	}
    }
  infileEast.close();    
  infileWest.close();
  Double_t EastRun[Eruns];
  Double_t WestRun[Wruns];
  Double_t EastMPV[Eruns];
  Double_t WestMPV[Wruns];
  Double_t EastError[Eruns];
  Double_t WestError[Wruns];

  infileWest.open(west);
  infileEast.open(east);
    
  

  Int_t jj=0;
  while (infileWest >> run >> mpv >> error)
    {
      if (mpv<10. && mpv>0. && error <2.)
	{
	  WestRun[jj]=run; 
	  WestMPV[jj]=mpv;
	  WestError[jj]=error;
	  jj++;
	}
    }

  Int_t ii = 0;
  while (infileEast >> run >> mpv >> error)
    {
      if (mpv<10. && mpv>0. && error <2.)
	{
	  EastRun[ii]=run; 
	  EastMPV[ii]=mpv;
	  EastError[ii]=error;

	  cout << ii << endl;
	  ii++;
	}
    }
  infileWest.close();
  infileEast.close();

  char temp[100];
  TGraphErrors *East = new TGraphErrors(Eruns, EastRun, EastMPV, 0, EastError);
  sprintf(temp, "East Side MPV: %s",t); 
  East->SetTitle(temp);
  East->GetYaxis()->SetRangeUser(1., 3.5);
  East->GetYaxis()->SetTitle("Energy (keV)");
  East->GetXaxis()->SetTitle("Run Number");
  TGraphErrors *West = new TGraphErrors(Wruns, WestRun, WestMPV, 0, WestError);
  sprintf(temp, "West Side MPV: %s",t);
  West->SetTitle(temp);
  West->GetYaxis()->SetRangeUser(1., 3.5);
  West->GetYaxis()->SetTitle("Energy (keV)");
  West->GetXaxis()->SetTitle("Run Number");
  TCanvas *c1 = new TCanvas("c1");
  TCanvas *c2 = new TCanvas("c2");

  //c1->Divide(2,1);
  c1->cd();
  East->Draw("AP*");
  c2->cd();
  West->Draw("AP*");

}

  
