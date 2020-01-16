#ifndef VERSION_HPP
#define VERSION_HPP
// ---------------------------------
// version number
#define vNumber 6.0
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
    std::string inputName;
    
    // Setters
    void            SetVerbose (int fv)   {verbose = fv;};
    
    // Getters
    double 	        GetVersion () {return VersionNumber;};
    Int_t           GetVerbose () {return verbose;};
    std::string   GetInputName () {return inputName;};

    // contructor
    version(int argc, char **argv){
        PrintLine();
        std::cout << std::endl;
        std::cout << "Dubna scintillator simulator " << std::endl;
        std::cout << "version " << VersionNumber << std::endl;
        std::cout << std::endl;
        std::cout << "usage: ";
        std::cout << std::endl;
        std::cout << "> ./RunProgram <configuration=\"typical\"> <verbose=0> ";
        std::cout << std::endl;
        std::cout << std::endl;
        
        verbose = 0;
        inputName = "typical";
        
        if (argc>2) {
            verbose = atoi(argv[2]);
            std::cout << "verbose: " << verbose << std::endl;
        }

        if (argc>1) {
            inputName = argv[1];
            std::cout << "input: " << inputName << std::endl;
        }


        PrintLine();
    } ;
    
    
    // prints
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }
    
    
};


#endif

