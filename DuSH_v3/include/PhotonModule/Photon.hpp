#ifndef PHOTON_HPP
#define PHOTON_HPP

/*
 A photon class
 generated Dec-7, 2019
 last edit Dec-7, 2019
 
 We use geometrical optics, and so a photon will be charecterised as a 3D polynomial line of segments, and hence inherit from TPolyLine3D. Each segement is started from last position and ends either at infinity or at an elastic scattering in one of the scintillation bar faces
 */

#include <iostream>
#include "TVector3.h"
#include "TPolyLine3D.h"

class Photon: public TPolyLine3D
{
private:

    
    Int_t       Npoints;
    TVector3    trajectoryStart;
    TVector3    trajectoryDirec;

public:

    
    
    
    // constructors
    Photon(){};
    Photon(Int_t n, Option_t *option="");
    Photon(Int_t n, Double_t *p, Option_t *option="");

    
    // setters
    void SetTrajectoryStart (TVector3 v) { trajectoryStart = v;};
    void SetTrajectoryDirec (TVector3 v) { trajectoryDirec = v;};

    // getters
    TVector3 GetTrajectoryStart () { return trajectoryStart;};
    TVector3 GetTrajectoryDirec () { return trajectoryDirec;};

    
    // geometry
    TVector3 findIntersectionLinePlane(const TVector3 &pos, const TVector3 &dir,
                                       const TVector3 &planeCenter, const  TVector3 &planeNormal);

    
    TVector3 TrajIntWithPlane (const TVector3 &planeCenter, const  TVector3 &planeNormal);
    
    // print
    void PrintTrajectory();
    
};

#endif

