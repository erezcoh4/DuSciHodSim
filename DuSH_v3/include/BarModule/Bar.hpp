#ifndef BAR_HPP
#define BAR_HPP

/*
 A scintillation bar class
 */

#include <iostream>
//#include "Auxiliary/Vector3.hpp"
#include "FaceModule/Face.hpp"
#include "TGeoBBox.h"

class Bar: public TGeoBBox
{
private:

//    // dimensions [mm]
//    double length, width, thickness;
    
	// position [mm]
	TVector3 center;
    Face    face_top, face_bottom, face_left, face_right, face_front, face_back;
    
    



public:

    // constructors
    Bar(){};
//    Bar(double flength=1, double fwidth=1, double fthickness=1, TVector3 fcenter=TVector3());
    Bar (const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);

//    void Print();
};

#endif

