#include <BarModule/Bar.hpp>
#include <WaveguideModule/Waveguide.hpp>
#include <PhotonModule/Photon.hpp>
#include <ProtonModule/Proton.hpp>
#include "TCanvas.h"
#include "TApplication.h"
#include "TView.h"
#include "Auxiliary/version.hpp"
#include <Auxiliary/auxiliary.hpp>

int main(int argc, char **argv){
    
    Int_t Nprotons = 0*1;
    double time = 0; // count the time since the proton entered the scintillator

    // start app and draw canvas
    TApplication theApp("tapp", &argc, argv);
    TCanvas * c;
    bool DoDrawScene = false;     // this will be run over if verbosity > 0
    bool DoProduceScintillationPhotons = true;
    version * vObj = new version( argc, argv );
    Int_t verbose = vObj -> GetVerbose();

    // simulation name
    std::string simname = Form("version_%.1f_%s",vObj->GetVersion(),vObj->GetInputName().c_str());
    // Form("version_%.1f_n_%.2f_emit_from_center",vObj->GetVersion(),bar->GetRefractiveIndex());

    // read input
    auxiliary * aux = new auxiliary();
    aux->SetVerbose(verbose);
    int IsXMLinput = aux->readXMLinput( vObj->GetInputName() );
    if (IsXMLinput==0){
        return 0;
    }
    if (verbose>0) { DoDrawScene = true; std::cout << "Drawing Scene..." << std::endl;};
    
    
    // proton gun
    TVector3 ProtonGunPosition = aux->pGunPosition;
    TVector3 ProtonGunDirection = aux->pGunDirection;
    double   ProtonGunEnergy = aux->pGunEnergy; // [MeV]
    PrintTVector3( ProtonGunPosition );
    PrintTVector3( ProtonGunDirection );
    
    
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", aux->width, aux->thickness, aux->length);
    bar -> SetRefractiveIndex(aux->refractiveIndex);
    bar -> SetPhotonsPerMeV(aux->PhotonsPerMeV);
    bar -> SetAbsorbtionLength(aux->AbsorbtionLength);
    bar -> SetVerbose(verbose);
    if (verbose>2) { bar->Print(); }

    // (1.5) build waveguide
    Waveguide * waveguide = new Waveguide("Waveguide",
                                          aux->width/2, aux->WaveguideWidth/2,
                                          aux->thickness/2, aux->WaveguideWidth/2,
                                          aux->WaveguideLength);
    
//    waveguide -> SetAbsorbtionLength(aux->AbsorbtionLength);
//    waveguide -> SetRefractiveIndex(aux->refractiveIndex);
    waveguide -> SetVerbose(verbose);

    // open App and draw if verbosity is desired
    if (DoDrawScene) {
        c = new TCanvas("c", "c",100,100,1000,1000);
        bar -> Draw();
        waveguide -> Draw("same");
        
    }
    
    
    // open output csv files
    std::string csv_header =    "ArrivedAtFrontFacet,DirectFromProduction,AbsorbedInScintillator,ReadOutByDetector,ProductionDirectionX,ProductionDirectionY,ProductionDirectionZ,TotalPathLength,TimeFromStart,HitFrontFacetPosX,HitFrontFacetPosY,HitFrontFacetPosZ";
    aux -> open_photons_csv(simname , csv_header );
    
    
    
    // (2) shoot protons onto the paddle
    // create protons
    for (int protonIdx=1; protonIdx <= Nprotons; protonIdx++ ) {
        
        if (verbose>1){
            PrintXLine();
            std::cout << "generating proton " << protonIdx << std::endl;
            PrintLine();
        }
        
        Proton * proton = new Proton (time, verbose );        
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



