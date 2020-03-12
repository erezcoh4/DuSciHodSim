#include <iostream>
#include <ProtonModule/Proton.hpp>
using namespace std;
#define MAXPROTONSTEPS 5000

// ------------------------------------------------------- //
Proton::Proton (double ftime, int fverbose):
TPolyLine3D( 2 ){
    
    Npoints = 2;
    verbose = fverbose;
    Debug( 1 , Form("Proton::Proton(), verbosity %d",verbose));
    r = new TRandom3(0);
    // set a new seed every time
    r->SetSeed(r->Integer(100000));
    GenerateEdepCurves();
    GenerateMeV2MeVeeCurve();
    DoProduceScintillationPhotons = true;
    NScintillationPhotons = 0;
    time = ftime; // time since proton enters scintillator, in [sec]
}




// ------------------------------------------------------- //
void Proton::PrintTrajectory(){
    std::cout << "proton trajectory: ";
    PrintTVector3( ProductionPosition );
    PrintTVector3( ProductionDirection );
    PrintTVector3( EndPosition );
}

// ------------------------------------------------------- //
void Proton::DrawTrajectory(int trajColor){
    Debug(2,"Proton::DrawTrajectory()");
    PrintTrajectory();
    this -> SetLineColor( trajColor );
    this -> Draw();
}


// ------------------------------------------------------- //
void Proton::Shoot( Bar * bar , auxiliary * aux , bool fdebugMode, Waveguide * waveguide, WaveguideMargin * wgmargin){
    Debug(2, "Proton::Shoot()");
    debugMode = fdebugMode;
    if (debugMode) Debug(0, "working in debug mode");
    // proton will be fired from ProductionPosition at ProductionDirection
    // if the proton will cross the scintillation bar
    // (which is supppose to be the case every time and can be easily verified using geometrical calculations, then operate Proton::PropagateInPaddle(),
    // which will compute proton energy loss in steps of 1 mm, produce scintillatio photons per this mm
    // (that we compute their propagate charecteristics),
    // and subsequently loose momentum
    // and decide at which point the proton will exit the scintillation bar and stop producing photons.
    
    this -> SetPoint(0,ProductionPosition.X(),ProductionPosition.Y(),ProductionPosition.Z());
    EndPosition = ProductionPosition + 500 * ProductionDirection;

    bool ProtonWasInScintillator = false;
    bool InScintillator = false;
    double dx_cm = 0.5; // the step length, measured in [cm]
    for (int stepIdx=0; stepIdx<MAXPROTONSTEPS ; stepIdx++){
        // step through proton propagation until
        // it either exists the scintillator or the number of steps exceeds MAXPROTONSTEPS
        TVector3 StepPos = ProductionPosition + ProductionDirection * (0.5 * (dx_cm*10) * (2*stepIdx-1));
                        
        if ( bar->ContainsPoint(StepPos) ){ // since a certain point the proton enters the scintillator
            InScintillator = true;
            double Edep_MeV = GetEdep( dx_cm );
            // we evaluate the number of scintillation photons produced from photon yield in MeVee,
            // since these are the units in which the scintillators vendor work.
            // and so we convert here from MeV to MeVee
            double photonYield_MeVee = MeVee_vs_MeV_proton_s3 -> Eval( Edep_MeV );
            Int_t Nphotons = r->Poisson((bar -> GetPhotonsPerMeV() * photonYield_MeVee));
            
            TickTime( dx_cm , bar -> GetRefractiveIndex() );
            
            Debug( 1, Form("photonYield MeVee: %.3f MeVee, out of %.2f produced %d photons",
                           photonYield_MeVee,bar -> GetPhotonsPerMeV() * photonYield_MeVee,Nphotons) );
            
            if (DoProduceScintillationPhotons){ // flag to suppress this for debugging purposes
                ProduceScintillationPhotons( bar, aux, Nphotons, StepPos , waveguide , wgmargin );
            }
            
            // of course, when the proton produced scintillation photons it looses some energy...
            UpdateProtonEnergy( -Edep_MeV );
            // and at least one time we want to flag that the proton entered the scintillator
            ProtonWasInScintillator = true;
            
            if (debugMode){
                // if in debug mode produce only one photon, once.
                // so stop producing scintillation photons...
                DoProduceScintillationPhotons = false;
            }
            
            Debug(2, Form("proton gave %.2f MeV (Ep=%.2f MeV), producing %d scintillation photons",Edep_MeV,Ep,Nphotons));
        } else {
            InScintillator = false;
            // at some point the proton trajectory will exit the scintillator
            // in this case, if the proton was already in the scintillator, we stop monitoring the proton behaviour
            Debug(2, Form("paddle does not contain step %d at (%.1f,%.1f,%.1f)",stepIdx,StepPos.X(),StepPos.Y(),StepPos.Z()));
            if (ProtonWasInScintillator){
                // proton exit scintillator
                EndPosition = StepPos + 100 * ProductionDirection;
                this -> SetPoint(1,EndPosition.X(),EndPosition.Y(),EndPosition.Z());
                break;
            }
        }
        Debug(2, Form("proton propagating stepIdx %d at (%.1f,%.1f,%.1f)",stepIdx,StepPos.X(),StepPos.Y(),StepPos.Z()));
        
        if (stepIdx%10==0) {
            if (InScintillator){
                Debug(-1, Form("proton propagating in scintillator stepIdx %d at (%.1f,%.1f,%.1f)",stepIdx,StepPos.X(),StepPos.Y(),StepPos.Z()));
            } else {
                Debug(-1, Form("proton propagating outside scitillator stepIdx %d at (%.1f,%.1f,%.1f)",stepIdx,StepPos.X(),StepPos.Y(),StepPos.Z()));
            }
        }
    }
    Debug(-1, Form("done stepping through proton propagation, produced %d scintillation photons in paddle", GetNScintillationPhotons()));
    
    if (DoDrawScene) {
        DrawTrajectory();
    }
}


