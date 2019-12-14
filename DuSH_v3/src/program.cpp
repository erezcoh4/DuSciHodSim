//#include <FaceModule/Face.hpp> // ToDo: Delete?
#include <BarModule/Bar.hpp>
#include <PhotonModule/Photon.hpp>
#include "TCanvas.h"
#include "TApplication.h"
#include "TView.h"
#include "Auxiliary/version.hpp"

int main(int argc, char **argv){
    
    version * vObj = new version( argc, argv );
    Int_t verbose = vObj -> GetVerbose();

        
    // defenitions
    // ToDo: Move these to (an XML) configuration file
    // scintillation bar dimensions [mm]
    // double length = 2000, width = 200, thickness = 60;
    // for debug purposes, also use a small cube
    double length = 50, width = 30, thickness = 30;
    double refractiveIndex = 1.5;
    
    // scintillation photon production
    int Nphotons     = 1;
    
    // define App
    TApplication theApp("tapp", &argc, argv);
        
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", width, thickness, length);
    bar -> SetRefractiveIndex(refractiveIndex);
    if (verbose>2) { bar->Print(); }
    
    
    // display
    TCanvas *c = new TCanvas("c", "c",100,100,1000,1000);
    bar -> Draw();
    
        
    
    // (2) shoot a proton onto the paddle
    // T.B.D.
    
    
    
    // (3) produce scintillation photons in paddle and propagate them
    int Npoints = 2;
    for (int photonIdx=1; photonIdx <= Nphotons; photonIdx++ ) {

        if (verbose>1){
            PrintXLine();
            std::cout << "generating photon " << photonIdx << std::endl;
            PrintLine();
        }
        
        Photon * photon = new Photon (Npoints, verbose);
        photon -> EmissionIsotropically( TVector3(0,0,0) );
        photon -> PropagateInPaddle( bar );
        photon -> Draw();
        
        if (verbose>1){
            std::cout << "done photon " << photonIdx << std::endl;
            PrintLine();
        }
    }
    
    // update canvas
    TView *view = c -> GetView();
    view -> ShowAxis();
    c -> Update();
    
    // run programm
    theApp.Run();
    return 0;
}



