#include <iostream>
#include <PhotonModule/Photon.hpp>
using namespace std;
#define MAXPHOTONSTEPS 1000

// ------------------------------------------------------- //
Photon::Photon (Int_t n, int fverbose):
TPolyLine3D( n ){
    
    Npoints = n;
    verbose = fverbose;
    if (verbose>1){
        std::cout << "built TPolyLine3D" << std::endl;
        std::cout << "fverbose: " << std::endl;
        std::cout << "photon verbosity: " << verbose << std::endl;
    }
    r = new TRandom3(0);
    SetInBar();
    photonArrivedAtFrontFacet = false;
    photonAbsorbedInScintillator = false;
    photonReadOutByDetector = false;
    TotalPathLength = 0;
}


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
    Debug(3, "Photon::TrajIntWithPlane()");
    if (verbose>3){
        
        PrintTVector3(planeCenter);
        PrintTVector3(planeNormal);
        PrintTVector3(trajectoryDirec);
        Debug(4, Form("denom = planeNormal dot trajectoryDirec : %f", denom));
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
        Debug(3, "No intersection point found : (plane || track)");
        return TVector3(-9999,-9999,-9999);
    }
    return TVector3(-9999,-9999,-9999);
}



// ------------------------------------------------------- //
void Photon::PrintTrajectory(){
    std::cout << "photon trajectory: ";
    PrintTVector3(trajectoryStart);
    PrintTVector3(trajectoryDirec);
    TVector3 trajEnd = trajectoryStart + 500 * trajectoryDirec;
    PrintTVector3(trajEnd);
    Debug(2,Form("Total path length: %.1f mm",TotalPathLength));
}

// ------------------------------------------------------- //
void Photon::DrawTrajectory(int trajColor){
    TVector3 trajEnd = trajectoryStart + 500 * trajectoryDirec;
    this -> SetPoint(Npoints,trajEnd.X(),trajEnd.Y(),trajEnd.Z());
    this -> SetLineColor( trajColor );
}
// ------------------------------------------------------- //
void Photon::EmitIsotropically(){
    // emit photon isotropically from emissionPos:
    // generate a photon direction on the unit sphere
    photonStartPosition = ProductionPosition;
    this -> SetPoint(1,photonStartPosition.X(),photonStartPosition.Y(),photonStartPosition.Z());
    r -> Sphere(x, y, z, 1);
    photonDirection = TVector3( x, y, z );
    SetProductionDirection( photonDirection );
    photonEndPosition = photonStartPosition + 5000 * photonDirection;
    photonDirectFromProduction = true;
}

// ------------------------------------------------------- //
bool Photon::PhotonTrajOppositeFacet(std::string facetName){
    if (
        ( facetName == "Back" && PhotonGoingForward)
        ||
        ( facetName == "Front" && PhotonGoingBackward)
        ||
        ( facetName == "Left" && PhotonGoingRight)
        ||
        ( facetName == "Right" && PhotonGoingLeft)
        ||
        ( facetName == "Top" && PhotonGoingDown)
        ||
        ( facetName == "Bottom" && PhotonGoingUp)
        )
    {
        return true;
    }
    return false;
}

