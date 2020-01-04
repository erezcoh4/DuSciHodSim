#include <BarModule/Bar.hpp>
#include <PhotonModule/Photon.hpp>
#include <ProtonModule/Proton.hpp>
#include "TCanvas.h"
#include "TApplication.h"
#include "TView.h"
#include "Auxiliary/version.hpp"
#include <Auxiliary/auxiliary.hpp>

int main(int argc, char **argv){
    
    
    // start app and draw canvas
    TApplication theApp("tapp", &argc, argv);
    TCanvas * c;
    bool DoDrawScene = false;     // this will be run over if verbosity > 0
    bool DoProduceScintillationPhotons = true;
    version * vObj = new version( argc, argv );
    Int_t verbose = vObj -> GetVerbose();

    // simulation name
    std::string simname = Form("version_%.1f_typical",vObj->GetVersion());
    // Form("version_%.1f_n_%.2f_emit_from_center",vObj->GetVersion(),bar->GetRefractiveIndex());

    // read input
    auxiliary * aux = new auxiliary();
    aux->SetVerbose(verbose);
    aux->readXMLinput(simname);
    
    if (verbose>0) { DoDrawScene = true; std::cout << "Drawing Scene..." << std::endl;};
    // predetermined scintillation photon production
    //    Int_t Nphotons = vObj -> GetNphotons();
    
    // defenitions
    // ToDo: Move these to (an XML) configuration file
    // scintillation bar dimensions [mm]
    // double length = 2000, width = 200, thickness = 60;
    // for debug purposes, also use a small cube
    Int_t Nprotons = 1;
//    double length = 2000, width = 200, thickness = 60;
//    TVector3 ProtonGunPosition = TVector3(0,-100,0);
//    TVector3 ProtonGunDirection = TVector3(0,1,0);
//    double   ProtonGunEnergy = 2000; // [MeV]
    
//    double refractiveIndex = vObj -> GetRefractiveIndex();
    // scintillator
    double length = aux->length, width = aux->width, thickness = aux->width;
    double refractiveIndex = aux->refractiveIndex;
    double PhotonsPerMeV = aux->PhotonsPerMeV;
    double PhotonsAbsorbtionLength = aux->AbsorbtionLength;
    
    // proton gun
    TVector3 ProtonGunPosition = aux->pGunPosition;
    TVector3 ProtonGunDirection = aux->pGunDirection;
    double   ProtonGunEnergy = aux->pGunEnergy; // [MeV]
    
    
    
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", width, thickness, length);
    bar -> SetRefractiveIndex(refractiveIndex);
//    double PhotonsPerMeV = vObj -> GetPhotonsPerMeV();
    bar -> SetPhotonsPerMeV(PhotonsPerMeV);
//    double PhotonsAbsorbtionLength = vObj -> GetAbsorbtionLength();
    bar -> SetAbsorbtionLength(PhotonsAbsorbtionLength);
    bar -> SetVerbose(verbose);
    if (verbose>2) { bar->Print(); }

    
    
    
    // open App and draw if verbosity is desired
    if (DoDrawScene) {
        c = new TCanvas("c", "c",100,100,1000,1000);
        bar -> Draw();
    }
    
            
    
    // open output csv files
    std::string csv_header =    "ArrivedAtFrontFacet,DirectFromProduction,AbsorbedInScintillator,ReadOutByDetector,ProductionDirectionX,ProductionDirectionY,ProductionDirectionZ,TotalPathLength";
    aux -> open_photons_csv(simname , csv_header );
    
    
    
    // (2) shoot protons onto the paddle
    // create protons
    for (int protonIdx=1; protonIdx <= Nprotons; protonIdx++ ) {
        
        if (verbose>1){
            PrintXLine();
            std::cout << "generating proton " << protonIdx << std::endl;
            PrintLine();
        }
        
        Proton * proton = new Proton (2, verbose );
        proton -> SetShowEveryNPhotons( aux->ShowEveryNPhotons );
        proton -> SetProducePhotons( DoProduceScintillationPhotons );
        proton -> SetDoDrawScene( DoDrawScene );
        proton -> SetProductionPosition( ProtonGunPosition );
        proton -> SetProductionDirection( ProtonGunDirection );
        proton -> SetEnergy( ProtonGunEnergy );
        proton -> Shoot( bar , aux );
                
        
        if (verbose>1){
            std::cout << "done proton " << protonIdx << std::endl;
            PrintLine();
        }
    }
    
    // update canvas
    if (DoDrawScene) {
        //        TView *view = c -> GetView();
        //        view -> ShowAxis();
        c -> Update();
        // run programm
        theApp.Run();
    }
    
    PrintLine(); std::cout << "done." << std::endl; PrintLine();
    return 0;
}



