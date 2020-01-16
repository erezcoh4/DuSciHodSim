#include <iostream>
#include <WaveguideModule/Waveguide.hpp>
#include "TMath.h"
#define PiRad 3.141528

// ------------------------------------------------------- //
Waveguide::Waveguide (const char *name, Double_t fdx1, Double_t fdx2, Double_t fdy1, Double_t fdy2, Double_t fdz):
TGeoTrd2 (name, fdx1, fdx2, fdy1, fdy2, fdz){
    std::cout << "constructed scintillation Waveguide" << std::endl;
    dz = fdz;
    dx1 = fdx1;
    dx2 = fdx2;
    dy1 = fdy1;
    dy2 = fdy2;
    defineFacets();
}

// ------------------------------------------------------- //
void Waveguide::defineFacets(){
// need to go over this and change these!
  facetNames = {
       "Top",  "Bottom" ,
       "Left", "Right" ,
       "Back", "Front"};
   
//   facetCenters = {
//       TVector3(0,thickness/2,0),   TVector3(0,-thickness/2,0),
//       TVector3(width/2,0,0),       TVector3(-width/2,0,0),
//       TVector3(0,0,-length/2),     TVector3(0,0,length/2)};
//
//   facetNormals = {
//       TVector3(0,1,0),    TVector3(0,-1,0),
//       TVector3(1,0,0),    TVector3(-1,0,0),
//       TVector3(0,0,-1),    TVector3(0,0,1)
//   };
}

// ------------------------------------------------------- //
void Waveguide::SetTotalInternalReflectionAngle(){
    // with respect to air
    TotalInternalReflectionAngle = asin( 1.0 / refractiveIndex );
    Debug(0, Form("refractive index: %.1f" , refractiveIndex ));
    Debug(0, Form("TIR angle: %.2f deg." , TotalInternalReflectionAngle*180./PiRad ));
}

