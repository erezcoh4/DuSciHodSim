#include <iostream>
#include <PhotonModule/Photon.hpp>
using namespace std;
#define MAXPHOTONSTEPS 10000

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
    // set a new seed every time
    r->SetSeed(r->Integer(100000));
    SetOutsideBar(); // lets start with not forcing it inside the scintillator and then put it inside when its emitted
    
    
    photonAbsorbedInScintillator = false;
    photonAbsorbedInWaveguide = false;
    photonHitBackFacet = false;
    photonArrivedAtFrontFacet = false;
    photonArrivedAtWaveguideExit = false;

    photonReadOutByDetector = false;
        
    
    TotalPathLength = 0;
    TotalPathLengthInScintillator = 0;
    TotalPathLengthInWaveguide = 0;
    HitFrontFacetPos            = TVector3(-999,-999,-999);
    HitWaveguideExitPos         = TVector3(-999,-999,-999);
    HitWaveguideMarginExitPos   = TVector3(-999,-999,-999);
    this -> SetLineColor( 1 );
}


// ------------------------------------------------------- //
TVector3 Photon::TrajIntWithPlane(
                                  const TVector3 planeCenter, const  TVector3 planeNormal) {
    // Finds the intersection point of the current photon trajectory with any plane.
    //
    // based on previous version Photon::findIntersectionLinePlane()
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
void Photon::PrintTrajectory(std::string name){
    std::cout << "photon trajectory: " << std::endl;
    std::cout << name << " "; PrintTVector3(trajectoryStart);
    std::cout << name << " "; PrintTVector3(trajectoryDirec);
    TVector3 trajEnd = trajectoryStart + 500 * trajectoryDirec;
    PrintTVector3(trajEnd);
    Debug(2,Form("Total path length: %.1f mm",TotalPathLength));
}

// ------------------------------------------------------- //
void Photon::DrawTrajectory(){
    PrintTrajectory("final (draw)");
    TVector3 trajEnd = trajectoryStart + 500 * trajectoryDirec;
    
}
// ------------------------------------------------------- //
void Photon::EmitIsotropically(double fProductionTime, Bar * bar){
    Debug(1,"Photon::EmitIsotropically()");
    // emit photon isotropically from emissionPos:
    // generate a photon direction on the unit sphere
    
    // production position
    photonStartPosition = ProductionPosition;
    Npoints = 1;
    this -> SetPoint(Npoints-1,photonStartPosition.X(),photonStartPosition.Y(),photonStartPosition.Z());
    PathPoints.push_back( photonStartPosition );
    if (bar->ContainsPoint (photonStartPosition) ){
        SetInBar( bar->GetRefractiveIndex() ) ;
    } else {
        SetOutsideBar();
    }
    
    // prodiction direction
    r -> Sphere(x, y, z, 1);
    photonDirection = TVector3( x, y, z );
    SetProductionDirection( photonDirection );
    photonEndPosition = photonStartPosition + 5000 * photonDirection;
    photonDirectFromProduction = true;
    
    // production time
    ProductionTime = fProductionTime;
//    ExitTime = 0;
    TimeFromStart = ProductionTime;
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
    SetInBar(bar->GetRefractiveIndex());
    
    int LastHitFacetIdx = -1;
    
    while (photonInBar && (Npoints < MAXPHOTONSTEPS)) { // stop if number of points it too large
        this -> SetTrajectoryDirec( photonDirection );
        this -> SetTrajectoryStart( photonStartPosition );
        if (verbose>2){ Debug(1,"photon In Bar"); PrintTrajectory("expected"); }
        
        // (3.2) Decide if the paddle bounding box could be crossed by a vector.
        // intersect with paddle facets determines photon end position...
        bool foundIntersectionPoint = false;
        for (int facetIdx=0; facetIdx<6; facetIdx++){
            
            if (foundIntersectionPoint) continue;
            std::string fName = bar->facetNames.at(facetIdx).c_str();
            
            Debug(2 , Form("looking for intersection at *%s* (LastHitFacetIdx %d)", fName.c_str() ,LastHitFacetIdx) );
            
            // avoid from intersecting trajectory with wrong facet - a one which is opposite to photon direction
            if (PhotonTrajOppositeFacet( fName )){
                Debug(2, "Photon Trajory Opposite Facet, continuing to next facet \n" );
                continue;
            }
            
            if (facetIdx!=LastHitFacetIdx ){
                Debug(3, "Yet haven't found, and looking for plane intersection: ");
                
                TVector3 FacetIntersection =
                this -> TrajIntWithPlane ( bar->facetCenters.at(facetIdx), bar->facetNormals.at(facetIdx) );
                
                if (verbose>3) {PrintTVector3(FacetIntersection);}
                
                if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                                        
                    
                    if (    abs(FacetIntersection.x() - bar->GetCenter().x()) <= bar -> GetWidth()/2     + 0.1
                        &&  abs(FacetIntersection.y() - bar->GetCenter().y()) <= bar -> GetThickness()/2 + 0.1
                        &&  abs(FacetIntersection.z() - bar->GetCenter().z()) <= bar -> GetLength()/2    + 0.1){
                        
                        Debug(4, "photon met scintillator facet. apply snell law");
                        
                        // if the photon hit any facet on its way (except the front facet)
                        // it is no longer a "direct photon" that was emitted from the proton directly
                        // to the detector with a short path
                        // so change the flag indicating that the photon did
                        // not hit any facet on its way out if the facet it hit
                        // is not the front facet where the detector is located...
                        // once this flag is changed once, the photon will forever be falgged as non-direct
                        if (bar->facetNames.at(facetIdx) != "Front"){
                            photonDirectFromProduction = false;
                        }
                        if (bar->facetNames.at(facetIdx) == "Back"){
                            photonHitBackFacet = true;
                        }
                        
                        photonEndPosition = FacetIntersection;
                        foundIntersectionPoint = true;
                        LastHitFacetIdx = facetIdx;
                        
                        this -> ApplySnellLaw( bar, facetIdx );
                    }
                    else {
                        if (verbose > 4){
                            std::cout
                            << "abs(FacetIntersection.x() - bar->GetCenter().x()): "
                            << abs(FacetIntersection.x() - bar->GetCenter().x())
                            << ", bar -> GetWidth()/2: "
                            << bar -> GetWidth()/2 << std::endl
                            << ", abs(FacetIntersection.y() - bar->GetCenter().y()):"
                            <<  abs(FacetIntersection.y() - bar->GetCenter().y())
                            << ", bar -> GetThickness()/2:"
                            << bar -> GetThickness()/2 << std::endl
                            << ", abs(FacetIntersection.z() - bar->GetCenter().z()): "
                            <<  abs(FacetIntersection.z() - bar->GetCenter().z())
                            << ", bar -> GetLength()/2:"
                            << bar -> GetLength()/2 << std::endl;
                        }
                        Debug(4, "intersection point of photon with plane plane not on facet" );
                    }
                }
                if (verbose>2){
                    std::cout << "done stepping through *" << fName << "* facet" << std::endl;
                    PrintEmptyLine();
                }
            }
        }
        double StepLength_mm = (photonEndPosition - photonStartPosition).Mag(); // measured in [mm]
        TotalPathLength += StepLength_mm;
        TotalPathLengthInScintillator += StepLength_mm;
        TimeFromStart += StepLength_mm / velocity_mm_sec;
                
        // add step point
        Npoints++;
        this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        PathPoints.push_back( photonEndPosition );
        photonStartPosition = photonEndPosition;
        
        // break loop if for some reason photon is outside scintillator
        if ( !bar->ContainsPoint( photonStartPosition) ) {
            photonInBar = false;
            Debug(2, "Photon is no longer in scintillation bar for some reason. Breaking propagation steps.");
        }
        
        if (verbose>2) {
            PrintTVector3(photonEndPosition);
            Debug(2, Form("Number of photon steps: %d , photonInBar: %d", Npoints-1, photonInBar) );
            PrintLine();
        }
    }
    
    if (!photonInBar){ // photon emerged from paddle
        SetTrajectoryStart( photonEndPosition );
        SetTrajectoryDirec( photonDirection );
        photonEndPosition = photonEndPosition + 500 * photonDirection;
        
        // add step point in world, outside scintillator and waveguide, if the photon did not enter the waveguide...
        if (!photonArrivedAtFrontFacet){
            Npoints++;
            this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
            PathPoints.push_back( photonEndPosition );
            TimeFromStart += 10;
        }
        if (verbose>3){
            // DrawTrajectory();
            PrintTrajectory("outside scintillator");
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
    if (random_number > exp( -TotalPathLengthInScintillator/AbsorbtionLength ) ){
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
double Photon::GetTrajectoryAngleWithPlane(TVector3 facetNormal, std::string facetName) {
    // [https://onlinemschool.com/math/library/analytic_geometry/plane_line/]
    //  The direction vector v will meet the plane represented by the equation
    //          ax + by + cz + d = 0
    // with a normal n=(a,b,c) [and a shift of d = -ax0 - by0 - cz0]
    // at an angle given by
    //
    //         phi = | n \cdot v |/ |n|*|v|
    TVector3 v = trajectoryDirec;
    TVector3 n = facetNormal;
    
    double phi = TMath::Pi()/2. - asin((fabs(n.Dot(v)) / (n.Mag() * v.Mag())));
    
    Debug(4, Form("Photon::GetTrajectoryAngleWithPlane(%s)",facetName.c_str()));
    Debug(3, Form("n.Dot(v): %.2f, |n|: %.1f, |v|: %.1f, phi: %.2f",n.Dot(v),n.Mag(),v.Mag(),phi));
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
void Photon::ArriveAtFrontFacet(){
    // if photon touched the front facet of the scintillation bar
    // and intersected with its plane,
    // it emerged out of the paddle
    photonInBar = false;
    photonArrivedAtFrontFacet = true;
    HitFrontFacetPos = photonEndPosition;
    this -> SetLineColor( 2 ); // change the color of the photons that arrived at the front facet to red
    if (photonDirectFromProduction){
        this -> SetLineColor( 3 ); // change the color to green if its a direct photon....
    }
    
}

// ------------------------------------------------------- //
void Photon::ApplySnellLaw(Bar * bar, int facetIdx){
    // This function performs the main action that happens
    // when the photon meets one of the scintillation bar facet.
    // The photon will either emerge from the bar if its angle
    // is smaller than the critical angle for complete internal reflection,
    // or it will stay in the paddle if the angle is greater than the critical angle,
    // and the trajectory direction will be flipped
    
    Debug(2 , Form("Photon::ApplySnellLaw(%s)",bar->facetNames.at(facetIdx).c_str()));
    
    // if photon touched the front facet of the scintillation bar
    // and intersected with its plane,
    // it emerged out of the paddle
    if (bar->facetNames.at(facetIdx) == "Front"){
        ArriveAtFrontFacet();
        return;
    }
    
    // photon emerge out of the
    // paddle also if its angle is smaller than the critical angle
    // for total internal reflection
    double photonTrajectoryAngleWithPlane = this -> GetTrajectoryAngleWithPlane( bar->facetNormals.at(facetIdx),
                                                                                bar->facetNames.at(facetIdx) );
    Debug(3 , Form("photon Trajectory Angle With Plane: %.1f deg.", aux.rad2deg(photonTrajectoryAngleWithPlane)));
    
    
    if ( photonTrajectoryAngleWithPlane > bar->GetTotalInternalReflectionAngle() ){ // total internal reflection
    
        Debug(3 , Form("angle %.1f deg. > TIR = %.1f deg., so reflected!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(bar->GetTotalInternalReflectionAngle())));
        
        this -> flipPhotonDirection( bar->facetNames.at(facetIdx) );
        
    } else { // photon exit scintillation bar
        
        Debug(3 , Form("angle %.1f deg. < TIR = %.1f deg., so emerge from scintillation bar!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(bar->GetTotalInternalReflectionAngle())));
        
        this -> ApplySnellDivergence( bar -> facetNormals.at(facetIdx) , bar -> GetRefractiveIndex() );
        photonInBar = false;
        return;
    }
    
}

// ------------------------------------------------------- //
void Photon::ApplySnellDivergence( TVector3 PlaneNormal, double n_in ){
    // change photon direction according to Snell' Law
    Debug(5,Form("ApplySnellDivergence( Plane normal=(%.1f,%.1f,%.1f), n(medium)=%.2f )"
                 ,PlaneNormal.x(),PlaneNormal.y(),PlaneNormal.z(),n_in));
    
    // Vector form of Snell' law:
    double n_out = 1; // air
    double r = n_in / n_out, r2 = r*r;
    TVector3 v_in = trajectoryDirec; // vector in bar
    
    // (1)
    // [http://www.starkeffects.com/snells-law-vector.shtml]
    // v_out = (n_in/n_out) (n x (-n x v_in)) - n * sqrt( 1 - (n_in/n_out)^2 |v_in x n|^2)
    //
    //    TVector3 CrossProduct = PlaneNormal.Cross( v_in );
    //    TVector3 v_out =  r*( PlaneNormal.Cross( - CrossProduct ) )
    //                      - PlaneNormal * sqrt(1 - r2*CrossProduct.Mag2());
    //    Debug(6,Form("Method (1) v(out) = (%.3f,%.3f,%.3f)",v_out.x(),v_out.y(),v_out.z()));
    //
    // (2)
    // [https://en.wikipedia.org/wiki/Snell%27s_law]
    // v_out = r * v_in + (rc - \sqrt { 1 - r^2(1-c^2) } ) * normal
    //
    // where
    //
    // r = n_in/n_out
    // c = - normal \dot v_in
    
    double c = - PlaneNormal.Dot( v_in ), c2 = c*c;
    TVector3 v_out =  r*v_in + ( r*c - sqrt( 1 - r2*(1-c2) ) )*PlaneNormal;
    // Debug(6,Form("Method (2) v(out) = (%.3f,%.3f,%.3f)",v_out.x(),v_out.y(),v_out.z()));
    
    photonDirection = v_out;
    trajectoryDirec = photonDirection;

    Debug(5, Form("direction in medium (%.2f,%.2f,%.2f), outside medium (%.2f,%.2f,%.2f), r=%.2f, c=%.2f",
                  v_in.x(),v_in.y(),v_in.z(),v_out.x(),v_out.y(),v_out.z(),r,c) );
}


// ------------------------------------------------------- //
double Photon::GetTimeFromStart () {
    // compute the photon time,
    // counting from the time when the proton entered the scintillator
    return TimeFromStart;
}



// ------------------------------------------------------- //
void Photon::PrintPath (std::string name){
    Debug(2,Form("Photon::PrintPath(%s)",name.c_str()));
    for (auto PathPoint : PathPoints){
        PrintTVector3(PathPoint);
    }
}


// ------------------------------------------------------- //
void Photon::flipPhotonDirection(std::string facetName){
    
    double shiftFromFacet = 0.1; // [mm]
    if (facetName == "Top" || facetName == "Bottom"){
        photonDirection.SetY( -photonDirection.y() );
        
        if ( facetName == "Top")
            photonEndPosition.SetY( photonEndPosition.y() - shiftFromFacet );
        else
            photonEndPosition.SetY( photonEndPosition.y() + shiftFromFacet );
    }
    else if (facetName == "Left" || facetName == "Right") {
        photonDirection.SetX( -photonDirection.x() );
        
        if ( facetName == "Left")
            photonEndPosition.SetX( photonEndPosition.x() - shiftFromFacet );
        else
            photonEndPosition.SetX( photonEndPosition.x() + shiftFromFacet );
    }
    else if (facetName == "Back" || facetName == "Front") {
        photonDirection.SetZ( -photonDirection.z() );
        
        if (facetName == "Back")
            photonEndPosition.SetZ( photonEndPosition.z() + shiftFromFacet );
    }
}


// ------------------------------------------------------- //
void Photon::ArriveAtWaveguideExit(){
    // move to waveguide
    photonInWaveguide = false;
    photonArrivedAtWaveguideExit = true;
    HitWaveguideExitPos = photonEndPosition;
    
    this -> SetLineColor( 4 );
}


// ------------------------------------------------------- //
void Photon::ArriveAtWaveguideMarginExit(){
    // move to waveguide
    photonInWaveguide = false;
    photonArrivedAtWaveguideExit = true;
    photonArrivedAtWaveguideMarginExit = true;
    HitWaveguideMarginExitPos = photonEndPosition;
    
    // visualize
    // (1) set the photon to shoot straight forward
    // (2) change the color of the photons that arrived at the front facet to blue
    this -> SetLineColor( 5 );
    photonEndPosition = photonEndPosition + 500 * TVector3(0,0,1);
    this -> SetPoint(Npoints,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        
    // detector - move this to hitting front facet of waveguide
    DecideIfReadOutByDetector();
}

// ------------------------------------------------------- //
void Photon::PropagateInWaveguide( Waveguide * waveguide ){
    Debug(2 , "Photon::PropagateInWaveguide()");
    SetInWaveguide(waveguide->GetRefractiveIndex());
    
    int LastHitFacetIdx = -1;
    
    while (photonInWaveguide && (Npoints < MAXPHOTONSTEPS)) { // stop if number of points it too large
        
        this -> SetTrajectoryDirec( photonDirection );
        this -> SetTrajectoryStart( photonStartPosition );
        if (verbose>2){ Debug(1,"photon in waveguide"); PrintTrajectory("expected"); }
        
        // (3.2) Decide if the paddle bounding box could be crossed by a vector.
        // intersect with paddle facets determines photon end position...
        bool foundIntersectionPoint = false;
        for (int facetIdx=0; facetIdx<6; facetIdx++){
            
            if (foundIntersectionPoint) continue;
            std::string fName = waveguide->facetNames.at(facetIdx).c_str();
            
            Debug(2 , Form("looking for waveguide intersection at *%s* (LastHitFacetIdx %d)", fName.c_str() ,LastHitFacetIdx) );
            
            // avoid from intersecting trajectory with wrong facet - a one which is opposite to photon direction
            if (PhotonTrajOppositeFacet( fName )){
                Debug(2, "Photon Trajory Opposite Facet, continuing to next facet \n" );
                continue;
            }
            
            if (facetIdx!=LastHitFacetIdx ){
                Debug(3, "Yet haven't found, and looking for plane intersection: ");
                
                TVector3 FacetIntersection =
                this -> TrajIntWithPlane ( waveguide->facetCenters.at(facetIdx), waveguide->facetNormals.at(facetIdx) );
                
                if (verbose>3) {Print4TVector3(FacetIntersection);}
                
                if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                    
                    if ( waveguide->CheckIfPointOnFacet( facetIdx, FacetIntersection ) ){
                        
                        Debug(4, "photon met waveguide facet. apply snell law");
                        
                        // if the photon hit any facet on its way (except the front facet)
                        // it is no longer a "direct photon" that was emitted from the proton directly
                        // to the detector with a short path
                        // so change the flag indicating that the photon did
                        // not hit any facet on its way out if the facet it hit
                        // is not the front facet where the detector is located...
                        // once this flag is changed once, the photon will forever be falgged as non-direct
                        if (waveguide->facetNames.at(facetIdx) != "Front"){
                            photonDirectFromProduction = false;
                        }
                        
                        photonEndPosition = FacetIntersection;
                        foundIntersectionPoint = true;
                        LastHitFacetIdx = facetIdx;
                        
                        this -> ApplySnellLaw( waveguide, facetIdx );
                    }
                    else {
                        Debug(4, "intersection point of photon with plane plane not on facet" );
                    }
                }
                if (verbose>2){
                    std::cout << "done stepping through *" << fName << "* facet" << std::endl;
                    PrintEmptyLine();
                }
            }
        }
        double StepLength_mm = (photonEndPosition - photonStartPosition).Mag(); // measured in [mm]
        TotalPathLength += StepLength_mm;
        TotalPathLengthInWaveguide += StepLength_mm;
        TimeFromStart += StepLength_mm / velocity_mm_sec;

        
        // add step point
        Npoints++;
        this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        PathPoints.push_back( photonEndPosition );
        photonStartPosition = photonEndPosition;
                
        // break loop if for some reason photon is outside waveguide
        if ( !waveguide->ContainsPoint( photonStartPosition) ) {
            photonInWaveguide = false;
            Debug(2, Form("Photon at (%.3f,%.3f,%.3f) is no longer in waveguide for some reason. Breaking propagation steps.",
                  photonStartPosition.x(),photonStartPosition.y(),photonStartPosition.z()));
        }
        // break loop if haven't found any interaction point in either facet - due to some funny behavior
        if (!foundIntersectionPoint){
            photonInWaveguide = false;
            Debug(2, "haven't found any interaction point in either facet, due to some funny behavior");
        }
        if (verbose>2) {
            Debug(2,"completed photon step in waveguide");
            PrintTVector3(photonEndPosition);
            PrintTVector3(photonDirection);
            Debug(2, Form("Number of photon steps: %d, photon %s in waveguide", Npoints-1, photonInWaveguide ? "still" : "not") );
            PrintLine();
        }
        
    }
    
    if (!photonInWaveguide){ // photon emerged from waveguide
        SetTrajectoryStart( photonEndPosition );
        SetTrajectoryDirec( photonDirection );
        photonEndPosition = photonEndPosition + 500 * photonDirection;
        
        // add step point in world, outside scintillator and waveguide, if the photon did not exit the waveguide
        // from the detector (front) side...
        if (!photonArrivedAtWaveguideExit){
            Npoints++;
            this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
            PathPoints.push_back( photonEndPosition );
            TimeFromStart += 10;
        }
        
        if (verbose>3){
            // DrawTrajectory();
            PrintTrajectory("outside waveguide");
            Debug(3,"photon exitted from waveguide!");
            PrintLine();
        }
    }
    
    this -> DecideIfAbsorbedInWaveguide( waveguide->GetAbsorbtionLength() );
}

// ------------------------------------------------------- //
void Photon::PropagateInWaveguideMargin( WaveguideMargin * wgmargin ){
    // same as Photon::PropagateInWaveguide
    Debug(2 , "Photon::PropagateInWaveguideMargin()");
    SetInWaveguide(wgmargin->GetRefractiveIndex());
    
    int LastHitFacetIdx = -1;
    
    while (photonInWaveguideMargin && (Npoints < MAXPHOTONSTEPS)) { // stop if number of points it too large
        
        this -> SetTrajectoryDirec( photonDirection );
        this -> SetTrajectoryStart( photonStartPosition );
        if (verbose>2){ Debug(1,"photon in waveguide margin"); PrintTrajectory("expected"); }
        
        // (3.2) Decide if the paddle bounding box could be crossed by a vector.
        // intersect with paddle facets determines photon end position...
        bool foundIntersectionPoint = false;
        for (int facetIdx=0; facetIdx<6; facetIdx++){
            
            if (foundIntersectionPoint) continue;
            std::string fName = wgmargin->facetNames.at(facetIdx).c_str();
            
            Debug(2 , Form("looking for waveguide intersection at *%s* (LastHitFacetIdx %d)", fName.c_str() ,LastHitFacetIdx) );
            
            // avoid from intersecting trajectory with wrong facet - a one which is opposite to photon direction
            if (PhotonTrajOppositeFacet( fName )){
                Debug(2, "Photon Trajory Opposite Facet, continuing to next facet \n" );
                continue;
            }
            
            if (facetIdx!=LastHitFacetIdx ){
                Debug(3, "Yet haven't found, and looking for plane intersection: ");
                
                TVector3 FacetIntersection =
                this -> TrajIntWithPlane ( wgmargin->facetCenters.at(facetIdx), wgmargin->facetNormals.at(facetIdx) );
                
                if (verbose>3) {Print4TVector3(FacetIntersection);}
                
                if (FacetIntersection.x()!=FacetIntersection.y() && FacetIntersection.x()!=FacetIntersection.z()){
                    
//                    if ( wgmargin->CheckIfPointOnFacet( facetIdx, FacetIntersection ) ){
                    if (    abs(FacetIntersection.x() - wgmargin->GetCenter().x()) <= wgmargin -> GetWidth()/2     + 0.1
                        &&  abs(FacetIntersection.y() - wgmargin->GetCenter().y()) <= wgmargin -> GetThickness()/2 + 0.1
                        &&  abs(FacetIntersection.z() - wgmargin->GetCenter().z()) <= wgmargin -> GetLength()/2    + 0.1){
                    
                        Debug(4, "photon met waveguide margin facet. apply snell law");
                        if (wgmargin->facetNames.at(facetIdx) != "Front"){
                            photonDirectFromProduction = false;
                        }
                        
                        photonEndPosition = FacetIntersection;
                        foundIntersectionPoint = true;
                        LastHitFacetIdx = facetIdx;
                        
                        this -> ApplySnellLaw( wgmargin, facetIdx );
                    }
                    else {
                        Debug(4, "intersection point of photon with plane plane not on facet" );
                    }
                }
                if (verbose>2){
                    std::cout << "done stepping through *" << fName << "* facet" << std::endl;
                    PrintEmptyLine();
                }
            }
        }
        double StepLength_mm = (photonEndPosition - photonStartPosition).Mag(); // measured in [mm]
        TotalPathLength += StepLength_mm;
        TotalPathLengthInWaveguideMargin += StepLength_mm;
        TimeFromStart += StepLength_mm / velocity_mm_sec;

        
        // add step point
        Npoints++;
        this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
        PathPoints.push_back( photonEndPosition );
        photonStartPosition = photonEndPosition;
                
        // break loop if for some reason photon is outside waveguide
        if ( !wgmargin->ContainsPoint( photonStartPosition) ) {
            photonInWaveguideMargin = false;
            Debug(2, Form("Photon at (%.3f,%.3f,%.3f) is no longer in waveguide for some reason. Breaking propagation steps.",
                  photonStartPosition.x(),photonStartPosition.y(),photonStartPosition.z()));
        }
        // break loop if haven't found any interaction point in either facet - due to some funny behavior
        if (!foundIntersectionPoint){
            photonInWaveguideMargin = false;
            Debug(2, "haven't found any interaction point in either facet, due to some funny behavior");
        }
        if (verbose>2) {
            Debug(2,"completed photon step in waveguide");
            PrintTVector3(photonEndPosition);
            PrintTVector3(photonDirection);
            Debug(2, Form("Number of photon steps: %d, photon %s in waveguide",
                          Npoints-1, photonInWaveguideMargin ? "still" : "not") );
            PrintLine();
        }
        
    }
    
    if (!photonInWaveguideMargin){ // photon emerged from waveguide
        SetTrajectoryStart( photonEndPosition );
        SetTrajectoryDirec( photonDirection );
        photonEndPosition = photonEndPosition + 500 * photonDirection;
        
        // add step point in world, outside scintillator and waveguide, if the photon did not exit the waveguide
        // from the detector (front) side...
        if (!photonArrivedAtWaveguideMarginExit){
            Npoints++;
            this -> SetPoint(Npoints-1,photonEndPosition.X(),photonEndPosition.Y(),photonEndPosition.Z());
            PathPoints.push_back( photonEndPosition );
            TimeFromStart += 10;
        }
        
        if (verbose>3){
            // DrawTrajectory();
            PrintTrajectory("outside waveguide margin");
            Debug(3,"photon exitted from waveguide margin!");
            PrintLine();
        }
    }
    this -> DecideIfAbsorbedInWaveguideMargin( wgmargin->GetAbsorbtionLength() );
}

// ------------------------------------------------------- //
void Photon::DecideIfAbsorbedInWaveguide( double AbsorbtionLength ){
    // same as Photon::::DecideIfAbsorbedInScintillator
    photonAbsorbedInWaveguide = false;
    double random_number = r -> Uniform(0.0,1.0);
    if (random_number > exp( -TotalPathLengthInWaveguide/AbsorbtionLength ) ){
        photonAbsorbedInWaveguide = true;
        photonReadOutByDetector = false;
    }
}

// ------------------------------------------------------- //
void Photon::DecideIfAbsorbedInWaveguideMargin( double AbsorbtionLength ){
    // same as Photon::::DecideIfAbsorbedInScintillator
    photonAbsorbedInWaveguideMargin = false;
    double random_number = r -> Uniform(0.0,1.0);
    if (random_number > exp( -TotalPathLengthInWaveguide/AbsorbtionLength ) ){
        photonAbsorbedInWaveguideMargin = true;
        photonReadOutByDetector = false;
    }
}


// ------------------------------------------------------- //
void Photon::ApplySnellLaw(Waveguide * waveguide, int facetIdx){
    // Same as ApplySnellLaw() that is used the scintillator, but for the waveguide
    Debug(2 , Form("Photon::ApplySnellLaw(%s)",waveguide->facetNames.at(facetIdx).c_str()));
    
    // if photon touched the front facet of the scintillation bar
    // and intersected with its plane,
    // it emerged out of the paddle
    if (waveguide->facetNames.at(facetIdx) == "Front"){
        ArriveAtWaveguideExit();
        return;
    }
    
    // photon emerge out of the
    // waveguide also if its angle is smaller than the critical angle
    // for total internal reflection
    double photonTrajectoryAngleWithPlane = this -> GetTrajectoryAngleWithPlane(waveguide->facetNormals.at(facetIdx),
                                                                                waveguide->facetNames.at(facetIdx) );
    Debug(3 , Form("photon Trajectory Angle With Plane: %.1f deg.", aux.rad2deg(photonTrajectoryAngleWithPlane)));
    
    // flip photon direction
    if ( photonTrajectoryAngleWithPlane > waveguide->GetTotalInternalReflectionAngle() ){ // total internal reflection
    
        Debug(3 , Form("angle %.1f deg. > TIR = %.1f deg., so reflected!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(waveguide->GetTotalInternalReflectionAngle())));
        
        this -> flipPhotonDirection( waveguide->facetNames.at(facetIdx) );
                
    } else { // photon exit waveguide
        
        Debug(3 , Form("angle %.1f deg. < TIR = %.1f deg., so emerge from waveguide!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(waveguide->GetTotalInternalReflectionAngle())));
        
        this -> ApplySnellDivergence( waveguide -> facetNormals.at(facetIdx) , waveguide -> GetRefractiveIndex() );
        photonInWaveguide = false;
        return;
    }
    
}



// ------------------------------------------------------- //
void Photon::ApplySnellLaw(WaveguideMargin * wgmaring, int facetIdx){
    // Same as ApplySnellLaw() that is used the scintillator, but for the waveguide
    Debug(2 , Form("Photon::ApplySnellLaw(%s)",wgmaring->facetNames.at(facetIdx).c_str()));
    
    // if photon touched the front facet of the scintillation bar
    // and intersected with its plane,
    // it emerged out of the paddle
    if (wgmaring->facetNames.at(facetIdx) == "Front"){
        ArriveAtWaveguideMarginExit();
        return;
    }
    
    // photon emerge out of the
    // waveguide also if its angle is smaller than the critical angle
    // for total internal reflection
    double photonTrajectoryAngleWithPlane = this -> GetTrajectoryAngleWithPlane(wgmaring->facetNormals.at(facetIdx),
                                                                                wgmaring->facetNames.at(facetIdx) );
    Debug(3 , Form("photon Trajectory Angle With Plane: %.1f deg.", aux.rad2deg(photonTrajectoryAngleWithPlane)));
    
    // flip photon direction
    if ( photonTrajectoryAngleWithPlane > wgmaring->GetTotalInternalReflectionAngle() ){ // total internal reflection
    
        Debug(3 , Form("angle %.1f deg. > TIR = %.1f deg., so reflected!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(wgmaring->GetTotalInternalReflectionAngle())));
        
        this -> flipPhotonDirection( wgmaring->facetNames.at(facetIdx) );
                
    } else { // photon exit waveguide
        
        Debug(3 , Form("angle %.1f deg. < TIR = %.1f deg., so emerge from waveguide!",
                       aux.rad2deg(photonTrajectoryAngleWithPlane), aux.rad2deg(wgmaring->GetTotalInternalReflectionAngle())));
        
        this -> ApplySnellDivergence( wgmaring -> facetNormals.at(facetIdx) , wgmaring -> GetRefractiveIndex() );
        photonInWaveguideMargin = false;
        return;
    }
    
}
