#ifndef WaveguideMargin_HPP
#define WaveguideMargin_HPP

/*
 A scintillation WaveguideMargin class
 */

#include <iostream>
#include "TGeoTrd2.h"
#include "TGeoBBox.h"
#include "TVector3.h"

#include <Auxiliary/auxiliary.hpp>

class WaveguideMargin: public TGeoBBox // public TGeoTrd2
{
private:
    
    // dimensions [mm]
    Int_t   verbose;
    // position [mm]
    TVector3 center;
    
    //    double dx1, dx2, dy1, dy2, dz;
    double length, width, thickness;
    double refractiveIndex;
    double TotalInternalReflectionAngle;
    double PhotonAbsorbtionLength;
    
    
    
    
    
public:
    
    std::vector<std::string> facetNames;
    std::vector<TVector3>    facetCenters, facetNormals;
    std::vector<std::vector<TVector3>> facetVertices;
    
    // setters
    void                        SetVerbose (int v)    { verbose = v;};
    void                SetRefractiveIndex (double n) {refractiveIndex = n; SetTotalInternalReflectionAngle();};
    void   SetTotalInternalReflectionAngle ();
    void               SetAbsorbtionLength (double v) {PhotonAbsorbtionLength = v;};

    // getters
    double              GetRefractiveIndex () {return refractiveIndex;};
    double GetTotalInternalReflectionAngle () {return TotalInternalReflectionAngle;}
    double             GetAbsorbtionLength () {return PhotonAbsorbtionLength;};
    std::vector<std::vector<TVector3>> GetFacetVertices () {return facetVertices;};
    std::vector<TVector3>  GetFacetCenters () {return facetCenters;};
    std::vector<TVector3>  GetFacetNormals () {return facetNormals;};
    double                       GetLength () {return length;};
    double                        GetWidth () {return width;};
    double                    GetThickness () {return thickness;};
    TVector3                     GetCenter () {return center;};

    
    
    // geometry
    
    void                      defineFacets ();
    bool               CheckIfPointOnFacet ( int facetIdx, TVector3 pt);
    bool                     ContainsPoint (const TVector3 position);
    
    // constructors
    //    WaveguideMargin (const char *name, Double_t dx1, Double_t dx2, Double_t dy1, Double_t dy2, Double_t dz);
    WaveguideMargin (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
    
    // print
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }

    
};

#endif

