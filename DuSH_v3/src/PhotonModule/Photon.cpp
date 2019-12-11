#include <iostream>
#include <PhotonModule/Photon.hpp>
using namespace std;
#define MAXPHOTONSTEPS 5

// ------------------------------------------------------- //
Photon::Photon (Int_t n, int fverbose):
TPolyLine3D( n ){
    std::cout << "built TPolyLine3D" << std::endl;
    Npoints = n;
    verbose = fverbose;
    std::cout << "fverbose: " << std::endl;
    std::cout << "photon verbosity: " << verbose << std::endl;
    r = new TRandom3();
}

//// ------------------------------------------------------- //
//Photon::Photon (Int_t n, Double_t *p, Option_t *option):
//TPolyLine3D( n, p, option ){
//    std::cout << "built TPolyLine3D" << std::endl;
//    Npoints = n;
//    verbose = 0;
//    r = new TRandom3();
//} delete by Dec-20


//// ------------------------------------------------------- // delete by Dec-20
//TVector3 Photon::findIntersectionLinePlane(
//                        const TVector3 &pos, const TVector3 &dir,
//                        const TVector3 &planeCenter, const  TVector3 &planeNormal) {
//    // Finds the intersection point of a straight line with any plane.
//    // input
//    // pos:            a point on the straight line.
//    // dir:            direction of the straight line.
//    // planeCenter:    a point on the plane.
//    // planeNormal:    normal vector of the plane.
//    //
//    // return
//    // pointIntersect: the intersection point (return value).
//
//    Double_t denom = planeNormal.Dot(dir);
//    if (denom != 0.0) {
//        Double_t t = ((planeCenter.x() - pos.x()) * planeNormal.x() +
//              (planeCenter.y() - pos.y()) * planeNormal.y() +
//              (planeCenter.z() - pos.z()) * planeNormal.z()) / denom;
//
//        return pos + (t * dir);
//    } else {
//    ::Warning("Photon::findIntersectionLinePlane()", "No intersection point found : (plane || track)");
//        return TVector3(-9999,-9999,-9999);
//    }
//    return TVector3(-9999,-9999,-9999);
//}
//

// ------------------------------------------------------- //
TVector3 Photon::TrajIntWithPlane(
                                  const TVector3 planeCenter, const  TVector3 planeNormal) {
    // Finds the intersection point of the current photon trajectory with any plane.
    //
    // based on TVector3 Photon::findIntersectionLinePlane()
    // and using the current trajectory start point and the direction
    //
    // input
    // planeCenter:    a point on the plane.
    // planeNormal:    normal vector of the plane.
    //
    // return
    // pointIntersect: the intersection point (return value).
    
    Double_t denom = planeNormal.Dot( trajectoryDirec );
    
    if (verbose>3){
        std::cout << "Photon::TrajIntWithPlane(), "
        << "denom: " << denom
        << ". planeCenter: " << "(" << planeCenter.X() << "," << planeCenter.Y() << "," << planeCenter.Z() << "), "
        << std::endl;
    }
    
    
    if (denom != 0.0) {
        Double_t t = (
                      (planeCenter.x() - trajectoryStart.x()) * planeNormal.x() +
                      (planeCenter.y() - trajectoryStart.y()) * planeNormal.y() +
                      (planeCenter.z() - trajectoryStart.z()) * planeNormal.z()
                      )
        / denom;
        
        return trajectoryStart + (t * trajectoryDirec);
    } else {
        ::Warning("Photon::TrajIntWithPlane()", "No intersection point found : (plane || track)");
        return TVector3(-9999,-9999,-9999);
    }
    return TVector3(-9999,-9999,-9999);
}



// ------------------------------------------------------- //
void Photon::PrintTrajectory(){
    std::cout << "photon trajectory: "
    << std::endl
    << "origin: (" << trajectoryStart.X() << "," << trajectoryStart.Y() << "," << trajectoryStart.Z() << "), "
    << "direction: (" << trajectoryDirec.X() << "," << trajectoryDirec.Y() << "," << trajectoryDirec.Z() << "), "
    << std::endl;
}



// ------------------------------------------------------- //
void Photon::EmissionIsotropically( TVector3 emissionPos ){
    // emit photon isotropically from emissionPos:
    // generate a photon direction on the unit sphere
    photonStartPosition = emissionPos;
    this -> SetPoint(1,photonStartPosition.X(),photonStartPosition.Y(),photonStartPosition.Z());
    r -> Sphere(x, y, z, 1);
    photonDirection = TVector3( x, y, z );
    photonEndPosition = photonStartPosition + 1000 * photonDirection;
}

