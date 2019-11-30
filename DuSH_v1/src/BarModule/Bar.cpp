#include <iostream>
#include <BarModule/Bar.hpp>


// ------------------------------------------------------- //
Bar::Bar(float flength, float fwidth, float fthickness, Vector3 fcenter) {
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
    center = Vector3();
    
    // construct faces
    // top and bottom   - in y direction
    face_top    = Face( length, width, fcenter + Vector3( 0 , 0.5 * thickness , 0 ) );
    face_bottom = Face( length, width, fcenter - Vector3( 0 , 0.5 * thickness , 0 ) );
    // left and right   - in x direction
    face_left   = Face( length, thickness, fcenter + Vector3( 0.5 * width , 0 , 0) );
    face_right  = Face( length, thickness, fcenter - Vector3( 0.5 * width , 0 , 0) );
    // front and back   - in z direction
    face_front  = Face( width, thickness, fcenter + Vector3( 0 , 0 , 0.5 * length) );
    face_back   = Face( width, thickness, fcenter - Vector3( 0 , 0 , 0.5 * length) );
    
}

