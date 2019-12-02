#include <FaceModule/Face.hpp>
#include <BarModule/Bar.hpp>

int main(){
    
    // scintillation bar dimensions [mm]
    double length    = 2000;
    double width     = 200;
    double thickness = 60;

//	Bar bar( length , width, thickness );

    Bar * bar = new TBar("Scintillation bar", length/2, width/2, thickness/2);
   	return 0;
}
