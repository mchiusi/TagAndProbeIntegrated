#ifndef ELETAGANDPROBEFILTER_H
#define ELETAGANDPROBEFILTER_H

#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/MessageLogger/interface/MessageLogger.h>
#include <FWCore/Utilities/interface/InputTag.h>
#include <DataFormats/PatCandidates/interface/Tau.h>
#include <DataFormats/PatCandidates/interface/Muon.h>
#include <DataFormats/PatCandidates/interface/MET.h>
#include <DataFormats/PatCandidates/interface/Electron.h>
#include <DataFormats/PatCandidates/interface/CompositeCandidate.h>

#include <iostream>
#include <utility>
#include <vector>

using namespace edm;
using namespace std;
// using namespace reco;


class EleTagAndProbeFilter : public edm::one::EDFilter<> {

    public:
        EleTagAndProbeFilter(const edm::ParameterSet &);
        ~EleTagAndProbeFilter();

    private:
        bool filter(edm::Event &, edm::EventSetup const&);

        float ComputeMT(math::XYZTLorentzVector visP4, const pat::MET& met);

        EDGetTokenT<pat::TauRefVector>   _tausTag;
        EDGetTokenT<pat::MuonRefVector>  _muonsTag;
        EDGetTokenT<pat::METCollection>  _metTag;
        EDGetTokenT<edm::View<pat::Electron> >  _electronsTag;
        bool _useWMassCuts;
};

EleTagAndProbeFilter::EleTagAndProbeFilter(const edm::ParameterSet & iConfig) :
_tausTag  (consumes<pat::TauRefVector>  (iConfig.getParameter<InputTag>("taus"))),
_muonsTag (consumes<pat::MuonRefVector> (iConfig.getParameter<InputTag>("muons"))),
_metTag   (consumes<pat::METCollection> (iConfig.getParameter<InputTag>("met"))),
_electronsTag (consumes<edm::View<pat::Electron>> (iConfig.getParameter<edm::InputTag>("electrons")))
{
    produces <pat::TauRefVector>  ();
    produces <pat::MuonRefVector> ();
    _useWMassCuts = iConfig.getParameter<bool>("useWMassCuts");
}

EleTagAndProbeFilter::~EleTagAndProbeFilter()
{}