// ------------------------------------------------------- //
void Proton::TickTime( double dx , double n ){
    Debug(1 ,"Proton::TickTime()");
    // compute how much time in [sec] it took for the proton to move dx in [cm]
    // first compute the velocity
    // E = m*c2*gamma = m*c3*(1/sqrt(c2-v2))
    // v2 = c2 (1 - c4*m2/E2)
    // beta = sqrt(1 - (m/E)), or v = c sqrt(1 - (mc2/E))
    double beta = sqrt( 1 - Mp/Ep );
    
    // now convert to cm/sec in paddle
    double c = 29979245800. / n; // 29979245800 [cm/sec] is the velocity in air
    double v = c * beta; // in [cm/sec]

    // finally, compute the time interval from the step size
    double dt = dx / v;
    
    // and update the time since the proton entered the scintillator in sec
    time += dt;
    Debug(1 ,Form("Ep=%.1f MeV, beta=%.1f, dx=%.1f cm, dt=%.1f sec., t=%.1f sec.",Ep,beta,dx,dt,time));
}

// ------------------------------------------------------- //
void Proton::GenerateMeV2MeVeeCurve(){
    // convert from proton energy deposition in [MeV] to photon yield in [MeVee]
    
    TCanvas * c = new TCanvas();
    
    // first method:
    // based on [Zhang et al., proceedings isinn-23]
    // [http://isinn.jinr.ru/proceedings/isinn-23/pdf/Zhang.pdf]
    
    Float_t proton_Edep_MeV[15]     = { 0, 0.61, 1.30, 2.09, 2.98, 4.04, 5.29, 6.52, 7.64, 8.92, 10.28, 12.63, 13.99, 15.39, 16.55};
    Float_t photonYield_MeVee[15]   = { 0, 0.08, 0.26, 0.51, 0.85, 1.33, 1.94, 2.61, 3.26, 3.96, 4.91, 6.41, 7.32, 8.24, 9.05};
    //    MeVee_vs_MeV_proton = new TGraph(15, proton_Edep_MeV, photonYield_MeVee);
    //    MeVee_vs_MeV_proton_s3 = new TSpline3("Edep [MeV] vs photon yield [MeVee]", MeVee_vs_MeV_proton);
    //
    //    MeVee_vs_MeV_proton -> Draw();
    //    MeVee_vs_MeV_proton_s3 -> Draw();
    
    // second method:
    // based on BAND studies
    
    // from Axel:
    // *********** These are parameters from a paper
    //double a1 = 0.95;
    //double a2 = 8.0;
    //double a3 = 0.1;
    //double a4 = 0.9;
    // *********** These are parameters from Or
    double a1 = 0.83;
    double a2 = 2.82;
    double a3 = 0.25;
    double a4 = 0.93;

    for (int i=0; i<15; i++){
         photonYield_MeVee[i]= (a1*proton_Edep_MeV[i]) - a2*(1.0 - exp(-a3*pow(proton_Edep_MeV[i], a4)));
    }
    MeVee_vs_MeV_proton = new TGraph(15, proton_Edep_MeV, photonYield_MeVee);
    MeVee_vs_MeV_proton_s3 = new TSpline3("Edep [MeV] vs photon yield [MeVee]", MeVee_vs_MeV_proton);
    MeVee_vs_MeV_proton -> Draw("o");
    MeVee_vs_MeV_proton_s3 -> Draw("same");

    c->SaveAs("/Users/erezcohen/Desktop/proton_MeV_to_MeVee.pdf");
    c->Close();
}

