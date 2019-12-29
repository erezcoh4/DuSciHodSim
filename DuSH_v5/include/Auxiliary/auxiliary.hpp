#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP
#include <iostream>
#include <iomanip>
#include <fstream>


#define PrintEmptyLine(){ std::cout << std::endl;}
#define PrintLine(){ std::cout << "------------------------------------------------" << std::endl;}
#define PrintXLine(){ std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;}
#define PrintTVector3(v){ std::cout <<setprecision(1)<<fixed << #v<< ": ("<<v.X()<<","<<v.Y()<<","<<v.Z()<<")"<< std::endl;}


class auxiliary
{
private:
    
    std::string csvfilename;
    std::ofstream csvfile;
    const std::string csvpath = "/Users/erezcohen/Desktop/data/DuSiHodSim/output/csv_files";
    
    
public:
    
    Int_t verbose;
    
    // contructor
    auxiliary(){};
    
    
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
    
    
    
    // auxilary
    double      rad2deg (double angle_rad) {return angle_rad*180./3.1415;};
    double      deg2rad (double angle_deg) {return angle_deg*3.1415/180.;};

};


#endif

