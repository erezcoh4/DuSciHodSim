#ifndef PROTON_HPP
#define PROTON_HPP

/*
 A proton class
 generated Dec-21, 2019
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
#include <Auxiliary/auxiliary.hpp>
#include "TGraph.h"
#include "TSpline.h"
#include "TCanvas.h"


class Proton: public TPolyLine3D
{
private:
    
    bool        protonInBar; // is the proton inside a scintillation bar?
    bool        DoDrawScene;
    bool        DoProduceScintillationPhotons;
    bool        debugMode = false;
    Int_t       Npoints, verbose, NScintillationPhotons;
    int         ShowEveryNPhotons;
    TVector3    ProductionPosition, ProductionDirection, EndPosition;
    double      Mp = 938.272;   // MeV
    double      Ep;             // MeV
    double      Pp;             // MeV/c
    double      KE;             // MeV
    double      time;           // time since the proton entered scintillator, in [sec]
    TRandom3 * r;
    Double_t x, y, z;
    Int_t Nsteps;
    TGraph      *dEdx_vs_KE_proton;
    TSpline3    *dEdx_vs_KE_proton_s3;
    TGraph      *MeVee_vs_MeV_proton;
    TSpline3    *MeVee_vs_MeV_proton_s3;

    
public:
    
    
    
    
    // constructors
    Proton(double ftime, Int_t fverbose=0);
    void            GenerateEdepCurves ();
    void        GenerateMeV2MeVeeCurve ();
    
    // setters
    void            SetTrajectoryDirec (TVector3 v);
    void                    SetVerbose (int v)      { verbose = v;};
    void                      SetInBar ()           { protonInBar = true; }
    void                 SetOutsideBar ()           { protonInBar = false; }
    void         SetProductionPosition (TVector3 v) { ProductionPosition = v; }
    void        SetProductionDirection (TVector3 v) { ProductionDirection = v.Unit(); }
    void             SetEnergyMomentum ();
    void                     SetEnergy (double fE)  { Ep = fE; SetEnergyMomentum();};
    void                SetDoDrawScene (bool fDo)   { DoDrawScene = fDo;};
    void             SetProducePhotons (bool fDo)   { DoProduceScintillationPhotons = fDo;};
    void       AddScintillationPhotons (Int_t N)    {   NScintillationPhotons += N;
                                                        Debug(2,Form("NScintillationPhotons: %d",NScintillationPhotons));};
    void        AddScintillationPhoton ()           {AddScintillationPhotons(1);};
    void          SetShowEveryNPhotons (int N)      {ShowEveryNPhotons = N;};
    
    // getters
    TVector3     GetProductionPosition () { return ProductionPosition; }
    TVector3    GetProductionDirection () { return ProductionDirection; }
    double                     GetEdep ( double dx_cm = 0.1 );
    Int_t     GetNScintillationPhotons () { return NScintillationPhotons; }
    
    // propagation
    void                         Shoot (Bar * bar, auxiliary * aux,
                                        bool fdebugMode=false,
                                        Waveguide * waveguide = nullptr,
                                        WaveguideMargin * wgmargin = nullptr);
    
    void   ProduceScintillationPhotons (Bar * bar, auxiliary * aux, int Nphotons=0,
                                        TVector3 EmissionPos = TVector3(0,0,0),
                                        Waveguide * waveguide = nullptr,
                                        WaveguideMargin * wgmargin = nullptr);
    
    void            UpdateProtonEnergy ( double dE );
        
    // time
    void                      TickTime ( double dx , double n=1 );
    
    // print
    void               PrintTrajectory ();
    void                DrawTrajectory (int trajColor=2);
    void                         Debug (Int_t verobosity_level, std::string text)
    { if ( verbose > verobosity_level ) std::cout << text << std::endl; };
};

#endif

