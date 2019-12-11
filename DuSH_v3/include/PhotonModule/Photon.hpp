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
#include "TRandom3.h"
#include <BarModule/Bar.hpp>
#include "Auxiliary/version.hpp"



class Photon: public TPolyLine3D
{
private:

    
    Int_t       Npoints, verbose;
    TVector3    photonStartPosition, photonEndPosition, photonDirection;
    TVector3    trajectoryStart;
    TVector3    trajectoryDirec;

    TRandom3 * r;
    Double_t x, y, z;

public:

    
    
    
    // constructors
    Photon(){verbose=0;};
    Photon(Int_t n, Int_t fverbose=0);
    //    Photon(Int_t n, Double_t *p, Option_t *option=""); delete by Dec-20

    
    // setters
    void SetTrajectoryStart (TVector3 v) { trajectoryStart = v;};
    void SetTrajectoryDirec (TVector3 v) { trajectoryDirec = v;};
    void         SetVerbose (int v)      { verbose = v;};

    // getters
    TVector3 GetTrajectoryStart () { return trajectoryStart;};
    TVector3 GetTrajectoryDirec () { return trajectoryDirec;};

    
    // geometry
//    TVector3 findIntersectionLinePlane(const TVector3 &pos, const TVector3 &dir,
//                                       const TVector3 &planeCenter, const  TVector3 &planeNormal);

    
    TVector3 TrajIntWithPlane (const TVector3 planeCenter, const  TVector3 planeNormal);
    
    // propagation
    void    EmissionIsotropically (TVector3 emissionPos);
    void        PropagateInPaddle (Bar * bar);
    
    // print
    void          PrintTrajectory ();
    
};

#endif

