#ifndef FACE_HPP
#define FACE_HPP

#include <iostream>
#include "Auxiliary/Vector3.hpp"

class Face
{
private:

    
	// dimensions [mm]
	float side_1, side_2; 

	// position [mm]
	Vector3 center;
	Vector3 corner_1, corner_2, corner_3, corner_4;


	// plane orientation in spherical coordinates
	float theta; 	// with respect to z-axis
	float phi;	// in x-y plane, with respect to x-axis



public:
    
    // verbosity
    int verbose=2;

    // constructors
    Face();
    Face(float fside_1, float fside_2, Vector3 fcenter, float ftheta, float fphi);
    
    // Setters
    void    SetVerbose (int fverbose=0) {verbose = fverbose;};
    void  SetQuietMode () {verbose = 0;};
	


    // Getters
    Vector3     GetCenter () {return center;};
    Vector3    GetCorner1 () {return corner_1;};
    Vector3    GetCorner2 () {return corner_2;};
    Vector3    GetCorner3 () {return corner_3;};
    Vector3    GetCorner4 () {return corner_4;};
	
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

    
    
};

#endif

