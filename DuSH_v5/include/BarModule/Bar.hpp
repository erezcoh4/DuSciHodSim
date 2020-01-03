#ifndef BAR_HPP
#define BAR_HPP

/*
 A scintillation bar class
 */

#include <iostream>
#include "TGeoBBox.h"
#include "TVector3.h"
#include <Auxiliary/auxiliary.hpp>

class Bar: public TGeoBBox
{
private:
    
    //    // dimensions [mm]
    //    double length, width, thickness;
    Int_t   verbose;
    // position [mm]
    TVector3 center;
    //    Face    face_top, face_bottom, face_left, face_right, face_front, face_back;
    
    double length, width, thickness;
    double refractiveIndex;
    double TotalInternalReflectionAngle;
    double PhotonsPerMeV;
    double PhotonAbsorbtionLength;
    
    
    
    
    
public:
    
    std::vector<std::string> facetNames;
    std::vector<TVector3>    facetCenters, facetNormals;
    
    // setters
    void                        SetVerbose (int v)    { verbose = v;};
    void                SetRefractiveIndex (double n) {refractiveIndex = n; SetTotalInternalReflectionAngle();};
    void   SetTotalInternalReflectionAngle ();
    void                  SetPhotonsPerMeV (double v) {PhotonsPerMeV = v;}
    void               SetAbsorbtionLength (double v) {PhotonAbsorbtionLength = v;};

    // getters
    double                       GetLength () {return length;};
    double                        GetWidth () {return width;};
    double                    GetThickness () {return thickness;};
    double              GetRefractiveIndex () {return refractiveIndex;};
    double GetTotalInternalReflectionAngle () {return TotalInternalReflectionAngle;}
    double                GetPhotonsPerMeV () {return PhotonsPerMeV;}
    double             GetAbsorbtionLength () {return PhotonAbsorbtionLength;};
    // geometry
    void                      defineFacets ();
    bool                     ContainsPoint (const TVector3 position);
    
    // constructors
    Bar(){    length = 0; width = 0; thickness = 0; refractiveIndex = 1; PhotonsPerMeV = 10;};
    Bar (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
    
    
    
    
};

#endif

