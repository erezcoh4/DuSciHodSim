#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <iostream>
#include <math.h>

class Vector3
{
private:

	float x, y, z;
	float theta; 	// with respect to z-axis
	float phi;	// in x-y plane, with respect to x-axis



public:

	// Setters
   	void 	    SetX(float fin) {x = fin;};
   	void 	    SetY(float fin) {y = fin;};
   	void        SetZ(float fin) {z = fin;};
   	void 	  SetPhi(float fin) {theta = fin;};
   	void 	SetTheta(float fin) {phi = fin;};

   	void 	  SetXYZ(float fx,float fy,float fz) {
			x=fx; y=fy; z=fz;
			float r = sqrt(x*x + y*y + z*z);
			theta = acos(z/r);
			phi = atan2(y,x);
		};


	// Getters
  	float 	    GetX() {return x;};
   	float 	    GetY() {return y;};
   	float       GetZ() {return z;};
   	float	  GetPhi() {return theta;};
   	float	GetTheta() {return phi;};
	

    
    
    // Constructor
    Vector3(){
        SetXYZ(0,0,0);
    }
    
    
    
            
	// Print
	void 	   Print() {
		std::cout <<
		"(x,y,z): (" 
		<< x << "," << y << "," << z 
		<< ")" 
		<< std::endl
		<<
		"(theta,phi): (" 
		<< theta << "," << phi
		<< ")" 
		;
	}
	

};


#endif

