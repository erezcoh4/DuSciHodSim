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

    // constructor
    Bar();
    


};

#endif

