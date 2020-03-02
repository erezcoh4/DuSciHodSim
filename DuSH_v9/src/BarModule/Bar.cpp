#include <iostream>
#include <BarModule/Bar.hpp>
#include "TMath.h"
#define PiRad 3.141528

// ------------------------------------------------------- //
Bar::Bar (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin):
TGeoBBox( name, dx/2, dy/2, dz/2, origin ){
    std::cout << "constructed scintillation Bar" << std::endl;
    length = dz;
    width = dx;
    thickness = dy;
    center = TVector3(origin[0],origin[1],origin[2]);
    defineFacets();
}

// ------------------------------------------------------- //
void Bar::defineFacets(){
    
    facetNames = {
        "Top",  "Bottom" ,
        "Left", "Right" ,
        "Back", "Front"};
    
    facetCenters = {
        TVector3(0,thickness/2,0) + center,   TVector3(0,-thickness/2,0) + center,
        TVector3(width/2,0,0) + center,       TVector3(-width/2,0,0) + center,
        TVector3(0,0,-length/2) + center,     TVector3(0,0,length/2) + center};
    
    facetNormals = {
        TVector3(0,1,0),    TVector3(0,-1,0),
        TVector3(1,0,0),    TVector3(-1,0,0),
        TVector3(0,0,-1),    TVector3(0,0,1)
    };
    
    if (verbose>2){
        Debug(-1, "Bar::defineFacets()");
        for (auto facetCenter : facetCenters){
            PrintTVector3( facetCenter );
        }
        for (auto facetNormal : facetNormals){
            PrintTVector3( facetNormal );
        }
    }
}

// ------------------------------------------------------- //
void Bar::SetTotalInternalReflectionAngle(){
    // with respect to air
    TotalInternalReflectionAngle = asin( 1.0 / refractiveIndex );
    Debug(0, Form("refractive index: %.1f" , refractiveIndex ));
    Debug(0, Form("TIR angle: %.2f deg." , TotalInternalReflectionAngle*180./PiRad ));
}

// ------------------------------------------------------- //
bool Bar::ContainsPoint(const TVector3 position) {
    if (TMath::Abs(position.X() - center.X()) > width/2.)    return false;
    if (TMath::Abs(position.Y() - center.Y()) > thickness/2.)return false;
    if (TMath::Abs(position.Z() - center.Z()) > length/2.)   return false;
    return true;
}