// ------------------------------------------------------- //
void Photon::PropagateInPaddle( Bar * bar ){
        
    bool photonInBar = true;
    int LastHitFacetIdx = 0;
     
    while (photonInBar && (Npoints < MAXPHOTONSTEPS)) { // stop if number of points it too large
        this -> SetTrajectoryDirec( photonDirection );
        this -> SetTrajectoryStart( photonStartPosition );
        if (verbose>2){
            this -> PrintTrajectory();
        }
        
        // (3.2) Decide if the paddle bounding box could be crossed by a vector.
        // intersect with paddle facets determines photon end position...
        bool foundIntersectionPoint = false;
        for (int facetIdx=0; facetIdx<6; facetIdx++){
            
            if (verbose>2){
                std::cout << "LastHitFacetIdx: " << LastHitFacetIdx << ", facetIdx: "  << facetIdx << std::endl;
            }
            
            if (foundIntersectionPoint==false && facetIdx!=LastHitFacetIdx ){
                
                TVector3 FacetIntersection =
                this -> TrajIntWithPlane ( bar->facetCenters.at(facetIdx), bar->facetNormals.at(facetIdx) );
                
                if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                    
                    if (verbose>3) {
                        std::cout << "bar->facetCenters.at(" << facetIdx << "): ";
                        TVector3 v = bar->facetCenters.at(facetIdx);
                        std::cout << "(" << v.X() << "," << v.Y()<< "," << v.Z() << ")" << std::endl ;
                    }
                    
                    if (    abs(FacetIntersection.x()) <= bar -> GetWidth()/2
                        &&  abs(FacetIntersection.y()) <= bar -> GetThickness()/2
                        &&  abs(FacetIntersection.z()) <= bar -> GetLength()/2 ){
                        
                        if (verbose>4) {
                            std::cout << bar->facetNames.at(facetIdx) << " facet Intersection at" << std::endl;
                            FacetIntersection.Print();
                            std::cout << "flip photon direction, intersection at " << bar->facetNames.at(facetIdx) << std::endl;
                        }
                        
                        photonEndPosition = FacetIntersection;
                        foundIntersectionPoint = true;
                        LastHitFacetIdx = facetIdx;
                        
                        
                        // flip photon direction and shift it away from facet
                        if (bar->facetNames.at(facetIdx) == "Top" || bar->facetNames.at(facetIdx) == "Bottom"){
                            photonDirection.SetY( -photonDirection.y() );
                            
                            if (bar->facetNames.at(facetIdx) == "Top")
                                photonEndPosition.SetY( photonEndPosition.y() - 0.1 );
                            else if (bar->facetNames.at(facetIdx) == "Bottom")
                                photonEndPosition.SetY( photonEndPosition.y() + 0.1 );
                        }
                        else if (bar->facetNames.at(facetIdx) == "Left" || bar->facetNames.at(facetIdx) == "Right") {
                            photonDirection.SetX( -photonDirection.x() );
                            
                            if (bar->facetNames.at(facetIdx) == "Left")
                                photonEndPosition.SetX( photonEndPosition.x() - 0.1 );
                            else if (bar->facetNames.at(facetIdx) == "Right")
                                photonEndPosition.SetX( photonEndPosition.x() + 0.1 );
                        }
                        else if (bar->facetNames.at(facetIdx) == "Back" || bar->facetNames.at(facetIdx) == "Front") {
                            photonDirection.SetZ( -photonDirection.z() );
                            
                            if (bar->facetNames.at(facetIdx) == "Back")
                                photonEndPosition.SetZ( photonEndPosition.z() + 0.1 );
                        }
                        // CONTINUE HERE: GOT STUCK IN AN ENDLESS LOOP....
                        
                        
                        // if photon got to the front facet of the scintillation bar
                        // and intersected with its plane,
                        // it emerged out of the paddle
                        // ToDo: add a condition for the photon to emerge out of the
                        // paddle also if its angle is smaller than the critical angle
                        // for total internal reflection
                        std::cout << "at facet "<< bar->facetNames.at(facetIdx) << ", Npoints: "  << Npoints  << std::endl;
                        if (bar->facetNames.at(facetIdx) == "Front" || Npoints >= MAXPHOTONSTEPS){
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
                    std::cout << "done stepping through facet " << facetIdx << std::endl;
                }
            }
        }
        
        this -> SetPoint(Npoints,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        photonStartPosition = photonEndPosition;
        Npoints = Npoints + 1;
        
        
        
        if (verbose>2) {
            std::cout << "photonEndPosition: ";
            std::cout
            << "("
            << photonEndPosition.X() << "," << photonEndPosition.Y()<< "," << photonEndPosition.Z()
            << ")"
            << std::endl ;
            std::cout << "Npoints: " << Npoints << ", MAXPHOTONSTEPS: " << MAXPHOTONSTEPS << std::endl;
            std::cout << "photonInBar: " << photonInBar << std::endl;
            PrintLine();
        }
    }
    
}
