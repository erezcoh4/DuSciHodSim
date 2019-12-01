#include <iostream>
#include <FaceModule/Face.hpp>


// ------------------------------------------------------- //
Face::Face() {
    // default constructor
    std::cout << "contracting a face...\n";
    // dimensions [mm]
    side_1 = 1;
    side_2 = 1;
    verbose = 0;
    
    // position [mm]
    center = Vector3();
    corner_1 = Vector3();
    corner_2 = Vector3();
    corner_3 = Vector3();
    corner_4 = Vector3();


    // plane orientation in spherical coordinates
    theta = 0;     // with respect to z-axis
    phi = 0;        // in x-y plane, with respect to x-axis
}

// ------------------------------------------------------- //
Face::Face(float fside_1, float fside_2, Vector3 fcenter, float ftheta, float fphi) {
    verbose=2;
    center = fcenter;
    center.SetPhi(fphi);
    center.SetTheta(ftheta);
    // constructor with dimesions and position
    if (verbose>1){
        std::cout << "contracting a face at " << std::endl;
        center.Print();
    }
        
    side_1  = fside_1;
    side_2  = fside_2;
    theta   = center.GetTheta();
    phi     = center.GetPhi();

    // the orientation of the face (center angles) is perpendicular to the face. Using this fact we can fix the position of the face corners
//    // first define face corners in x-y direction
//    corner_1 = center + (side_1/2) * Vector3( 1 , 0 , 0 ) + (side_2/2) * Vector3( 0 , 1 , 0 );
//    corner_2 = center - (side_1/2) * Vector3( 1 , 0 , 0 ) + (side_2/2) * Vector3( 0 , 1 , 0 );
//    corner_3 = center + (side_1/2) * Vector3( 1 , 0 , 0 ) - (side_2/2) * Vector3( 0 , 1 , 0 );÷
//    corner_4 = center - (side_1/2) * Vector3( 1 , 0 , 0 ) - (side_2/2) * Vector3( 0 , 1 , 0 );
    
//    // now rotate the entire face by theta and phi
//    RotX = (1, 0, 0; 0, cos(phi), -sin(phi); 0 , sin(phi) , cos(phi));
//    RotZ = (cos(phi), -sin(phi), 0; 0 , sin(phi) , cos(phi));
//    corner_1 = RotMatrix * corner_1;
//
//    // last, move the face to the right position
//    corner_1 = corner_1 + center;
//    corner_2 = corner_2 + center;
//    corner_3 = corner_3 + center;
//    corner_4 = corner_4 + center;
}


