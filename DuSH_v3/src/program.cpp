#include <FaceModule/Face.hpp> // ToDo: Delete?
#include <BarModule/Bar.hpp>
#include <PhotonModule/Photon.hpp>
#include "TCanvas.h"
#include "TApplication.h"
#include "TRandom3.h"
#include "TView.h"
#define MAXPHOTONSTEPS 3

int main(int argc, char **argv){
    
    int verbose = 0;
    if (argc>1) {
        verbose = atoi(argv[1]);
        std::cout << "verbose: " << verbose << std::endl;
    }
    
    // defenitions
    // ToDo: Move these to (an XML) configuration file
    // scintillation bar dimensions [mm]
    double length    = 2000;
    double width     = 200;
    double thickness = 60;
    // scintillation photon production
    int Nphotons     = 1;
    TRandom3 * r = new TRandom3();
    Double_t x, y, z;
    
    // define App
//    TApplication theApp("tapp", &argc, argv);
    
    
    // (1) build scintillation bar
    double dx = width, dy = thickness, dz = length;
    Bar * bar = new Bar("Scintillation bar", dx/2, dy/2, dz/2);
    bar->Print();
    
    // display
    TCanvas *c = new TCanvas("c", "c",100,100,1000,1000);
    bar -> Draw();
    
        
    
    // (2) shoot a proton onto the paddle
    // T.B.D.
    
    
    
    // (3) produce scintillation photons in paddle and propagate them
    int Npoints = 2;
    
    for (int photonIdx=1; photonIdx <= Nphotons; photonIdx++ ) {
        if (verbose>1){
            std::cout << "------------------------------------------------" << std::endl;
            std::cout << "generating photon " << photonIdx << std::endl;
        }
        Photon * photon = new Photon (Npoints);
        
        // (3.0) choose photon production location
        TVector3 photonStartPosition(0,0,0);
        photon -> SetPoint(1,photonStartPosition.X(),photonStartPosition.Y(),photonStartPosition.Z());

        // (3.1) emit the photon isotropically:
        // generate a photon direction on the unit sphere
        r -> Sphere(x, y, z, 1);
        TVector3 photonDirection( x, y, z );
        bool photonInBar = true;
        
        while (photonInBar) {
            
            photon -> SetTrajectoryDirec( photonDirection );
            photon -> SetTrajectoryStart( photonStartPosition );
            if (verbose>2){
                photon -> PrintTrajectory();
            }
            
            // (3.2) Decide if the paddle bounding box could be crossed by a vector.
            // intersect with paddle facets determines photon end position...
            TVector3 photonEndPosition(0,0,0);
            
            
            bool foundIntersectionPoint = false;
            for (int facetIdx=1; facetIdx<=6; facetIdx++){
                if (foundIntersectionPoint==false){
                    
                    TVector3 FacetIntersection =
                    photon -> TrajIntWithPlane ( bar->facetCenters.at(facetIdx), bar->facetNormals.at(facetIdx) );
                    
                    if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                        
                        if (verbose>3) {
                            std::cout << "bar->facetCenters.at(" << facetIdx << ").Print():" << std::endl;
                            bar->facetCenters.at(facetIdx).Print();
                        }
                        
                        if (    abs(FacetIntersection.x()) <= dx/2
                            &&  abs(FacetIntersection.y()) <= dy/2
                            &&  abs(FacetIntersection.z()) <= dz/2 ){
                            
                            if (verbose>4) {
                                std::cout << bar->facetNames.at(facetIdx) << " facet Intersection at" << std::endl;
                                FacetIntersection.Print();
                                std::cout << "flip photon direction, intersection at " << bar->facetNames.at(facetIdx) << std::endl;
                            }
                                                        
                            photonEndPosition = FacetIntersection;
                            foundIntersectionPoint = true;
                            //                continue;

                            // flip photon direction
                            if (bar->facetNames.at(facetIdx) == "top" || bar->facetNames.at(facetIdx) == "bottom"){
                                photonDirection.SetY( -photonDirection.y() );
                            }
                            else if (bar->facetNames.at(facetIdx) == "left" || bar->facetNames.at(facetIdx) == "right") {
                                photonDirection.SetX( -photonDirection.x() );
                            }
                            else if (bar->facetNames.at(facetIdx) == "back" || bar->facetNames.at(facetIdx) == "front") {
                                photonDirection.SetZ( -photonDirection.z() );
                            }
                            // CONTINUE HERE: GOT STUCK IN AN ENDLESS LOOP....


                            // if photon got to the front facet of the scintillation bar
                            // and intersected with its plane,
                            // it emerged out of the paddle
                            // ToDo: add a condition for the photon to emerge out of the
                            // paddle also if its angle is smaller than the critical angle
                            // for total internal reflection
                            std::cout << "bar->facetNames.at(facetIdx) , Npoints:"  << std::endl;
                            std::cout << bar->facetNames.at(facetIdx) << "," << Npoints  << std::endl;
                            if (bar->facetNames.at(facetIdx) == "front" || Npoints >= MAXPHOTONSTEPS){
                                photonInBar = false;
                            }
                        }
                        else {
                            if (verbose>4) {
                                std::cout << "intersection point of photon with plane not in one of the facets" << std::endl;
                            }
                        }
                    }
                    if (verbose>2){
                        std::cout << "done stepping through facet " << facetIdx << std::endl << std::endl;
                    }
                }
            }
            
            photon -> SetPoint(Npoints,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
            photonStartPosition = photonEndPosition;
            Npoints = Npoints + 1;
            if (verbose>2) {
                std::cout << "photonEndPosition" << std::endl;
                photonEndPosition.Print();
                std::cout << "Npoints: " << Npoints << std::endl;
                std::cout << "photonInBar: " << photonInBar << std::endl;
            }
        }
        photon -> Draw();
        if (verbose>1){
            std::cout << "done photon " << photonIdx << std::endl;
            std::cout << "------------------------------------------------" << std::endl;
        }
    }
    
    // update canvas
    TView *view = c -> GetView();
    view -> ShowAxis();
    c -> Update();
    
    // run programm
//    theApp.Run();
    std::cout << argc << strlen(argv[0]) << std::endl;
    return 0;
}