// ------------------------------------------------------- //
void Photon::PropagateInPaddle( Bar * bar ){
    Debug(2 , "Photon::PropagateInPaddle()");
    SetInBar();
    int LastHitFacetIdx = -1;
    
    while (photonInBar && (Npoints < MAXPHOTONSTEPS)) { // stop if number of points it too large
        this -> SetTrajectoryDirec( photonDirection );
        this -> SetTrajectoryStart( photonStartPosition );
        if (verbose>2){ PrintTrajectory(); }
        
        // (3.2) Decide if the paddle bounding box could be crossed by a vector.
        // intersect with paddle facets determines photon end position...
        bool foundIntersectionPoint = false;
        for (int facetIdx=0; facetIdx<6; facetIdx++){
            Debug(2 , Form("LastHitFacetIdx: %d, facetIdx: %d", LastHitFacetIdx , facetIdx) );
            
            // avoid from intersecting trajectory with wrong facet - a one which is opposite to photon direction
            if (PhotonTrajOppositeFacet( bar->facetNames.at(facetIdx) )){
                Debug(5, "Photon Trajory Opposite Facet" );
                continue;
            }
            
            
            if (foundIntersectionPoint==false && facetIdx!=LastHitFacetIdx ){
                Debug(3, "Haven't found plane intersection");
                
                TVector3 FacetIntersection =
                this -> TrajIntWithPlane ( bar->facetCenters.at(facetIdx), bar->facetNormals.at(facetIdx) );
                
                if (verbose>3) {PrintTVector3(FacetIntersection);}
                
                if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                    
                    //                    if (verbose>3) { PrintTVector3(bar->facetCenters.at(facetIdx)); }
                    
                    if (    abs(FacetIntersection.x()) <= bar -> GetWidth()/2
                        &&  abs(FacetIntersection.y()) <= bar -> GetThickness()/2
                        &&  abs(FacetIntersection.z()) <= bar -> GetLength()/2 ){
                        
                        Debug(4, "photon met scintillator facet! now apply snell law");
                        
                        // change the flag indicating that the photon did not hit any facet on its way out if the facet it hit is not the front facet where the detector is located...
                        if (bar->facetNames.at(facetIdx) != "Front"){
                            photonDirectFromProduction = false;
                        }
                        
                        photonEndPosition = FacetIntersection;
                        foundIntersectionPoint = true;
                        LastHitFacetIdx = facetIdx;
                        
                        this -> ApplySnellLaw( bar, facetIdx );
                    }
                    else {
                        Debug(4, "intersection point of photon with plane plane not on facet" );
                    }
                }
                if (verbose>2){
                    std::cout << "done stepping through facet " << facetIdx << std::endl;
                    PrintEmptyLine();
                }
            }
        }
        TotalPathLength += (photonEndPosition - photonStartPosition).Mag();
        this -> SetPoint(Npoints,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        photonStartPosition = photonEndPosition;
                
        if (verbose>2) {
            PrintTVector3(photonEndPosition);
            Debug(2, Form("Number of photon steps: %d , photonInBar: %d", Npoints-1, photonInBar) );
            PrintLine();
        }
        Npoints = Npoints + 1;
    }
    
    if (!photonInBar){ // photon emerged from paddle
        SetTrajectoryStart( photonEndPosition );
        SetTrajectoryDirec( photonDirection );
        
        if (verbose>3){
            DrawTrajectory( 4 );
            PrintTrajectory();
            Debug(3,"photon exitted from paddle!");
            PrintLine();
        }
    }
    
    this -> DecideIfAbsorbedInScintillator( bar->GetAbsorbtionLength() );
}
// ------------------------------------------------------- //
void Photon::DecideIfAbsorbedInScintillator( double AbsorbtionLength ){
    // effectively decide if photon is absorbed in scintillator
    // following an 'absorbtion length' decay in the paddle
    // by checking its total path length in the scintillator,
    // and then statistically deciding wether or not it was absorbed
    // by generating a random uniform number, and checking if its smaller then an exponential
    // decay function value at the photon total path length
    photonAbsorbedInScintillator = false;
    
    double random_number = r -> Uniform(0.0,1.0);
    if (random_number > exp( -TotalPathLength/AbsorbtionLength ) ){
        photonAbsorbedInScintillator = true;
        photonReadOutByDetector = false;
    }
        
}

// ------------------------------------------------------- //
void Photon::SetTrajectoryDirec (TVector3 v) {
    trajectoryDirec = v;
    
    PhotonGoingForward = (v.Z()>0) ? true : false;
    PhotonGoingBackward = !PhotonGoingForward;
    
    PhotonGoingUp = (v.Y()>0) ? true : false;
    PhotonGoingDown = !PhotonGoingUp;
    
    PhotonGoingLeft = (v.X()>0) ? true : false;
    PhotonGoingRight = !PhotonGoingLeft;
}

// ------------------------------------------------------- //
double Photon::GetTrajectoryAngleWithPlane(Bar * bar, int facetIdx) {
    // [https://onlinemschool.com/math/library/analytic_geometry/plane_line/]
    //  The direction vector v will meet the plane represented by the equation
    //          ax + by + cz + d = 0
    // with a normal n=(a,b,c) [and a shift of d = -ax0 - by0 - cz0]
    // at an angle given by
    //
    //         phi = | n \cdot v |/ |n|*|v|
    TVector3 v = trajectoryDirec;
    TVector3 n = bar -> facetNormals.at(facetIdx);
    
    double phi = TMath::Pi()/2. - asin((fabs(n.Dot(v)) / (n.Mag() * v.Mag())));
    
    Debug(4,bar->facetNames.at(facetIdx));
    Debug(3, Form("n.Dot(v): %.2f, |n|: %.1f, |n|: %.1f, phi: %.2f",n.Dot(v),n.Mag(),v.Mag(),phi));
    return phi;
}

