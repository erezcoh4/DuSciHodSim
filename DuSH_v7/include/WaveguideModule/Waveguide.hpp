#ifndef WAVEGUIDE_HPP
#define WAVEGUIDE_HPP

/*
 A scintillation Waveguide class
 */

#include <iostream>
#include "TGeoTrd2.h"
#include "TVector3.h"
#include <Auxiliary/auxiliary.hpp>

// TGeoTrd2 (Double_t dx1, Double_t dx2, Double_t dy1, Double_t dy2, Double_t dz)
class Waveguide: public TGeoTrd2
{
private:
    
    // dimensions [mm]
    Int_t   verbose;
    // position [mm]
    TVector3 center;
    
    double dx1, dx2, dy1, dy2, dz;
    double refractiveIndex;
    double TotalInternalReflectionAngle;
    double PhotonAbsorbtionLength;
    
    
    
    
    
public:
    
    std::vector<std::string> facetNames;
    std::vector<TVector3>    facetCenters, facetNormals;
    
    // setters
    void                        SetVerbose (int v)    { verbose = v;};
    void                SetRefractiveIndex (double n) {refractiveIndex = n; SetTotalInternalReflectionAngle();};
    void   SetTotalInternalReflectionAngle ();
    void               SetAbsorbtionLength (double v) {PhotonAbsorbtionLength = v;};

    // getters
    double              GetRefractiveIndex () {return refractiveIndex;};
    double GetTotalInternalReflectionAngle () {return TotalInternalReflectionAngle;}
    double             GetAbsorbtionLength () {return PhotonAbsorbtionLength;};
    // geometry
    void                      defineFacets ();
    
    // constructors
    Waveguide (const char *name, Double_t dx1, Double_t dx2, Double_t dy1, Double_t dy2, Double_t dz);
    
    // print
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }

    
};

#endif

