#include <FaceModule/Face.hpp>
#include <BarModule/Bar.hpp>
#include "TCanvas.h"
#include "TApplication.h"



int main(int argc, char **argv){
    
    // defenitions
    // ToDo: Move these to (an XML) configuration file
    // scintillation bar dimensions [mm]
    double length    = 2000;
    double width     = 200;
    double thickness = 60;
    // scintillation photon production
    int Nphotons     = 1;

    
    
    TApplication theApp("tapp", &argc, argv);
    
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", length/2, width/2, thickness/2);
    bar->Print();
    
    // display
    TCanvas *c = new TCanvas("c", "c",0,0,600,600);
    bar -> Draw();
    c -> Update();
    
        
    
    // (2) shoot a proton onto the paddle
    
    // (3) produce scintillation photons in paddle
    for (int photonIdx=1; photonIdx <= Nphotons; photonIdx++ ) {
        Photon * photon = new Photon( sprintf() );
    }
    
    // run programm
    theApp.Run();
   	return 0;
}
