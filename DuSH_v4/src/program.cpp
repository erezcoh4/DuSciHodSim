//#include <FaceModule/Face.hpp> // ToDo: Delete?
#include <BarModule/Bar.hpp>
#include <PhotonModule/Photon.hpp>
#include "TCanvas.h"
#include "TApplication.h"
#include "TView.h"
#include "Auxiliary/version.hpp"

int main(int argc, char **argv){
    
    // defintions
    TApplication theApp("tapp", &argc, argv);
    TCanvas * c;
    bool DoDrawScene = false;
    
    version * vObj = new version( argc, argv );
    Int_t verbose = vObj -> GetVerbose();
    if (verbose>0) { DoDrawScene = true; };
    // predetermined scintillation photon production
    Int_t Nphotons = vObj -> GetNphotons();
    
    
    // defenitions
    // ToDo: Move these to (an XML) configuration file
    // scintillation bar dimensions [mm]
    // double length = 2000, width = 200, thickness = 60;
    // for debug purposes, also use a small cube
    double length = 2000, width = 200, thickness = 60;
    double refractiveIndex = vObj -> GetRefractiveIndex();
    
    
    
    
    
    
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", width, thickness, length);
    bar -> SetRefractiveIndex(refractiveIndex);
    if (verbose>2) { bar->Print(); }
    
    
    // open App and drawif verbosity
    if (DoDrawScene) {
        c = new TCanvas("c", "c",100,100,1000,1000);
        bar -> Draw();
    }
    
    
    // (2) shoot a proton onto the paddle
    // T.B.D.
    
    
    
    // (3) produce scintillation photons in paddle and propagate them
    std::string simname = Form("version_%.1f_n_%.2f_emit_from_center",vObj->GetVersion(),bar->GetRefractiveIndex());
    std::string csv_header = "ArrivedAtFrontFacet,ProductionDirection_x,ProductionDirection_y,ProductionDirection_z";
    vObj -> open_photons_csv(simname , csv_header );
    for (int photonIdx=1; photonIdx <= Nphotons; photonIdx++ ) {
        
        if (verbose>1){
            PrintXLine();
            std::cout << "generating photon " << photonIdx << std::endl;
            PrintLine();
        }
        
        Photon * photon = new Photon (2, verbose);
        photon -> SetProductionPosition( TVector3(0,0,0) );
        photon -> EmitIsotropically();
        photon -> PropagateInPaddle( bar );
        
        
        vObj -> write_photons_csv( {
            (double)photon->GetArrivedAtFrontFacet(),
            photon->GetProductionDirection().X(),
            photon->GetProductionDirection().Y(),
            photon->GetProductionDirection().Z()
        } );
        
        
        if (DoDrawScene) { photon -> Draw(); }
        
        if (verbose>1){
            std::cout << "done photon " << photonIdx << std::endl;
            PrintLine();
        }
    }
    
    // update canvas
    if (DoDrawScene) {
        // TView *view = c -> GetView();
        // view -> ShowAxis();
        c -> Update();
        // run programm
        theApp.Run();
    }
    
    PrintLine(); std::cout << "done." << std::endl; PrintLine();
    return 0;
}



