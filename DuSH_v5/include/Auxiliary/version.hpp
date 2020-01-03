#ifndef VERSION_HPP
#define VERSION_HPP
// ---------------------------------
// version number
#define vNumber 5.0
// ---------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <PhotonModule/Photon.hpp>
#include <Auxiliary/auxiliary.hpp>


class version
{
private:
    
    const double VersionNumber = vNumber;
    
    
public:
    
    
    auxiliary aux;
    
    Int_t verbose;
    Int_t Nphotons;
    double refractiveIndex;
    double PhotonsPerMeV;
    double PhotonAbsorbtionLength;
    
    // Setters
    void            SetVerbose (int fv)   {verbose = fv;};
    void    SetRefractiveIndex (double n) {refractiveIndex = n;};
    
    // Getters
    double 	        GetVersion () {return VersionNumber;};
    Int_t           GetVerbose () {std::cout << "GetVerbose(): "  << verbose  << std::endl; return verbose;};
    Int_t          GetNphotons () {std::cout << "GetNphotons() : "<< Nphotons << std::endl; return Nphotons;};
    double  GetRefractiveIndex () {return refractiveIndex;};
    double    GetPhotonsPerMeV () {return PhotonsPerMeV;};
    double GetAbsorbtionLength () {return PhotonAbsorbtionLength;};

    // contructor
    version(int argc, char **argv){
        PrintLine();
        std::cout << std::endl;
        std::cout << "Dubna scintillator simulator " << std::endl;
        std::cout << "version " << VersionNumber << std::endl;
        std::cout << std::endl;
        std::cout << "usage: ";
        std::cout << "./RunProgram <verbose=0> <Nphotons=2> <refractiveIndex=1.5> <PhotonsPerMeV=1000>";
        std::cout << std::endl;
        std::cout << std::endl;
        
        verbose = 0;
        Nphotons = 2;
        refractiveIndex = 1.5;
        PhotonsPerMeV = 100;
        PhotonAbsorbtionLength = 8000; // mm

        if (argc>1) {
            
            verbose = atoi(argv[1]);
            std::cout << "verbose: " << verbose << std::endl;
        }
        if (argc>2) {
            Nphotons = atoi(argv[2]);
            std::cout << "number of photons that will be generated: " << Nphotons << std::endl;
        }
        if (argc>3) {
            refractiveIndex = atof(argv[3]);
            std::cout << "scintillation bar refractive index: " << refractiveIndex << std::endl;
        }
        if (argc>4) {
            PhotonsPerMeV =  atof(argv[3]);
            std::cout << "Photons Per MeV energy deposition: " << PhotonsPerMeV << std::endl;
        }
        if (argc>5) {
            PhotonAbsorbtionLength =  atof(argv[5]);
            std::cout << "Photon absorbtion length: " << PhotonAbsorbtionLength << " mm" << std::endl;
        }

        PrintLine();
    } ;
    
    
    // prints
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }
    
    
};


#endif