bool EleTagAndProbeFilter::filter(edm::Event & iEvent, edm::EventSetup const& iSetup)
{

    std::unique_ptr<pat::MuonRefVector> resultMuon ( new pat::MuonRefVector );
    std::unique_ptr<pat::TauRefVector>  resultTau  ( new pat::TauRefVector  );

    // ---------------------   search for the tag in the event --------------------
    Handle<pat::MuonRefVector> muonHandle;
    iEvent.getByToken (_muonsTag, muonHandle);

    // reject events with more than 1 mu in the event (reject DY)
    // or without mu (should not happen in SingleMu dataset)
    if (muonHandle->size() != 1) return false;
    const pat::MuonRef mu = (*muonHandle)[0] ;

    //---------------------   get the met for mt computation etc. -----------------
    Handle<pat::METCollection> metHandle;
    iEvent.getByToken (_metTag, metHandle);
    const pat::MET& met = (*metHandle)[0];
    cout<<"met.pt() = "<<met.pt()<<endl;
    cout<<"met.uncorPt() = "<<met.uncorPt()<<endl;
    cout<<"met.uncorPhi() = "<<met.uncorPhi()<<endl;

    float mt = ComputeMT (mu->p4(), met);

    if (mt >= 30) return false; // reject W+jets

    Handle<pat::TauRefVector> tauHandle;
    iEvent.getByToken (_tausTag, tauHandle);
    if (tauHandle->size() < 1) return false;

    //vector<pair<float, int>> tausIdxPtVecSS;
    //vector<pair<float, int>> tausIdxPtVecOS;
    vector<pair<float, int>> tausIdxPtVec;
    for (uint itau = 0; itau < tauHandle->size(); ++itau)
    {
        const pat::TauRef tau = (*tauHandle)[itau] ;
        math::XYZTLorentzVector pSum = mu->p4() + tau->p4();
        if (pSum.mass() <= 40 || pSum.mass() >= 80) continue; // visible mass in (40, 80)
        if (deltaR(*tau, *mu) < 0.5) continue;

        // max pt
//        if (mu -> charge() / tau -> charge() > 0 ){
//            tausIdxPtVecSS.push_back(make_pair(tau -> pt(), itau));
//        } else {
//             tausIdxPtVecOS.push_back(make_pair(tau -> pt(), itau));
//        }

        // min iso
        //float isoMVA = tau->tauID("byIsolationMVArun2v1DBoldDMwLTraw");
        //if (mu -> charge() / tau -> charge() > 0 ){
        //    tausIdxPtVecSS.push_back(make_pair(isoMVA, itau));
        //} else {
        //    tausIdxPtVecOS.push_back(make_pair(isoMVA, itau));
        //}

        // min iso
        float isoMVA = tau->tauID("byIsolationMVArun2v1DBoldDMwLTraw");
        tausIdxPtVec.push_back(make_pair(isoMVA, itau));

    }


    pat::TauRef tau;

    /*
    if (tausIdxPtVecOS.size() != 0)
    {
        if (tausIdxPtVecOS.size() > 1) sort (tausIdxPtVecOS.begin(), tausIdxPtVecOS.end()); // will be sorted by first idx i.e. highest pt
        // int tauIdx = tausIdxPtVecOS.back().second; // max pt
        int tauIdx = tausIdxPtVecOS.at(0).second; // min iso
        tau = (*tauHandle)[tauIdx];
    } else if (tausIdxPtVecSS.size() != 0 )
    {
        if (tausIdxPtVecSS.size() > 1) sort (tausIdxPtVecSS.begin(), tausIdxPtVecSS.end()); // will be sorted by first idx i.e. highest pt
        // int tauIdx = tausIdxPtVecSS.back().second; // max pt
        int tauIdx = tausIdxPtVecSS.at(0).second; // min iso
        tau = (*tauHandle)[tauIdx];
    } else return false;//They are both 0!
    */

    if (tausIdxPtVec.size() == 0) return false; //No tau found
    if (tausIdxPtVec.size() > 1) sort (tausIdxPtVec.begin(), tausIdxPtVec.end()); //Sort if multiple taus
    int tauIdx = tausIdxPtVec.back().second; // min iso --> max MVA score
    tau = (*tauHandle)[tauIdx];

    resultTau->push_back (tau);
    resultMuon->push_back (mu);
    iEvent.put(std::move(resultMuon));
    iEvent.put(std::move(resultTau));
    // iEvent.put(resultMuon);
    // iEvent.put(resultTau);

    return true;
}

float EleTagAndProbeFilter::ComputeMT (math::XYZTLorentzVector visP4, const pat::MET& met)
{
  math::XYZTLorentzVector METP4 (met.uncorPt()*TMath::Cos(met.uncorPhi()), met.uncorPt()*TMath::Sin(met.uncorPhi()), 0, met.uncorPt());
  cout<<"MET Px = "<<met.uncorPt()*TMath::Cos(met.uncorPhi())<<endl;
    float scalSum = met.uncorPt() + visP4.pt();

    // math::XYZTLorentzVector METP4 (met.px(), met.py(), 0, met.pt());
    // float scalSum = met.pt() + visP4.pt();

    math::XYZTLorentzVector vecSum (visP4);
    vecSum += METP4;
    float vecSumPt = vecSum.pt();
    return sqrt (scalSum*scalSum - vecSumPt*vecSumPt);
}

#include <FWCore/Framework/interface/MakerMacros.h>
DEFINE_FWK_MODULE(EleTagAndProbeFilter);

#endif