// ------------------------------------------------------- //
void Photon::DecideIfReadOutByDetector(){
    if (!photonAbsorbedInScintillator)  { // if not absorbed in scintillator, it can be read out
        photonReadOutByDetector = true;
    }
    // ToDo: add surface area covered by detector...
}

// ------------------------------------------------------- //
void Photon::ApplySnellLaw(Bar * bar, int facetIdx){
    // This function performs the main action that happens
    // when the photon meets one of the scintillation bar facet.
    // The photon will either emerge from the bar if its angle
    // is smaller than the critical angle for complete internal reflection,
    // or it will stay in the paddle if the angle is greater than the critical angle,
    // and the trajectory direction will be flipped
    
    Debug(2 , "Photon::ApplySnellLaw()");
    Debug(3 , bar->facetNames.at(facetIdx));
    
    // if photon touched the front facet of the scintillation bar
    // and intersected with its plane,
    // it emerged out of the paddle
    if (bar->facetNames.at(facetIdx) == "Front"){
        photonInBar = false;
        photonArrivedAtFrontFacet = true;
        DecideIfReadOutByDetector();
        return;
    }
    
    // photon emerge out of the
    // paddle also if its angle is smaller than the critical angle
    // for total internal reflection
    double photonTrajectoryAngleWithPlane = this -> GetTrajectoryAngleWithPlane( bar, facetIdx );
    Debug(3 , Form("photon Trajectory Angle With Plane: %.1f deg.", aux.rad2deg(photonTrajectoryAngleWithPlane)));
    
    
    if ( photonTrajectoryAngleWithPlane > bar->GetTotalInternalReflectionAngle() ){ // total internal reflection
        
        double shiftFromFacet = 0.1; // [mm]
        if (bar->facetNames.at(facetIdx) == "Top" || bar->facetNames.at(facetIdx) == "Bottom"){
            photonDirection.SetY( -photonDirection.y() );
            
            if (bar->facetNames.at(facetIdx) == "Top")
                photonEndPosition.SetY( photonEndPosition.y() - shiftFromFacet );
            else if (bar->facetNames.at(facetIdx) == "Bottom")
                photonEndPosition.SetY( photonEndPosition.y() + shiftFromFacet );
        }
        else if (bar->facetNames.at(facetIdx) == "Left" || bar->facetNames.at(facetIdx) == "Right") {
            photonDirection.SetX( -photonDirection.x() );
            
            if (bar->facetNames.at(facetIdx) == "Left")
                photonEndPosition.SetX( photonEndPosition.x() - shiftFromFacet );
            else if (bar->facetNames.at(facetIdx) == "Right")
                photonEndPosition.SetX( photonEndPosition.x() + shiftFromFacet );
        }
        else if (bar->facetNames.at(facetIdx) == "Back" || bar->facetNames.at(facetIdx) == "Front") {
            photonDirection.SetZ( -photonDirection.z() );
            
            if (bar->facetNames.at(facetIdx) == "Back")
                photonEndPosition.SetZ( photonEndPosition.z() + shiftFromFacet );
        }
    }
    else { // photon exit scintillation bar
        this -> ApplySnellDivergence( bar -> facetNormals.at(facetIdx) , bar -> GetRefractiveIndex() );
        photonInBar = false;
        Debug(4, "photon exit scintillation bar angle < TIR" );
        return;
    }
    
}

// ------------------------------------------------------- //
void Photon::ApplySnellDivergence( TVector3 PlaneNormal, double n_in ){
    // change photon direction according to Snell' Law
    // [http://www.starkeffects.com/snells-law-vector.shtml]
    // v_out = (n_in/n_out) (n x (v_in x n)) - n * sqrt( 1 - (n_in/n_out)^2 |v_in x n|^2)
    
    // CONTINUE HERE: Apply Snell' beam direction change and check it...
    
    double n_out = 1; // air
    double n_ratio = n_in / n_out;
    TVector3 v_in = trajectoryDirec; // vector in bar
    TVector3 CrossProduct = v_in.Cross( PlaneNormal );
    
    TVector3 v_out =    n_ratio*( PlaneNormal.Cross(CrossProduct) )
    - PlaneNormal * sqrt(1 - n_ratio*n_ratio*CrossProduct.Mag2());
}
