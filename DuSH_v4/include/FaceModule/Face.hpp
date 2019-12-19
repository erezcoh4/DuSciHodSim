#ifndef FACE_HPP
#define FACE_HPP

#include "TVector3.h"
#include <iostream>
//#include "Auxiliary/Vector3.hpp"


class Face
{
private:

    
	// dimensions [mm]
	double side_1, side_2;

	// position [mm]
	TVector3 center;
	TVector3 corner_1, corner_2, corner_3, corner_4;


	// plane orientation in spherical coordinates
	double theta; 	// with respect to z-axis
	double phi;	// in x-y plane, with respect to x-axis



public:
    
    // verbosity
    int verbose;

    // constructors
    Face(double fside_1=1, double fside_2=1, TVector3 fcenter=TVector3(), double ftheta=0, double fphi=0);
    
    // Setters
    void    SetVerbose (int fverbose=0) {verbose = fverbose;};
    void  SetQuietMode () {verbose = 0;};
    void        SetPhi (double fin) {theta = fin;};
    void      SetTheta (double fin) {phi = fin;};



    // Getters
    TVector3    GetCenter () {return center;};
    TVector3   GetCorner1 () {return corner_1;};
    TVector3   GetCorner2 () {return corner_2;};
    TVector3   GetCorner3 () {return corner_3;};
    TVector3   GetCorner4 () {return corner_4;};
	
    double          GetC1x () {return corner_1.x();};
    double          GetC1y () {return corner_1.y();};
    double          GetC1z () {return corner_1.z();};
    double          GetC2x () {return corner_2.x();};
    double          GetC2y () {return corner_2.y();};
    double          GetC2z () {return corner_2.z();};
    double          GetC3x () {return corner_3.x();};
    double          GetC3y () {return corner_3.y();};
    double          GetC3z () {return corner_3.z();};
    double          GetC4x () {return corner_4.x();};
    double          GetC4y () {return corner_4.y();};
    double          GetC4z () {return corner_4.z();};

    double          GetPhi () {return theta;};
    double        GetTheta () {return phi;};
    

    
};

#endif

