#include <FaceModule/Face.hpp>
#include <BarModule/Bar.hpp>

int main(){
    
    // scintillation bar dimensions [mm]
    float length    = 2000;
    float width     = 200;
    float thickness = 60;

	Bar bar( length , width, thickness );

   	return 0;
}
