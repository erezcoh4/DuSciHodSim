#include <iostream>
#include <BarModule/Bar.hpp>
#include "TMath.h"
#define PiRad 3.141528


// ------------------------------------------------------- //
Bar::Bar(double flength, double fwidth, double fthickness, TVector3 fcenter) {
    // standard constructor
    // z direction: length
    // x direction: width
    // y direction: thickness
    // for simplicity, we start (November 2019) by always assuming that the bar center is located at the origin (0,0,0)
    
    std::cout << "contracting a scintillation bar...\n";
    
    // dimensions [mm]
    length      = flength;
    width       = fwidth;
    thickness   = fthickness;

    // position [mm]
    center = TVector3();
    
    // construct faces
    // Face( side_1, side_2, center, theta, phi)
    
    // top and bottom   - in y direction
    face_top = Face( length, width, fcenter + TVector3( 0 , 0.5 * thickness , 0 ) , 0 , 0);
    face_bottom = Face( length, width, fcenter - TVector3( 0 , 0.5 * thickness , 0 ) , PiRad, 0);
    
    // left and right   - in x direction
    face_left = Face( length, thickness, fcenter + TVector3( 0.5 * width , 0 , 0) , 0 , PiRad/2);
    face_right = Face( length, thickness, fcenter - TVector3( 0.5 * width , 0 , 0) , 0 , -PiRad/2);
    
    // front and back   - in z direction
    face_front = Face( width, thickness, fcenter + TVector3( 0 , 0 , 0.5 * length) , PiRad/2 , 0 );
    face_back = Face( width, thickness, fcenter - TVector3( 0 , 0 , 0.5 * length) , -PiRad/2 , 0 );
    
}

