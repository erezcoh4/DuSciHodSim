#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TXMLEngine.h"


#define PrintEmptyLine(){ std::cout << std::endl;}
#define PrintLine(){ std::cout << "------------------------------------------------" << std::endl;}
#define PrintXLine(){ std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;}
#define PrintTVector3(v){ std::cout <<std::setprecision(1)<<std::fixed << #v<< ": ("<<v.X()<<","<<v.Y()<<","<<v.Z()<<")"<< std::endl;}


class auxiliary
{
private:
    
    const std::string csvpath = "/Users/erezcohen/Desktop/data/DuSiHodSim/output/csv_files";
    std::string csvfilename;
    std::ofstream csvfile;
    
    const std::string xmlpath = "/Users/erezcohen/Desktop/Projects/DubnaSciHod/Software/DuSciHodSim/DuSH_v7/input";
    std::string xmlfilename;
    
public:
    
    Int_t verbose;
    int ShowEveryNPhotons;
    
    // scintillator
    double      refractiveIndex;
    double      length, width, thickness; // [mm]
    double      WaveguideWidth, WaveguideLength; // [mm]
    double      PhotonsPerMeV, AbsorbtionLength;
    
    // proton gun
    double      pGunEnergy;
    TVector3    pGunPosition, pGunDirection; // [mm]
    
    
    // contructor
    auxiliary(){};
    void                    SetVerbose (int v)      { verbose = v;};
    
    // prints
    void Debug (Int_t verobosity_level, std::string text) { if ( verbose > verobosity_level ) std::cout << text << std::endl; }
    
    
    
    // xml configuration files
    // [https://root.cern/doc/master/xmlreadfile_8C.html]
    int readXMLinput(std::string simname = ""){
        
        xmlfilename = xmlpath + "/" + simname + ".xml";
        
        std::cout << "opened input xml:" << std::endl << xmlfilename << std::endl;
        PrintLine();
        // create xml engine
        TXMLEngine xml;
        // Now try to parse xml file
        // Only file with restricted xml syntax are supported
        XMLDocPointer_t xmldoc = xml.ParseFile( xmlfilename.c_str() );
        if (!xmldoc) {
            Debug(0, "Problem with input XML file! exiting.");
            return 0;
            
        }
        
        // my parsing
        XMLNodePointer_t ScintillatorNode = xml.DocGetRootElement(xmldoc);
        
        std::cout << xml.GetNodeName(ScintillatorNode) << std::endl;
        XMLNodePointer_t child = xml.GetChild(ScintillatorNode);
        while (child != 0) {
            
            std::cout << xml.GetNodeName(child) << ": " << xml.GetNodeContent(child) << std::endl;
            
            if ( strcmp(xml.GetNodeName(child),"refractiveIndex") == 0) {
                
                refractiveIndex = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"PhotonsPerMeV") == 0) {
                
                PhotonsPerMeV = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"length") == 0) {
                
                length = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"width") == 0) {
                
                width = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"thickness") == 0) {
                
                thickness = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"AbsorbtionLength") == 0) {
                
                AbsorbtionLength = std::stod(xml.GetNodeContent(child));
                
            }
            
            child = xml.GetNext(child);
        }
        
        // proton gun
        pGunPosition = TVector3();
        pGunDirection = TVector3();
        XMLNodePointer_t ProtonGunNode = xml.GetNext(ScintillatorNode);
        std::cout << xml.GetNodeName(ProtonGunNode) << std::endl;
        child = xml.GetChild(ProtonGunNode);
        while (child != 0) {
            
            std::cout << xml.GetNodeName(child) << ": " << xml.GetNodeContent(child) << std::endl;
            
            if ( strcmp(xml.GetNodeName(child),"Energy") == 0) {
                
                pGunEnergy = std::stod(xml.GetNodeContent(child));
                
            } else if ( strcmp(xml.GetNodeName(child),"x") == 0) {
                
                pGunPosition.SetX( std::stod(xml.GetNodeContent(child)) );
                
            } else if ( strcmp(xml.GetNodeName(child),"y") == 0) {
                
                pGunPosition.SetY( std::stod(xml.GetNodeContent(child)) );
                
            } else if ( strcmp(xml.GetNodeName(child),"z") == 0) {
                
                pGunPosition.SetZ( std::stod(xml.GetNodeContent(child)) );
                
            } else if ( strcmp(xml.GetNodeName(child),"vx") == 0) {
                
                pGunDirection.SetX( std::stod(xml.GetNodeContent(child)) );
                
            } else if ( strcmp(xml.GetNodeName(child),"vy") == 0) {
                
                pGunDirection.SetY( std::stod(xml.GetNodeContent(child)) );
                
            } else if ( strcmp(xml.GetNodeName(child),"vz") == 0) {
                
                pGunDirection.SetZ( std::stod(xml.GetNodeContent(child)) );
                
            }
            child = xml.GetNext(child);
        }
        
        
        
        // display
        ShowEveryNPhotons = 1000;
        XMLNodePointer_t DisplayNode = xml.GetNext(ProtonGunNode);
        std::cout << xml.GetNodeName(DisplayNode) << std::endl;
        child = xml.GetChild(DisplayNode);
        while (child != 0) {
            
            std::cout << xml.GetNodeName(child) << ": " << xml.GetNodeContent(child) << std::endl;
            
            if ( strcmp(xml.GetNodeName(child),"ShowEveryNPhotons") == 0) {
                
                ShowEveryNPhotons = std::stoi(xml.GetNodeContent(child));
                
            }
            child = xml.GetNext(child);
        }
        
        
        // waveguide
        WaveguideWidth = 2 * 10 * 2.54; // [mm] since its 2''
        WaveguideLength = 100; // [mm]
        XMLNodePointer_t WaveguideNode = xml.GetNext(DisplayNode);
        std::cout << xml.GetNodeName(WaveguideNode) << std::endl;
        child = xml.GetChild(WaveguideNode);
        while (child != 0) {
            
            std::cout << xml.GetNodeName(child) << ": " << xml.GetNodeContent(child) << std::endl;
            
            if ( strcmp(xml.GetNodeName(child),"WaveguideWidth") == 0) {
                
                WaveguideWidth = std::stod(xml.GetNodeContent(child));
                
            }
            else if ( strcmp(xml.GetNodeName(child),"WaveguideLength") == 0) {
                
                WaveguideLength = std::stod(xml.GetNodeContent(child));
                
            }
            child = xml.GetNext(child);
        }
        
        // Release memory before exit
        xml.FreeDoc(xmldoc);
        
        PrintLine();
        return 1;
    }
    
    
    // csv files
    void open_photons_csv(std::string simname = "",std::string header = "i,ArrivedAtFrontFacet"){
        Debug(0,"version::open_photons_csv()");
        
        csvfilename = csvpath + "/" + simname + "_photons.csv";
        csvfile.open( csvfilename );
        csvfile << header << std::endl;
        
        Debug(0,Form("opened output csv:\n%s",csvfilename.c_str()));
    }
    void write_photons_csv( std::vector<double> values ){
        // for (auto v : values){ std::cout << v << ","; } std::cout << std::endl;
        
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

