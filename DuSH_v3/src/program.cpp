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
    double length = 30, width = 30, thickness = 30;

    // scintillation photon production
    int Nphotons     = 1;
    
    // define App
    TApplication theApp("tapp", &argc, argv);
        
    // (1) build scintillation bar
    Bar * bar = new Bar("Scintillation bar", width/2, thickness/2, length/2);
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
//        // (3.0) choose photon production location
//        TVector3 photonStartPosition(0,0,0);
//        photon -> SetPoint(1,photonStartPosition.X(),photonStartPosition.Y(),photonStartPosition.Z());
//
//        // (3.1) emit the photon isotropically:
//        // generate a photon direction on the unit sphere
//        r -> Sphere(x, y, z, 1);
//        TVector3 photonDirection( x, y, z );
//        bool photonInBar = true;
//
//        int LastHitFacetIdx = 0;
//        while (photonInBar) {
//
//            photon -> SetTrajectoryDirec( photonDirection );
//            photon -> SetTrajectoryStart( photonStartPosition );
//            if (verbose>2){
//                photon -> PrintTrajectory();
//            }
//
//            // (3.2) Decide if the paddle bounding box could be crossed by a vector.
//            // intersect with paddle facets determines photon end position...
//            TVector3 photonEndPosition(0,0,0);
//
//
//            bool foundIntersectionPoint = false;
//            for (int facetIdx=0; facetIdx<6; facetIdx++){
//
//                if (verbose>2){
//                    std::cout << "LastHitFacetIdx: " << LastHitFacetIdx << ", facetIdx: "  << facetIdx << std::endl;
//                }
//
//                if (foundIntersectionPoint==false && facetIdx!=LastHitFacetIdx ){
//
//                    TVector3 FacetIntersection =
//                    photon -> TrajIntWithPlane ( bar->facetCenters.at(facetIdx), bar->facetNormals.at(facetIdx) );
//
//                    if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
//
//                        if (verbose>3) {
//                            std::cout << "bar->facetCenters.at(" << facetIdx << "): ";
//                            TVector3 v = bar->facetCenters.at(facetIdx);
//                            std::cout << "(" << v.X() << "," << v.Y()<< "," << v.Z() << ")" << std::endl ;
//                        }
//
//                        if (    abs(FacetIntersection.x()) <= width/2
//                            &&  abs(FacetIntersection.y()) <= thickness/2
//                            &&  abs(FacetIntersection.z()) <= length/2 ){
//
//                            if (verbose>4) {
//                                std::cout << bar->facetNames.at(facetIdx) << " facet Intersection at" << std::endl;
//                                FacetIntersection.Print();
//                                std::cout << "flip photon direction, intersection at " << bar->facetNames.at(facetIdx) << std::endl;
//                            }
//
//                            photonEndPosition = FacetIntersection;
//                            foundIntersectionPoint = true;
//                            LastHitFacetIdx = facetIdx;
//
//
//                            // flip photon direction and shift it away from facet
//                            if (bar->facetNames.at(facetIdx) == "Top" || bar->facetNames.at(facetIdx) == "Bottom"){
//                                photonDirection.SetY( -photonDirection.y() );
//
//                                if (bar->facetNames.at(facetIdx) == "Top")
//                                    photonEndPosition.SetY( photonEndPosition.y() - 0.1 );
//                                else if (bar->facetNames.at(facetIdx) == "Bottom")
//                                    photonEndPosition.SetY( photonEndPosition.y() + 0.1 );
//                            }
//                            else if (bar->facetNames.at(facetIdx) == "Left" || bar->facetNames.at(facetIdx) == "Right") {
//                                photonDirection.SetX( -photonDirection.x() );
//
//                                if (bar->facetNames.at(facetIdx) == "Left")
//                                    photonEndPosition.SetX( photonEndPosition.x() - 0.1 );
//                                else if (bar->facetNames.at(facetIdx) == "Right")
//                                    photonEndPosition.SetX( photonEndPosition.x() + 0.1 );
//                            }
//                            else if (bar->facetNames.at(facetIdx) == "Back" || bar->facetNames.at(facetIdx) == "Front") {
//                                photonDirection.SetZ( -photonDirection.z() );
//
//                                if (bar->facetNames.at(facetIdx) == "Back")
//                                    photonEndPosition.SetZ( photonEndPosition.z() + 0.1 );
//                            }
//                            // CONTINUE HERE: GOT STUCK IN AN ENDLESS LOOP....
//
//
//                            // if photon got to the front facet of the scintillation bar
//                            // and intersected with its plane,
//                            // it emerged out of the paddle
//                            // ToDo: add a condition for the photon to emerge out of the
//                            // paddle also if its angle is smaller than the critical angle
//                            // for total internal reflection
//                            std::cout << "at facet "<< bar->facetNames.at(facetIdx) << ", Npoints: "  << Npoints  << std::endl;
//                            if (bar->facetNames.at(facetIdx) == "front" || Npoints >= MAXPHOTONSTEPS){
//                                photonInBar = false;
//                            }
//                        }
//                        else {
//                            if (verbose>4) {
//                                std::cout << "intersection point of photon with plane not in one of the facets" << std::endl;
//                            }
//                        }
//                    }
//                    if (verbose>2){
//                        std::cout << "done stepping through facet " << facetIdx << std::endl;
//                    }
//                }
//            }
//
//            photon -> SetPoint(Npoints,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
//            photonStartPosition = photonEndPosition;
//            Npoints = Npoints + 1;
//            if (verbose>2) {
//                std::cout << "photonEndPosition: ";
//                std::cout
//                << "("
//                << photonEndPosition.X() << "," << photonEndPosition.Y()<< "," << photonEndPosition.Z()
//                << ")"
//                << std::endl ;
//                std::cout << "Npoints: " << Npoints << std::endl;
//                std::cout << "photonInBar: " << photonInBar << std::endl;
//                PrintLine();
//            }
//        }
//        photon -> Draw();
//        if (verbose>1){
//            std::cout << "done photon " << photonIdx << std::endl;
//            PrintLine();
//        }
    }
    
    // update canvas
    TView *view = c -> GetView();
    view -> ShowAxis();
    c -> Update();
    
    // run programm
    theApp.Run();
    return 0;
}



