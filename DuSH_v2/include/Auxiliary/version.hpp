#ifndef VERSION_HPP
#define VERSION_HPP

#include <iostream>

class version
{
private:

    const double VersionNumber = 2;


public:
    


	// Getters
  	double 	    GetVersion() {return VersionNumber;};

    version(){std::cout << "version " << VersionNumber << std::endl;} ;
    

};


#endif

