#include <iostream>
#include <PhotonModule/Photon.hpp>
using namespace std;

// ------------------------------------------------------- //
Photon::Photon (Int_t n, Option_t *option):
TPolyLine3D( n, option ){
    std::cout << "built TPolyLine3D" << std::endl;
    Npoints = n;
}

// ------------------------------------------------------- //
Photon::Photon (Int_t n, Double_t *p, Option_t *option):
TPolyLine3D( n, p, option ){
    std::cout << "built TPolyLine3D" << std::endl;
    Npoints = n;
}



// ------------------------------------------------------- //
TVector3 Photon::findIntersectionLinePlane(
                        const TVector3 &pos, const TVector3 &dir,
                        const TVector3 &planeCenter, const  TVector3 &planeNormal) {
    // Finds the intersection point of a straight line with any plane.
    // input
    // pos:            a point on the straight line.
    // dir:            direction of the straight line.
    // planeCenter:    a point on the plane.
    // planeNormal:    normal vector of the plane.
    //
    // return
    // pointIntersect: the intersection point (return value).

    Double_t denom = planeNormal.Dot(dir);
    if (denom != 0.0) {
        Double_t t = ((planeCenter.x() - pos.x()) * planeNormal.x() +
              (planeCenter.y() - pos.y()) * planeNormal.y() +
              (planeCenter.z() - pos.z()) * planeNormal.z()) / denom;

        return pos + (t * dir);
    } else {
    ::Warning("Photon::findIntersectionLinePlane()", "No intersection point found : (plane || track)");
        return TVector3(-9999,-9999,-9999);
    }
    return TVector3(-9999,-9999,-9999);
}


// ------------------------------------------------------- //
TVector3 Photon::TrajIntWithPlane(
                        const TVector3 &planeCenter, const  TVector3 &planeNormal) {
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
//    planeNormal.Print();
//    trajectoryDirec.Print();
    std::cout << "denom: " << denom << std::endl;
    planeCenter.Print();
    
    if (denom != 0.0) {
        Double_t t = ((planeCenter.x() - trajectoryStart.x()) * planeNormal.x() +
              (planeCenter.y() - trajectoryStart.y()) * planeNormal.y() +
              (planeCenter.z() - trajectoryStart.z()) * planeNormal.z()) / denom;

        return trajectoryStart + (t * trajectoryDirec);
    } else {
    ::Warning("Photon::IntersectionPointOfCurrentTrajectoryWithPlane()", "No intersection point found : (plane || track)");
        return TVector3(-9999,-9999,-9999);
    }
    return TVector3(-9999,-9999,-9999);
}



