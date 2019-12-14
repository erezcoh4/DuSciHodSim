#ifndef BAR_HPP
#define BAR_HPP

/*
 A scintillation bar class
 */

#include <iostream>
//#include "Auxiliary/Vector3.hpp"
#include "FaceModule/Face.hpp"
#include "TGeoBBox.h"
#include "TVector3.h"

class Bar: public TGeoBBox
{
private:
    
    //    // dimensions [mm]
    //    double length, width, thickness;
    
    // position [mm]
    TVector3 center;
//    Face    face_top, face_bottom, face_left, face_right, face_front, face_back;
    
    double length, width, thickness;
    double refractiveIndex;
    double TotalInternalReflectionAngle;
    
    
    
    
    
    
public:
    
    std::vector<std::string> facetNames;
    std::vector<TVector3>    facetCenters, facetNormals;
    
    // setters
    void                SetRefractiveIndex (double n) {refractiveIndex = n; SetTotalInternalReflectionAngle();};
    void   SetTotalInternalReflectionAngle ();
    
    // getters
    double                       GetLength () {return length;};
    double                        GetWidth () {return width;};
    double                    GetThickness () {return thickness;};
    double              GetRefrectiveIndex () {return refractiveIndex;};
    double GetTotalInternalReflectionAngle () {return TotalInternalReflectionAngle;}
    
    
    void defineFacets();
    
    // constructors
    Bar(){    length = 0; width = 0; thickness = 0; refractiveIndex = 1; };
    
    Bar (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
 
    

};

#endif

