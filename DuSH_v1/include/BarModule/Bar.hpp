#ifndef BAR_HPP
#define BAR_HPP

/*
 A scintillation bar class
 */

#include <iostream>
#include "Auxiliary/Vector3.hpp"
#include "FaceModule/Face.hpp"

class Bar
{
private:

    // dimensions [mm]
    float length, width, thickness;
    
	// position [mm]
	Vector3 center;
    Face    face_top, face_bottom, face_left, face_right, face_front, face_back;
    
    



public:

    // constructors
    Bar(){};
    Bar(float flength=1, float fwidth=1, float fthickness=1, Vector3 fcenter=Vector3());
    


};

#endif

