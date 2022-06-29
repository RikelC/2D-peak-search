/*********************************************************************************
*
* A simple macro for testing 2D peak searching algorithm of root
*
* Author: Rike Chakma
* Date: 15/04/2022
* Caution: be careful with the root version you use. 
* For version 5.xx, use float and for version 6.xx and above use double
*
************************************************************************************/



#include <TSpectrum2.h>
#include "TCanvas.h"
#include "TRandom.h"
#include "TH2.h"
#include "TF2.h"
#include "TMath.h"
#include "TROOT.h"
#include "TCanvas.h"


void test_2D(){
  int nbinsx = 500;
  int nbinsy = 500;
  float xmin = -10.;
  float  xmax = 10.;
  float ymin = -10.;
  float  ymax = 10.;
  //number of peaks
  int nX = 5;
  int nY = 5;
  TCanvas* c = new TCanvas;
  TH2F* h2 = new TH2F("h2", "2D peak search ;x;y",nbinsx,xmin, xmax,nbinsy,ymin,ymax);
  //define gaus function
  TF2 *xyg = new TF2("xyg","xygaus",-10,10,-10,10);
  //fill histogram
  for(int i =0; i <nX;i++){
    float x = -8. + (i +1) * 2.;
    for(int j =0; j <nY;j++){
      float y = -8. + (j +1) * 2.;
      xyg->SetParameters(10,x,0.2,y,0.2); //amplitude, meanx,sigmax,meany,sigmay
      h2->FillRandom("xyg",1000);
    }
  }

  c->cd(); 
  h2->Draw("col"); 
  c->Update();
  //----------------------------------------
  //peak search
  // declare source and des arrays
  float** source = new float*[nbinsx];
  float** dest = new float*[nbinsx];
  for (int i=0;i<nbinsx;i++)
  {
    source[i]=new float[nbinsy];
    dest[i]=new float[nbinsy];
  }
  // initialize
  for (int i = 0; i < nbinsx; i++){
    for (int j = 0; j < nbinsy; j++){
      source[i][j] = h2->GetBinContent(i + 1,j + 1);
    }
  }

  TSpectrum2 *s = new TSpectrum2();
  //sigma = number of bins for 1 sigma
  int nfound = s->SearchHighRes(source, dest, nbinsx, nbinsy, 5., 5, kFALSE, 10, kTRUE, 3);
  printf("Found %d candidate peaks\n",nfound);
  float *xpos = (float*)s->GetPositionX();// in terms of bin number
  float *ypos = (float*)s->GetPositionY();
  float *posX = new float[nfound];
  float *posY = new float[nfound];
  for(int i=0;i<nfound;i++){
    int bx = (int)(xpos[i]+ 0.5);
    int by = (int)(ypos[i]+ 0.5);
    //convert to the true values 
    posX[i] = h2->GetXaxis()->GetBinCenter(bx);
    posY[i] = h2->GetXaxis()->GetBinCenter(by);
    float val = source[bx][by];
    printf("posx= %f, posy= %f, val =%f\n",posX[i],posY[i], val);
  }
  //add polymarker
  TPolyMarker * pm = (TPolyMarker*)h2->GetListOfFunctions()->FindObject("TPolyMarker");
  if (pm) {
    h2->GetListOfFunctions()->Remove(pm);
    delete pm;
  }
   
  pm = new TPolyMarker(nfound, posX, posY);
  h2->GetListOfFunctions()->Add(pm);
  pm->SetMarkerStyle(23);
  pm->SetMarkerColor(kRed);
  pm->SetMarkerSize(1.3);
  //clear memory
  for (int i=0;i<nbinsx;i++)
  {
    delete [] source[i];
    delete [] dest[i];
  }
  delete [] source;
  delete [] dest;
  
  delete[] posX; 
  delete [] posY;
 
  c->cd(); 
  h2->Draw("col");
  c->Update();

}
