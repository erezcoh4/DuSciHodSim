#ifndef PHOTON_HPP
#define PHOTON_HPP

/*
 A photon class
 generated Dec-7, 2019
 last edit Dec-21, 2019
 
 We use geometrical optics, and so a photon will be charecterised as a 3D polynomial line of segments, and hence inherit from TPolyLine3D. Each segement is started from last position and ends either at infinity or at an elastic scattering in one of the scintillation bar faces
 */

#include <iostream>
#include "TVector3.h"
#include "TPolyLine3D.h"
#include "TRandom3.h"
#include <BarModule/Bar.hpp>
#include "Auxiliary/version.hpp"
#include "Auxiliary/auxiliary.hpp"



class Photon: public TPolyLine3D
{
private:
    
    auxiliary   aux;
    
    bool        photonInBar; // is the photon inside a scintillation bar?
    bool        photonArrivedAtFrontFacet; // can the photon be read out by SiPM?
    bool        photonAbsorbedInScintillator; // statistically, following 'absorbtion length' decay
    bool        photonReadOutByDetector; // statistically, following 'absorbtion length' decay
    bool        photonDirectFromProduction; // has the photon hit any facet, or got to the end directly from production?
    
    Int_t       Npoints, verbose;
    
    double      TotalPathLength;
    double      v_mm_sec;
    double      ProductionTime, ExitTime;
    
    TVector3    ProductionPosition, ProductionDirection;
    TVector3    photonStartPosition, photonEndPosition, photonDirection;
    TVector3    trajectoryStart;
    TVector3    trajectoryDirec;
    TVector3    HitFrontFacetPos;
    
    TRandom3 * r;
    Double_t x, y, z;
    
    bool        PhotonGoingForward, PhotonGoingBackward, PhotonGoingLeft, PhotonGoingRight, PhotonGoingUp, PhotonGoingDown;
    
public:
    
    
    
    
    // constructors
    Photon(){verbose=0;};
    Photon(Int_t n, Int_t fverbose=0);
    //    Photon(Int_t n, Double_t *p, Option_t *option=""); delete by Dec-20
    
    
    // setters
    void            SetTrajectoryStart (TVector3 v) { trajectoryStart = v;};
    void            SetTrajectoryDirec (TVector3 v);
    void                    SetVerbose (int v)      { verbose = v;};
    void                      SetInBar (double n)   { photonInBar = true; v_mm_sec = 3.e11/n;}
    void                 SetOutsideBar ()           { photonInBar = false; }
    void         SetProductionPosition (TVector3 v) { ProductionPosition = v; }
    void        SetProductionDirection (TVector3 v) { ProductionDirection = v; }

    
    // getters
    bool        GetArrivedAtFrontFacet () { return photonArrivedAtFrontFacet;};
    bool     GetAbsorbedInScintillator () { return photonAbsorbedInScintillator;};
    bool          GetReadOutByDetector () { return photonReadOutByDetector;};
    bool       GetDirectFromProduction () { return photonDirectFromProduction; }
    double          GetTotalPathLength () { return TotalPathLength; }
    double            GetTimeFromStart ();
    TVector3        GetTrajectoryStart () { return trajectoryStart;};
    TVector3        GetTrajectoryDirec () { return trajectoryDirec;};  
    TVector3     GetProductionPosition () { return ProductionPosition; };
    TVector3    GetProductionDirection () { return ProductionDirection; };
    TVector3       GetHitFrontFacetPos () { return HitFrontFacetPos; };
    
    // geometry
    TVector3           TrajIntWithPlane (const TVector3 planeCenter, const  TVector3 planeNormal);
    
    // propagation
    bool        PhotonTrajOppositeFacet (std::string facetName);
    double  GetTrajectoryAngleWithPlane (Bar * bar, int facetIdx);
    void              EmitIsotropically (double fProductionTime);
    void              PropagateInPaddle (Bar * bar);
    void                  ApplySnellLaw (Bar * bar, int facetIdx);
    void           ApplySnellDivergence (TVector3 PlaneNormal,  double n_in);
    void DecideIfAbsorbedInScintillator ( double AbsorbtionLength );
    void      DecideIfReadOutByDetector ();
    void             ArriveAtFrontFacet (); // what happens when a photon arrives at the front facet
    
    // print and draw
    void          PrintTrajectory ();
    void           DrawTrajectory ();
    void                    Debug (Int_t verobosity_level, std::string text)
    { if ( verbose > verobosity_level ) std::cout << text << std::endl; };
};

#endif

