#include <iostream>
#include <WaveguideModule/Waveguide.hpp>
#include "TMath.h"
#define PiRad 3.141528

// ------------------------------------------------------- //
Waveguide::Waveguide (const char *name, Double_t fdx1, Double_t fdx2, Double_t fdy1, Double_t fdy2, Double_t fdz):
TGeoTrd2 (name, fdx1, fdx2, fdy1, fdy2, fdz){
    std::cout << "constructed scintillation Waveguide" << std::endl;
    dz = fdz;
    dx1 = fdx1;
    dx2 = fdx2;
    dy1 = fdy1;
    dy2 = fdy2;
    defineFacets();    
}

// ------------------------------------------------------- //
void Waveguide::defineFacets(){
// need to go over this and change these!
  facetNames = {
       "Top",  "Bottom" ,
       "Left", "Right" ,
       "Back", "Front"};
   
    //    Given any two vectors a=r2-r1 and b=r3-r1 , the vector cross product axb
    // will be perpendicular to both of them, even if we have to consider the degenerate case of 0
    // (that is, zero is perpendicular to anything).
    TVector3 r1, r2, r3, r4;
    

    // (1) Top facet - trapez
    r1 = TVector3( -dx1 , +dy1 , -dz);
    r2 = TVector3( +dx1 , +dy1 , -dz);
    r3 = TVector3( +dx2 , +dy2 , +dz);
    r4 = TVector3( -dx2 , +dy2 , +dz);
    std::vector<TVector3> TopVertices = {r1, r2, r3, r4};
    TVector3 TopCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 TopNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;

    // (2) Bottom facet - trapez
    r1 = TVector3( -dx1 , -dy1 , -dz);
    r2 = TVector3( +dx1 , -dy1 , -dz);
    r3 = TVector3( +dx2 , -dy2 , +dz);
    r4 = TVector3( -dx2 , -dy2 , +dz);
    std::vector<TVector3> BottomVertices = {r1, r2, r3, r4};
    TVector3 BottomCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 BottomNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;

    
    // (3) Left facet - trapez
    r1 = TVector3( +dx1 , +dy1 , -dz);
    r2 = TVector3( +dx2 , +dy2 , +dz);
    r3 = TVector3( +dx2 , -dy2 , +dz);
    r4 = TVector3( +dx1 , -dy1 , -dz);
    std::vector<TVector3> LeftVertices = {r1, r2, r3, r4};
    TVector3 LeftCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 LeftNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;

    // (4) Right facet - trapez
    r1 = TVector3( -dx1 , +dy1 , -dz);
    r2 = TVector3( -dx1 , -dy1 , -dz);
    r3 = TVector3( -dx2 , -dy2 , +dz);
    r4 = TVector3( -dx2 , +dy2 , +dz);
    std::vector<TVector3> RightVertices = {r1, r2, r3, r4};
    TVector3 RightCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 RightNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;

    // (5) Back facet - rectangle
    r1 = TVector3( -dx1 , -dy1 , -dz);
    r2 = TVector3( +dx1 , -dy1 , -dz);
    r3 = TVector3( +dx1 , +dy1 , -dz);
    r4 = TVector3( -dx1 , +dy1 , -dz);
    std::vector<TVector3> BackVertices = {r1, r2, r3, r4};
    TVector3 BackCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 BackNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;
    
    // (6) Front facet - rectangle
    r1 = TVector3( -dx2 , -dy2 , +dz);
    r2 = TVector3( +dx2 , -dy2 , +dz);
    r3 = TVector3( +dx2 , +dy2 , +dz);
    r4 = TVector3( -dx2 , +dy2 , +dz);
    std::vector<TVector3> FrontVertices = {r1, r2, r3, r4};
    TVector3 FrontCenter = (r1 + r2 + r3 + r4)*0.25;
    TVector3 FrontNormal = ( (r2 - r1).Cross( r3 - r1 ) ).Unit() ;

    
    facetCenters = {
        TopCenter,   BottomCenter,
        LeftCenter,  RightCenter,
        BackCenter,  FrontCenter};
    
    facetNormals = {
        TopNormal,   BottomNormal,
        LeftNormal,  RightNormal,
        BackNormal,  FrontNormal
    };
        
    facetVertices = {
        TopVertices,   BottomVertices,
        LeftVertices,  RightVertices,
        BackVertices,  FrontVertices
    };
    
    Debug(-1, "Waveguide::defineFacets()");
    for (auto facetCenter : facetCenters){
        PrintTVector3( facetCenter );
    }
    for (auto facetNormal : facetNormals){
        PrintTVector3( facetNormal );
    }
}

// ------------------------------------------------------- //
void Waveguide::SetTotalInternalReflectionAngle(){
    // with respect to air
    TotalInternalReflectionAngle = asin( 1.0 / refractiveIndex );
    Debug(0, Form("refractive index: %.1f" , refractiveIndex ));
    Debug(0, Form("TIR angle: %.2f deg." , TotalInternalReflectionAngle*180./PiRad ));
}


// ------------------------------------------------------- //
bool Waveguide::CheckIfPointOnFacet ( int facetIdx, TVector3 pt ){
    Debug(2,"Waveguide::CheckIfPointOnFacet()");
    // check if the point pt is on the facet facetIdx
    // (and not just somewhere on the facet plane but outside trapez area)
    //
    //
    // The solution by Reinier van Vliet and Remco Lam.
    // [http://www.eecs.umich.edu/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html]
    // To determine whether a point is on the interior of a convex polygon in 3D one might be tempted to first determine whether the point is on the plane, then determine it's interior status.
    // Both of these can be accomplished at once by computing the sum of the angles between the test point (q below) and every pair of edge points p[i]->p[i+1]. This sum will only be twopi if both the point is on the plane of the polygon AND on the interior. The angle sum will tend to 0 the further away from the polygon point q becomes.
    
    double Eps = 0.001;
    
    std::vector<TVector3> arms;
    std::vector<TVector3> Vertices = facetVertices.at(facetIdx);
    for (int vIdx=0; vIdx<4 ; vIdx++) {
        TVector3 arm = Vertices.at(vIdx) - pt;
        // in the case where the point is exactly on a vertex return true
        if ( arm.Mag() < Eps ) return true;
        arms.push_back( Vertices.at(vIdx) - pt );
    }
    
    Double_t SumOfAngles =
        arms.at(0).Angle(arms.at(1))
    +   arms.at(1).Angle(arms.at(2))
    +   arms.at(2).Angle(arms.at(3))
    +   arms.at(3).Angle(arms.at(0));

    Debug(2, Form("SumOfAngles: %.2f pi",SumOfAngles/TMath::Pi()));
    
    if ( fabs(SumOfAngles - 2*TMath::Pi()) < Eps ) return true;
    
    return false;
}


// ------------------------------------------------------- //
bool Waveguide::ContainsPoint(const TVector3 position) {
    Double_t point[3] = {position.x(), position.y(), position.z()};
    return this->Contains(point);
}
