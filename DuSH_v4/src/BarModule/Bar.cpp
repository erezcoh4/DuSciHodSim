#include <iostream>
#include <BarModule/Bar.hpp>
#include "TMath.h"
#define PiRad 3.141528

// ------------------------------------------------------- //
//Bar(char *name, double flength=1, double fwidth=1, double fthickness=1, TVector3 fcenter=TVector3()){
Bar::Bar (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin):
TGeoBBox( name, dx/2, dy/2, dz/2, origin ){
    std::cout << "built TGeoBBox" << std::endl;
    length = dz;
    width = dx;
    thickness = dy;
    defineFacets();
//    refractiveIndex = 1;
//    SetTotalInternalReflectionAngle();
}

// ------------------------------------------------------- //
void Bar::defineFacets(){
    
  facetNames = {
       "Top",  "Bottom" ,
       "Left", "Right" ,
       "Back", "Front"};
   
   facetCenters = {
       TVector3(0,thickness/2,0),   TVector3(0,-thickness/2,0),
       TVector3(width/2,0,0),       TVector3(-width/2,0,0),
       TVector3(0,0,-length/2),     TVector3(0,0,length/2)};
   
   facetNormals = {
       TVector3(0,1,0),    TVector3(0,-1,0),
       TVector3(1,0,0),    TVector3(-1,0,0),
       TVector3(0,0,-1),    TVector3(0,0,1)
   };
}

// ------------------------------------------------------- //
void Bar::SetTotalInternalReflectionAngle(){
    // with respect to air
    TotalInternalReflectionAngle = asin( 1.0 / refractiveIndex );
    std::cout << "refractiveIndex: " << refractiveIndex << std::endl;
    std::cout << "TotalInternalReflectionAngle: " << TotalInternalReflectionAngle << std::endl;
}

