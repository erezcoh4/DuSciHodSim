#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <iostream>
#include <math.h>

class Vector3
{
private:




public:
    
    double x, y, z;
    double theta;     // with respect to z-axis
    double phi;    // in x-y plane, with respect to x-axis

	// Setters
   	void 	    SetX(double fin) {x = fin;};
   	void 	    SetY(double fin) {y = fin;};
   	void        SetZ(double fin) {z = fin;};
   	void 	  SetPhi(double fin) {theta = fin;};
   	void 	SetTheta(double fin) {phi = fin;};

   	void 	  SetXYZ(double fx,double fy,double fz) {
			x=fx; y=fy; z=fz;
			double r = sqrt(x*x + y*y + z*z);
			theta = acos(z/r);
			phi = atan2(y,x);
		};


	// Getters
  	double 	    GetX() {return x;};
   	double 	    GetY() {return y;};
   	double       GetZ() {return z;};
   	double	  GetPhi() {return theta;};
   	double	GetTheta() {return phi;};
	

    
    
    // Constructor
    Vector3(double fx=0, double fy=0, double fz=0){
        SetXYZ(fx,fy,fz);
    };
    
    
    // operators
    Vector3 operator+(Vector3 v) {
        x = x + v.x;
        y = y + v.y;
        z = z + v.z;
        return *this;
    };
    Vector3 operator-(Vector3 v) {
        x = x - v.x;
        y = y - v.y;
        z = z - v.z;
        return *this;
    };

            
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

