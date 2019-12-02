#ifndef FACE_HPP
#define FACE_HPP

#include <iostream>
#include "Auxiliary/Vector3.hpp"
#include "TVector3.h"

class Face
{
private:

    
	// dimensions [mm]
	float side_1, side_2; 

	// position [mm]
	TVector3 center;
	TVector3 corner_1, corner_2, corner_3, corner_4;


	// plane orientation in spherical coordinates
	float theta; 	// with respect to z-axis
	float phi;	// in x-y plane, with respect to x-axis



public:
    
    // verbosity
    int verbose;

    // constructors
    Face();
    Face(float fside_1, float fside_2, Vector3 fcenter, float ftheta, float fphi);
    
    // Setters
    void    SetVerbose (int fverbose=0) {verbose = fverbose;};
    void  SetQuietMode () {verbose = 0;};
    void        SetPhi (float fin) {theta = fin;};
    void      SetTheta (float fin) {phi = fin;};



    // Getters
    TVector3     GetCenter () {return center;};
    TVector3    GetCorner1 () {return corner_1;};
    TVector3   GetCorner2 () {return corner_2;};
    TVector3   GetCorner3 () {return corner_3;};
    TVector3   GetCorner4 () {return corner_4;};
	
    float          GetC1x () {return corner_1.x;};
    float          GetC1y () {return corner_1.y;};
    float          GetC1z () {return corner_1.z;};
    float          GetC2x () {return corner_2.x;};
    float          GetC2y () {return corner_2.y;};
    float          GetC2z () {return corner_2.z;};
    float          GetC3x () {return corner_3.x;};
    float          GetC3y () {return corner_3.y;};
    float          GetC3z () {return corner_3.z;};
    float          GetC4x () {return corner_4.x;};
    float          GetC4y () {return corner_4.y;};
    float          GetC4z () {return corner_4.z;};

    float          GetPhi () {return theta;};
    float        GetTheta () {return phi;};
    

    
};

#endif

