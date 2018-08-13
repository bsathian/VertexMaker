// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      HypDilepVertexMaker
//
/**\class HypDilepVertexMaker HypDilepVertexMaker.h CMS3/NtupleMaker/interface/HypDilepVertexMaker.h

 Description: fit the hyp leptons to a single vertex and add
              relevant branches to output
*/
//
// Original Author:  slava77
//         Created:  Thu Dec  17 11:07:38 CDT 2009
// $Id: HypDilepVertexMaker.h,v 1.3 2010/03/03 04:19:43 kalavase Exp $
//
//
#ifndef CMS3_HYPDILEPVERTEXMAKER_H
#define CMS3_HYPDILEPVERTEXMAKER_H

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"

//#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
//#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "CMS3/NtupleMaker/interface/MatchUtilities.h"
#include "Math/VectorUtil.h"
#include "TMath.h"

typedef math::XYZTLorentzVectorF LorentzVector;

//
// class declaration
//

class HypDilepVertexMaker : public edm::EDProducer {
 public:
  explicit HypDilepVertexMaker (const edm::ParameterSet&);

 private:
  //  virtual void beginJob();
  virtual void produce(edm::Event&, const edm::EventSetup&);
  //  virtual void endJob();

  // ----------member data --------------------------- 
   std::string aliasprefix_;

    //new stuff for new CMSSW
    
    edm::EDGetTokenT<std::vector<LorentzVector>> musp4Token;
    edm::EDGetTokenT<std::vector<LorentzVector>> elsp4Token;
    edm::EDGetTokenT<std::vector<pat::Muon>> recoMuonToken;
    edm::EDGetTokenT<std::vector<pat::Electron>> recoElectronToken;
    edm::EDGetTokenT<std::vector<int>> hypllIdToken;
    edm::EDGetTokenT<std::vector<int>> hypltIdToken;
    edm::EDGetTokenT<std::vector<int>> hypllIndexToken;
    edm::EDGetTokenT<std::vector<int>> hypltIndexToken;
};

#endif //CMS2_HYPDILEPVERTEXMAKER_H