// ------------------------------------------------------- //
void Proton::GenerateEdepCurves(){
    // compute dE/dx which is the energy deposition of a proton in scintillator given its energy and dx=step_size
    // step_size is measured in [mm]
    // Ep measured in [MeV/c]
    // dE/dx measured in [MeV/c]
    //
    // based on proton energy loss in plastic scintillator (vinyltoluene-based) by NIST
    // [https://physics.nist.gov/cgi-bin/Star/ap_table.pl]
    
    
    Float_t KE_MeV_proton[132] = // MeV
    {1.000E-03,1.500E-03,2.000E-03,2.500E-03,3.000E-03,4.000E-03,5.000E-03,6.000E-03,7.000E-03,8.000E-03,9.000E-03,1.000E-02,1.250E-02,1.500E-02,1.750E-02,2.000E-02,2.250E-02,2.500E-02,2.750E-02,3.000E-02,3.500E-02,4.000E-02,4.500E-02,5.000E-02,5.500E-02,6.000E-02,6.500E-02,7.000E-02,7.500E-02,8.000E-02,8.500E-02,9.000E-02,9.500E-02,1.000E-01,1.250E-01,1.500E-01,1.750E-01,2.000E-01,2.250E-01,2.500E-01,2.750E-01,3.000E-01,3.500E-01,4.000E-01,4.500E-01,5.000E-01,5.500E-01,6.000E-01,6.500E-01,7.000E-01,7.500E-01,8.000E-01,8.500E-01,9.000E-01,9.500E-01,1.000E+00,1.250E+00,1.500E+00,1.750E+00,2.000E+00,2.250E+00,2.500E+00,2.750E+00,3.000E+00,3.500E+00,4.000E+00,4.500E+00,5.000E+00,5.500E+00,6.000E+00,6.500E+00,7.000E+00,7.500E+00,8.000E+00,8.500E+00,9.000E+00,9.500E+00,1.000E+01,1.250E+01,1.500E+01,1.750E+01,2.000E+01,2.500E+01,2.750E+01,3.000E+01,3.500E+01,4.000E+01,4.500E+01,5.000E+01,5.500E+01,6.000E+01,6.500E+01,7.000E+01,7.500E+01,8.000E+01,8.500E+01,9.000E+01,9.500E+01,1.000E+02,1.250E+02,1.500E+02,1.750E+02,2.000E+02,2.250E+02,2.500E+02,2.750E+02,3.000E+02,3.500E+02,4.000E+02,4.500E+02,5.000E+02,5.500E+02,6.000E+02,6.500E+02,7.000E+02,7.500E+02,8.000E+02,8.500E+02,9.000E+02,9.500E+02,1.000E+03,1.500E+03,2.000E+03,2.500E+03,3.000E+03,4.000E+03,5.000E+03,6.000E+03,7.000E+03,8.000E+03,9.000E+03,1.000E+04};
    
    Float_t StoppingPower_MeV_Per_gcm2_proton[132] = // MeV / (g/cm2)
    {2.323E+02,2.667E+02,2.975E+02,3.256E+02,3.516E+02,3.927E+02,4.303E+02,4.648E+02,4.965E+02,5.250E+02,5.510E+02,5.752E+02,6.247E+02,6.663E+02,7.024E+02,7.345E+02,7.629E+02,7.878E+02,8.098E+02,8.293E+02,8.638E+02,8.935E+02,9.184E+02,9.382E+02,9.530E+02,9.635E+02,9.704E+02,9.744E+02,9.758E+02,9.748E+02,9.719E+02,9.674E+02,9.615E+02,9.544E+02,9.073E+02,8.517E+02,7.961E+02,7.441E+02,6.972E+02,6.554E+02,6.185E+02,5.859E+02,5.313E+02,4.876E+02,4.517E+02,4.218E+02,3.963E+02,3.740E+02,3.547E+02,3.375E+02,3.222E+02,3.084E+02,2.959E+02,2.845E+02,2.740E+02,2.643E+02,2.259E+02,1.983E+02,1.772E+02,1.606E+02,1.472E+02,1.360E+02,1.266E+02,1.185E+02,1.053E+02,9.490E+01,8.657E+01,7.969E+01,7.392E+01,6.899E+01,6.473E+01,6.101E+01,5.773E+01,5.482E+01,5.221E+01,4.986E+01,4.773E+01,4.579E+01,3.821E+01,3.294E+01,2.904E+01,2.604E+01,2.171E+01,2.008E+01,1.871E+01,1.651E+01,1.482E+01,1.348E+01,1.239E+01,1.149E+01,1.073E+01,1.007E+01,9.509E+00,9.014E+00,8.577E+00,8.189E+00,7.841E+00,7.528E+00,7.245E+00,6.151E+00,5.407E+00,4.867E+00,4.458E+00,4.138E+00,3.880E+00,3.668E+00,3.491E+00,3.214E+00,3.006E+00,2.846E+00,2.719E+00,2.616E+00,2.532E+00,2.462E+00,2.402E+00,2.350E+00,2.306E+00,2.267E+00,2.234E+00,2.204E+00,2.178E+00,2.033E+00,1.981E+00,1.962E+00,1.959E+00,1.970E+00,1.989E+00,2.010E+00,2.030E+00,2.049E+00,2.067E+00,2.084E+00};
    
    // [https://physics.nist.gov/cgi-bin/Star/compos.pl?refer=ap&matno=216]
    Float_t ScintillatorDensity = 1.023; //g/cm^3, see e.g. Eljen EJ204 or SaintGobain BC408
    
    Float_t dEdx_MeV_Per_cm_proton[132];
    for (int i = 0 ; i < 132; i++) {
        dEdx_MeV_Per_cm_proton[i] = StoppingPower_MeV_Per_gcm2_proton[i] * ScintillatorDensity; // [MeV / (g/cm2) * (g/cm3)]
    }
    
    dEdx_vs_KE_proton = new TGraph(132, KE_MeV_proton, dEdx_MeV_Per_cm_proton);
    dEdx_vs_KE_proton_s3 = new TSpline3("KE [MeV]  vs dE/dx [MeV/cm]", dEdx_vs_KE_proton);
    
    TCanvas * c = new TCanvas();
    c->SetLogx(); c->SetLogy();
    dEdx_vs_KE_proton_s3 -> Draw();
    c->SaveAs("/Users/erezcohen/Desktop/proton_range.pdf");
    c->Close();
}

