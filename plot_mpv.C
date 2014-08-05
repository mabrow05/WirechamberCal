{
  /////////////////////////CATHODE////////////////////////////////
  Char_t *runtype = "Beta";
  Char_t geo[10] = "2012-2013";
  if (true)
    {
      ifstream infileEast, infileWest;
      Char_t *t="cathode";
      
      //*t="anode";
      //*t="cathode";
      
      Char_t east[100], west[100];
      
      sprintf(east, "East_mpvTrack_%s_%s_%s.dat", geo, t, runtype);
      sprintf(west, "West_mpvTrack_%s_%s_%s.dat", geo, t, runtype);
      
      Int_t Eruns=0, Wruns=0;
      Double_t run, mpv, error;
      infileEast.open(east);
      infileWest.open(west);
      
      while (infileWest >> run >> mpv >> error)
	{
	  Wruns++;
	}
      
      while (infileEast >> run >> mpv >> error)
	{
	  Eruns++;
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
	  WestRun[jj]=run; 
	  WestMPV[jj]=mpv;
	  WestError[jj]=error;
	  jj++;    
	}
      
      Int_t ii = 0;
      while (infileEast >> run >> mpv >> error)
	{
	  EastRun[ii]=run; 
	  EastMPV[ii]=mpv;
	  EastError[ii]=error;
	      
	  cout << ii << endl;
	  ii++;
	}
      infileWest.close();
      infileEast.close();
      
      char temp[100];
      TGraphErrors *East = new TGraphErrors(Eruns, EastRun, EastMPV, 0, EastError);
      sprintf(temp, "%s East Side MPV: %s",runtype, t); 
      East->SetTitle(temp);
      East->GetYaxis()->SetRangeUser(1., 3.5);
      East->GetYaxis()->SetTitle("Energy (keV)");
      East->GetXaxis()->SetTitle("Run Number");
      TGraphErrors *West = new TGraphErrors(Wruns, WestRun, WestMPV, 0, WestError);
      sprintf(temp, "%s West Side MPV: %s",runtype, t);
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

  ////////////////////////////ANODE///////////////////////////
  if (true)
    {
      ifstream infileEast, infileWest;
      Char_t *t="anode";
      //*t="anode";
      //*t="cathode";
      
      Char_t east[100], west[100];
      
      sprintf(east, "East_mpvTrack_%s_%s_%s.dat", geo, t, runtype);
      sprintf(west, "West_mpvTrack_%s_%s_%s.dat", geo, t, runtype);
      
      infileWest.open(west);
      infileEast.open(east);
      Int_t Wruns=0, Eruns=0;
      Double_t run, mpv, error;
      while (infileWest >> run >> mpv >> error)
	{	    
	  Wruns++;
	}
      
      while (infileEast >> run >> mpv >> error)
	{	    
	  Eruns++;
	}
      infileEast.close();    
      infileWest.close();

      Double_t EastRunA[Eruns];
      Double_t WestRunA[Wruns];
      Double_t EastMPVA[Eruns];
      Double_t WestMPVA[Wruns];
      Double_t EastErrorA[Eruns];
      Double_t WestErrorA[Wruns];

      infileWest.open(west);
      infileEast.open(east);
      
      
      
      Int_t jj=0;
      while (infileWest >> run >> mpv >> error)
	{
	  WestRunA[jj]=run; 
	  WestMPVA[jj]=mpv;
	  WestErrorA[jj]=error;
	  jj++;
	}
      
      Int_t ii = 0;
      while (infileEast >> run >> mpv >> error)
	{
	  EastRunA[ii]=run; 
	  EastMPVA[ii]=mpv;
	  EastErrorA[ii]=error;
	  
	  cout << ii << endl;
	  ii++;
	}
      infileWest.close();
      infileEast.close();
      
      char temp[100];
      TGraphErrors *East = new TGraphErrors(Eruns, EastRunA, EastMPVA, 0, EastErrorA);
      sprintf(temp, "%s East Side MPV: %s",runtype, t); 
      East->SetTitle(temp);
      East->GetYaxis()->SetRangeUser(1., 3.5);
      East->GetYaxis()->SetTitle("Energy (keV)");
      East->GetXaxis()->SetTitle("Run Number");
      TGraphErrors *West = new TGraphErrors(Wruns, WestRunA, WestMPVA, 0, WestErrorA);
      sprintf(temp, "%s West Side MPV: %s",runtype,t);
      West->SetTitle(temp);
      West->GetYaxis()->SetRangeUser(1., 3.5);
      West->GetYaxis()->SetTitle("Energy (keV)");
      West->GetXaxis()->SetTitle("Run Number");
      TCanvas *c3 = new TCanvas("c3");
      TCanvas *c4 = new TCanvas("c4");
      
      //c1->Divide(2,1);
      c3->cd();
      East->Draw("AP*");
      c4->cd();
      West->Draw("AP*");
    }

}

  
