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
#include <WaveguideModule/Waveguide.hpp>
#include <WaveguideMarginModule/WaveguideMargin.hpp>
#include "Auxiliary/version.hpp"
#include "Auxiliary/auxiliary.hpp"



class Photon: public TPolyLine3D
{
private:
    
    auxiliary   aux;
    
    bool        photonInBar; // is the photon inside a scintillation bar?
    bool        photonInWaveguide; // is the photon inside the waveguide?
    bool        photonInWaveguideMargin;
    
    bool        photonHitBackFacet; // did photon hit back facet of scintillator (we might place another detector there so crucial)
    bool        photonArrivedAtFrontFacet; // can the photon be read out by SiPM if they're coupled with no waveguide?
    bool        photonArrivedAtWaveguideExit; // can the photon be read out by SiPM?
    bool        photonArrivedAtWaveguideMarginExit; // can the photon be read out by SiPM?
    
    bool        photonAbsorbedInScintillator; // statistically, following 'absorbtion length' decay
    bool        photonAbsorbedInWaveguide;
    bool        photonAbsorbedInWaveguideMargin;

    
    bool        photonReadOutByDetector; // statistically, following 'absorbtion length' decay
    bool        photonDirectFromProduction; // has the photon hit any facet, or got to the end directly from production?
    
    
    Int_t       Npoints, verbose;
    
    double      TotalPathLength, TotalPathLengthInScintillator, TotalPathLengthInWaveguide, TotalPathLengthInWaveguideMargin;
    
//    double      vScintillator_mm_sec, vWaveguide_mm_sec;
    double      velocity_mm_sec;  // in [mm/sec]
    double      ProductionTime;
    double      TimeFromStart; // in [sec]
    
    TVector3    ProductionPosition, ProductionDirection;
    TVector3    photonStartPosition, photonEndPosition, photonDirection;
    TVector3    trajectoryStart;
    TVector3    trajectoryDirec;
    
    TVector3    HitFrontFacetPos;
    TVector3    HitWaveguideExitPos;
    TVector3    HitWaveguideMarginExitPos;

    TRandom3 * r;
    Double_t x, y, z;
    
    std::vector<TVector3> PathPoints;
    
    bool        PhotonGoingForward, PhotonGoingBackward, PhotonGoingLeft, PhotonGoingRight, PhotonGoingUp, PhotonGoingDown;
    
public:
    
    
    
    
    // constructors
    Photon(){verbose=0;};
    Photon(Int_t n, Int_t fverbose=0);
    
    
    // setters
    void            SetTrajectoryStart (TVector3 v) { trajectoryStart = v;};
    void            SetTrajectoryDirec (TVector3 v);
    void                    SetVerbose (int v)      { verbose = v;};
    void                   SetVelocity (double n)   { velocity_mm_sec = 3.e11/n;} // in [mm/sec]
    void                      SetInBar (double n)   { photonInBar = true; SetVelocity(n); }
    void                 SetOutsideBar ()           { photonInBar = false; SetVelocity(1); }
    void                SetInWaveguide (double n)   { photonInWaveguide = true; SetVelocity(n); }
    void           SetOutsideWaveguide ()           { photonInWaveguide = false; SetVelocity(1); }
    void         SetProductionPosition (TVector3 v) { ProductionPosition = v; }
    void        SetProductionDirection (TVector3 v) { ProductionDirection = v; }

    
    // getters
    bool                  GetArrivedAtFrontFacet () { return photonArrivedAtFrontFacet;};
    bool               GetArrivedAtWaveguideExit () { return photonArrivedAtWaveguideExit;};
    bool         GetArrivedAtWaveguideMarginExit () { return photonArrivedAtWaveguideMarginExit;};
    
    bool               GetAbsorbedInScintillator () { return photonAbsorbedInScintillator;};
    bool                  GetAbsorbedInWaveguide () { return photonAbsorbedInWaveguide;};
    bool            GetAbsorbedInWaveguideMargin () { return photonAbsorbedInWaveguideMargin;};
    
    bool                    GetReadOutByDetector () { return photonReadOutByDetector;};
    bool                 GetDirectFromProduction () { return photonDirectFromProduction; }
    bool                         GetHitBackFacet () { return photonHitBackFacet;    }
    
    double      GetTotalPathLengthInScintillator () { return TotalPathLengthInScintillator; }
    double         GetTotalPathLengthInWaveguide () { return TotalPathLengthInWaveguide; }
    double   GetTotalPathLengthInWaveguideMargin () { return TotalPathLengthInWaveguideMargin; }
    double                    GetTotalPathLength () { return TotalPathLength; }
    double                      GetTimeFromStart ();
    
    TVector3                  GetTrajectoryStart () { return trajectoryStart;};
    TVector3                  GetTrajectoryDirec () { return trajectoryDirec;};
    TVector3               GetProductionPosition () { return ProductionPosition; };
    TVector3              GetProductionDirection () { return ProductionDirection; };
    TVector3                 GetHitFrontFacetPos () { return HitFrontFacetPos; };
    TVector3              GetHitWaveguideExitPos () { return HitWaveguideExitPos; };
    TVector3        GetHitWaveguideMarginExitPos () { return HitWaveguideMarginExitPos; };

    // geometry
    TVector3                    TrajIntWithPlane (const TVector3 planeCenter, const  TVector3 planeNormal);
    
    // propagation
    bool                 PhotonTrajOppositeFacet (std::string facetName);
    double           GetTrajectoryAngleWithPlane (TVector3 facetNormal, std::string facetName = "");
    void                       EmitIsotropically (double fProductionTime, Bar * bar );
    void                       PropagateInPaddle (Bar * bar);
    void                           ApplySnellLaw (Bar * bar, int facetIdx);
    void                           ApplySnellLaw (Waveguide * waveguide, int facetIdx);
    void                           ApplySnellLaw (WaveguideMargin * wgmargin, int facetIdx);
    void                    ApplySnellDivergence (TVector3 PlaneNormal,  double n_in);
    
    void          DecideIfAbsorbedInScintillator (double AbsorbtionLength );
    void             DecideIfAbsorbedInWaveguide (double AbsorbtionLength );
    void       DecideIfAbsorbedInWaveguideMargin (double AbsorbtionLength );
    
    void               DecideIfReadOutByDetector ();
    void                      ArriveAtFrontFacet (); // what happens when a photon arrives at the front facet
    void                    PropagateInWaveguide (Waveguide * waveguide);
    void              PropagateInWaveguideMargin (WaveguideMargin * wgmargin);
    void                   ArriveAtWaveguideExit ();
    void             ArriveAtWaveguideMarginExit ();
    void                     flipPhotonDirection (std::string facetName = "Top");
    
    // print and draw
    void          PrintTrajectory (std::string name="");
    void                PrintPath (std::string name="");
    void           DrawTrajectory ();
    void                    Debug (Int_t verobosity_level, std::string text)
    {
        if ( verbose > verobosity_level+2 ) {std::cout <<  "\x1B[35m" << text << "\x1B[0m" << std::endl; return;}
        if ( verbose > verobosity_level+1 ) {std::cout <<  "\x1B[34m" << text << "\x1B[0m" << std::endl; return;}
        if ( verbose > verobosity_level )   {std::cout <<  "\x1B[33m" << text << "\x1B[0m" << std::endl; return;}
    };
};

#endif