// ------------------------------------------------------- //
double Proton::GetEdep( double dx_cm ){
    // input: dx in [cm]
    // also use KE in [MeV]
    // return: dE/dx in [MeV]
    double dEdx_MeV_Per_cm = dEdx_vs_KE_proton_s3 -> Eval( KE );
    double Edep = dEdx_MeV_Per_cm * dx_cm;
    return Edep;
}

// ------------------------------------------------------- //
void Proton::UpdateProtonEnergy( double dE ){
    Ep += dE;
    SetEnergyMomentum();
}

// ------------------------------------------------------- //
void Proton::SetEnergyMomentum(){
    KE = Ep - Mp;
    Pp = sqrt( Ep*Ep - Mp*Mp );
}

// ------------------------------------------------------- //
void Proton::ProduceScintillationPhotons( Bar * bar, auxiliary * aux,
                                         int Nphotons, TVector3 EmissionPos,
                                         Waveguide * waveguide, WaveguideMargin * wgmargin ){
    Debug(2 , "Proton::ProduceScintillationPhotons()");
    
    if (debugMode){
        Nphotons = 1;
    }
    
    // produce scintillation photons in paddle and propagate them
    for (int photonIdx=1; photonIdx <= Nphotons; photonIdx++ ) {
        
        if (verbose>1){
            PrintXLine();
            std::cout << "generating photon " << photonIdx << std::endl;
            PrintLine();
        }
        
        Photon * photon = new Photon (0, verbose);
        photon -> SetProductionPosition( EmissionPos );
        photon -> EmitIsotropically( time , bar );
        photon -> PropagateInPaddle( bar );
        
        // if the photon reached front facet, move to waveguide...
        if (photon->GetArrivedAtFrontFacet()){
            photon->PropagateInWaveguide( waveguide );
            
            // if the photon reached waveguide exit, move to waveguide margin...
            if (photon->GetArrivedAtWaveguideExit()){
                photon->PropagateInWaveguideMargin( wgmargin );
            }
        }

        // write photon story into output csv
        // when adding values here, also add the corresponding header label at
        // program.cpp
        // in "open output csv files" (around line 59)
        aux->write_photons_csv( {
            (double)photon->GetArrivedAtFrontFacet(),
            (double)photon->GetArrivedAtWaveguideExit(),
            (double)photon->GetArrivedAtWaveguideMarginExit(),
            (double)photon->GetDirectFromProduction(),
            (double)photon->GetAbsorbedInScintillator(),
            (double)photon->GetAbsorbedInWaveguide(),
            (double)photon->GetReadOutByDetector(),
            (double)photon->GetHitBackFacet(),
            
            photon->GetProductionDirection().X(),
            photon->GetProductionDirection().Y(),
            photon->GetProductionDirection().Z(),
            
            photon->GetTotalPathLengthInScintillator(),
            photon->GetTotalPathLengthInWaveguide(),
            photon->GetTotalPathLength(),
            photon->GetTimeFromStart(),
            
            photon->GetHitFrontFacetPos().X(),
            photon->GetHitFrontFacetPos().Y(),
            photon->GetHitFrontFacetPos().Z(),
            
            photon->GetHitWaveguideExitPos().X(),
            photon->GetHitWaveguideExitPos().Y(),
            photon->GetHitWaveguideExitPos().Z(),
            
            photon->GetHitWaveguideMarginExitPos().X(),
            photon->GetHitWaveguideMarginExitPos().Y(),
            photon->GetHitWaveguideMarginExitPos().Z()
        } );
        
        aux->UpdatePhotonCounts(
                                photon->GetArrivedAtFrontFacet(),
                                photon->GetArrivedAtWaveguideExit(),
                                photon->GetAbsorbedInScintillator(),
                                photon->GetAbsorbedInWaveguide(),
                                photon->GetAbsorbedInWaveguideMargin()
                                );
        
        
        if (DoDrawScene && (NScintillationPhotons % ShowEveryNPhotons == 0)) {
            // draw every Nth photon
            photon -> PrintPath();
            photon -> Draw();
        }
        
        if (verbose>1){
            std::cout << "done photon " << photonIdx << std::endl;
            PrintLine();
        }
        AddScintillationPhoton();
    }
    
  
}
