#ifndef VERSION_HPP
#define VERSION_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
#include <PhotonModule/Photon.hpp>

#define PrintEmptyLine(){ std::cout << std::endl;}
#define PrintLine(){ std::cout << "------------------------------------------------" << std::endl;}
#define PrintXLine(){ std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;}
#define PrintTVector3(v){ std::cout <<setprecision(1)<<fixed << #v<< ": ("<<v.X()<<","<<v.Y()<<","<<v.Z()<<")"<< std::endl;}


class version
{
private:
    
    const double VersionNumber = 4;
    std::string csvfilename;
    std::ofstream csvfile;
    const std::string csvpath = "/Users/erezcohen/Desktop/data/DuSiHodSim/output/csv_files";
    
    
public:
    
    Int_t verbose;
    Int_t Nphotons;
    double refractiveIndex;
    
    // Setters
    void            SetVerbose (int fv)     {verbose = fv;};
    void    SetRefractiveIndex (double n) {refractiveIndex = n;};
    
    // Getters
    double 	        GetVersion () {return VersionNumber;};
    Int_t           GetVerbose () {std::cout << "GetVerbose(): "  << verbose  << std::endl; return verbose;};
    Int_t          GetNphotons () {std::cout << "GetNphotons() : "<< Nphotons << std::endl; return Nphotons;};
    double  GetRefractiveIndex () {return refractiveIndex;};
    
    
    // contructor
    version(int argc, char **argv){
        PrintLine();
        std::cout << std::endl;
        std::cout << "Dubna scintillator simulator " << std::endl;
        std::cout << "version " << VersionNumber << std::endl;
        std::cout << std::endl;
        std::cout << "usage: ";
        std::cout << "./RunProgram <verbose=0> <Nphotons=2> <refractiveIndex=1.5>"
        std::cout << std::endl;

        verbose = 0;
        Nphotons = 2;
        refractiveIndex = 1.5;
        
        if (argc>1) {
            
            verbose = atoi(argv[1]);
            std::cout << "verbose: " << verbose << std::endl;
            
            if (argc>2) {
                Nphotons = atoi(argv[2]);
                std::cout << "number of photons that will be generated: " << Nphotons << std::endl;
            }
            if (argc>3) {
                refractiveIndex = atof(argv[3]);
                std::cout << "scintillation bar refractive index: " << refractiveIndex << std::endl;
            }
        }
        
        PrintLine();
    } ;
    
    
    // prints
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }
    
    
    
    
    
    // csv files
    void open_photons_csv(std::string simname = "",std::string header = "i,ArrivedAtFrontFacet"){
        Debug(0,"version::open_photons_csv()");
        
        csvfilename = csvpath + "/" + simname + "_photons.csv";
        csvfile.open( csvfilename );
        csvfile << header << std::endl;
        
        Debug(0,Form("opened %s",csvfilename.c_str()));
    }
    
    void write_photons_csv( std::vector<double> values ){
//        for (auto v : values){ csvfile << v << ","; }
        for (size_t i=0; i<values.size()-1; i++){
            csvfile << values.at(i) << ",";
        }
        csvfile << values.at(values.size()-1);
        csvfile << std::endl;
    }
    void close_photons_csv(){
        csvfile.close();
    }
    
    
};


#endif

