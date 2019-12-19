#ifndef VERSION_HPP
#define VERSION_HPP

#include <iostream>
#include <iomanip>
#define PrintEmptyLine(){ std::cout << std::endl;}
#define PrintLine(){ std::cout << "------------------------------------------------" << std::endl;}
#define PrintXLine(){ std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;}
#define PrintTVector3(v){ std::cout <<setprecision(1)<<fixed << #v<< ": ("<<v.X()<<","<<v.Y()<<","<<v.Z()<<")"<< std::endl;}


class version
{
private:

    const double VersionNumber = 3;
    

public:
    
    Int_t verbose;
    Int_t Nphotons;
    
    
    // Setters
    void        SetVerbose (int fv)     {verbose = fv;};
    
    
    // Getters
  	double 	    GetVersion () {return VersionNumber;};
    Int_t       GetVerbose () {std::cout << "GetVerbose(): "  << verbose  << std::endl; return verbose;};
    Int_t      GetNphotons () {std::cout << "GetNphotons() : "<< Nphotons << std::endl; return Nphotons;};
    
    // contructor
    version(int argc, char **argv){
        PrintLine();
        std::cout << std::endl;
        std::cout << "Dubna scintillator simulator " << std::endl;
        std::cout << "version " << VersionNumber << std::endl;
        std::cout << std::endl;
        
        verbose = 0;
        Nphotons = 2;
        
        if (argc>1) {
            
            verbose = atoi(argv[1]);
            std::cout << "verbose: " << verbose << std::endl;
            
            if (argc>2) {
                Nphotons = atoi(argv[2]);
                std::cout << "number of photons that will be generated: " << Nphotons << std::endl;
            }
        }
        
        PrintLine();
    } ;
    
    
    // prints
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }
    
    
        

};


#endif

