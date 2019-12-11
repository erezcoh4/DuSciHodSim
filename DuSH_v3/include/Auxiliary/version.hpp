#ifndef VERSION_HPP
#define VERSION_HPP

#include <iostream>
#define PrintLine(){ std::cout << "------------------------------------------------" << std::endl;}
#define PrintXLine(){ std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;}

class version
{
private:

    const double VersionNumber = 3;
    

public:
    
    Int_t verbose;

    // Setters
    void        SetVerbose (int fv)     {verbose = fv;};
    
    
    // Getters
  	double 	    GetVersion () {return VersionNumber;};
    Int_t       GetVerbose () {std::cout << "GetVerbose (): "<< verbose << std::endl; return verbose;};
    
    
    // contructor
    version(int argc, char **argv){
        PrintLine();
        std::cout << std::endl;
        std::cout << "Dubna scintillator simulator " << std::endl;
        std::cout << "version " << VersionNumber << std::endl;
        std::cout << std::endl;
        
        verbose = 0;
        
        if (argc>1) {
            verbose = atoi(argv[1]);
            std::cout << "verbose: " << verbose << std::endl;
        }
        
        PrintLine();
    } ;
    
    
    
        

};


#endif

