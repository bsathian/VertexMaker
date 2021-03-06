#include "CMS3/NtupleMaker/interface/HypDilepVertexMaker.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "CMS3/NtupleMaker/interface/MatchUtilities.h"
#include "Math/VectorUtil.h"
#include "TMath.h"

typedef math::XYZTLorentzVectorF LorentzVector;
using namespace reco;
using namespace edm;
using namespace std;


HypDilepVertexMaker::HypDilepVertexMaker (const edm::ParameterSet& cfg) {

  aliasprefix_ = cfg.getUntrackedParameter<std::string>("aliasPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");


  //new stuff for new CMSSW
  musp4Token = consumes<std::vector<LorentzVector>>(cfg.getParameter<InputTag>("cms2muonsInputTag"));
  elsp4Token = consumes<std::vector<LorentzVector>>(cfg.getParameter<InputTag>("cms2electronsInputTag"));
  recoMuonToken = consumes<std::vector<pat::Muon>>(cfg.getParameter<InputTag>("recomuonsInputTag"));
  recoElectronToken = consumes<std::vector<pat::Electron>>(cfg.getParameter<InputTag>("recoelectronsInputTag"));
  hypllIdToken = consumes<std::vector<int>>(cfg.getParameter<InputTag>("hypllIdInputTag"));
  hypltIdToken = consumes<std::vector<int>>(cfg.getParameter<InputTag>("hypltIdInputTag"));
  hypllIndexToken = consumes<std::vector<int>>(cfg.getParameter<InputTag>("hypllIndexInputTag"));
  hypltIndexToken = consumes<std::vector<int>>(cfg.getParameter<InputTag>("hypltIndexInputTag"));
  

  produces<vector<int> >          (branchprefix+"status"        ).setBranchAlias(aliasprefix_+"_status"        );
  produces<vector<int> >          (branchprefix+"ndf"           ).setBranchAlias(aliasprefix_+"_ndf"           );
  produces<vector<float> >        (branchprefix+"chi2ndf"       ).setBranchAlias(aliasprefix_+"_chi2ndf"       );
  produces<vector<float> >        (branchprefix+"prob"          ).setBranchAlias(aliasprefix_+"_prob"          );
  produces<vector<float> >        (branchprefix+"v4cxx"         ).setBranchAlias(aliasprefix_+"_v4cxx"         );
  produces<vector<float> >        (branchprefix+"v4cxy"         ).setBranchAlias(aliasprefix_+"_v4cxy"         );
  produces<vector<float> >        (branchprefix+"v4cyy"         ).setBranchAlias(aliasprefix_+"_v4cyy"         );
  produces<vector<float> >        (branchprefix+"v4czz"         ).setBranchAlias(aliasprefix_+"_v4czz"         );
  produces<vector<float> >        (branchprefix+"v4czx"         ).setBranchAlias(aliasprefix_+"_v4czx"         );
  produces<vector<float> >        (branchprefix+"v4czy"         ).setBranchAlias(aliasprefix_+"_v4czy"         );
  produces<vector<LorentzVector> >(branchprefix+"p4"            ).setBranchAlias(aliasprefix_+"_p4"            );
  produces<vector<LorentzVector> >(branchprefix+"v4"            ).setBranchAlias(aliasprefix_+"_v4"            );
  
}

void HypDilepVertexMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  edm::ESHandle<TransientTrackBuilder> ttrackBuilder;
  es.get<TransientTrackRecord>().get("TransientTrackBuilder",ttrackBuilder);

  Handle<vector<LorentzVector> > mus_p4_h;
    ev.getByToken(musp4Token,mus_p4_h);
  const vector<LorentzVector>* mus_p4 = mus_p4_h.product(); 

  Handle<vector<pat::Muon> > mus_h;
    ev.getByToken(recoMuonToken,mus_h);
  vector<pat::Muon> mus = *(mus_h.product());
  MatchUtilities::alignToLVector(*mus_p4, mus);

  Handle<vector<LorentzVector> > els_p4_h;
  ev.getByToken(elsp4Token,els_p4_h);
  const vector<LorentzVector>* els_p4 = els_p4_h.product(); 

  Handle<vector<pat::Electron> > els_h;
    ev.getByToken(recoElectronToken,els_h);
  vector<pat::Electron> els = *(els_h.product());
  MatchUtilities::alignToLVector(*els_p4, els);

  //now read hyp-related stuff
  Handle<vector<int> > hyp_ll_id_h;
  ev.getByToken(hypllIdToken,hyp_ll_id_h);
  const vector<int>* hyp_ll_id = hyp_ll_id_h.product();

  Handle<vector<int> > hyp_ll_index_h;
  ev.getByToken(hypllIndexToken,hyp_ll_index_h);
  const vector<int>* hyp_ll_index = hyp_ll_index_h.product();
  
  Handle<vector<int> > hyp_lt_id_h;
  ev.getByToken(hypltIdToken,hyp_lt_id_h);
  const vector<int>* hyp_lt_id = hyp_lt_id_h.product();

  Handle<vector<int> > hyp_lt_index_h;
  ev.getByToken(hypltIndexToken,hyp_lt_index_h);
  const vector<int>* hyp_lt_index = hyp_lt_index_h.product();
  
  unsigned int iH = 0;
  unsigned int nH = hyp_ll_id->size();

  auto_ptr<vector<int> >           hyp_FVFit_status (new vector<int>          (nH, -9999)                                 );
  auto_ptr<vector<int> >           hyp_FVFit_ndf    (new vector<int>          (nH, -9999)                                 );
  auto_ptr<vector<float> >         hyp_FVFit_chi2ndf(new vector<float>        (nH, -9999)                                 );
  auto_ptr<vector<float> >         hyp_FVFit_prob   (new vector<float>        (nH, -9999)                                 );
  auto_ptr<vector<float> >         hyp_FVFit_v4cxx  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<float> >         hyp_FVFit_v4cxy  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<float> >         hyp_FVFit_v4cyy  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<float> >         hyp_FVFit_v4czz  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<float> >         hyp_FVFit_v4czx  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<float> >         hyp_FVFit_v4czy  (new vector<float>        (nH, -9999)                                 ); 
  auto_ptr<vector<LorentzVector> > hyp_FVFit_p4     (new vector<LorentzVector>(nH, LorentzVector(-9999,-9999,-9999,9999)));
  auto_ptr<vector<LorentzVector> > hyp_FVFit_v4     (new vector<LorentzVector>(nH, LorentzVector(-9999,-9999,-9999,9999)));


  for(; iH<nH; ++iH){
    const reco::Track* llTrack = 0;
    const reco::Track* ltTrack = 0;
    float llMass = 0.13957;
    float ltMass = 0.13957;

    int ihyp_ll_id = hyp_ll_id->at(iH);
    int ihyp_lt_id = hyp_lt_id->at(iH);
    int ihyp_ll_index = hyp_ll_index->at(iH);
    int ihyp_lt_index = hyp_lt_index->at(iH);


    if(abs(ihyp_ll_id)==11){
      if(els[ihyp_ll_index].track().isAvailable() && els[ihyp_ll_index].track().isNonnull()){
	llTrack = els[ihyp_ll_index].track().get();
      } else if(els[ihyp_ll_index].gsfTrack().isAvailable() && els[ihyp_ll_index].gsfTrack().isNonnull()){
	llTrack = els[ihyp_ll_index].gsfTrack().get();
      }
      llMass = 0.0005109989;
    }
    if(abs(ihyp_ll_id)==13){
      if(mus[ihyp_ll_index].track().isAvailable() && mus[ihyp_ll_index].track().isNonnull()){
	llTrack = mus[ihyp_ll_index].track().get();
      } //do nothing for muons without a track [might add a case for global mus with inner track coll dropped, not usefull here]
      llMass = 0.105658369;
    }

    //same for lt
    if(abs(ihyp_lt_id)==11){
      if(els[ihyp_lt_index].track().isAvailable() && els[ihyp_lt_index].track().isNonnull()){
	ltTrack = els[ihyp_lt_index].track().get();
      } else if(els[ihyp_lt_index].gsfTrack().isAvailable() && els[ihyp_lt_index].gsfTrack().isNonnull()){
	ltTrack = els[ihyp_lt_index].gsfTrack().get();
      }
      ltMass = 0.0005109989;
    }
    if(abs(ihyp_lt_id)==13){
      if(mus[ihyp_lt_index].track().isAvailable() && mus[ihyp_lt_index].track().isNonnull()){
	ltTrack = mus[ihyp_lt_index].track().get();
      } //do nothing for muons without a track [might add a case for global mus with inner track coll dropped, not usefull here]
      ltMass = 0.105658369;
    }

    if (llTrack == 0 || ltTrack == 0){
      LogDebug("HypDilepVertexMaker")<<"failed to get tracks to vertex";
      (*hyp_FVFit_status)[iH] = 2*(llTrack==0) + 4*(ltTrack==0);
      continue;
    }


    TransientTrack llTT = (*ttrackBuilder).build(llTrack);
    TransientTrack ltTT = (*ttrackBuilder).build(ltTrack);

    //this is a copy-paste from Onia2MuMu.cc
    vector<TransientTrack> t_tks;
    t_tks.push_back(llTT);
    t_tks.push_back(ltTT);
    KalmanVertexFitter kvf;
    TransientVertex tv = kvf.vertex(t_tks);
    
    if (!tv.isValid()){
      LogDebug("HypDilepVertexMaker")<<"failed to fit vertex";
      (*hyp_FVFit_status)[iH] = 8;
      continue;
    }

    (*hyp_FVFit_ndf)[iH]     = (int)tv.degreesOfFreedom();
    (*hyp_FVFit_chi2ndf)[iH] = tv.normalisedChiSquared();
    (*hyp_FVFit_prob)[iH]    = TMath::Prob(tv.totalChiSquared(), (int)tv.degreesOfFreedom());
    (*hyp_FVFit_v4cxx)[iH]   = tv.positionError().cxx();
    (*hyp_FVFit_v4cxy)[iH]   = tv.positionError().cyx();
    (*hyp_FVFit_v4cyy)[iH]   = tv.positionError().cyy();
    (*hyp_FVFit_v4czz)[iH]   = tv.positionError().czz();
    (*hyp_FVFit_v4czx)[iH]   = tv.positionError().czx();
    (*hyp_FVFit_v4czy)[iH]   = tv.positionError().czy();
    if (tv.hasRefittedTracks()){
      GlobalVector llp3Ref = tv.refittedTracks()[0].trajectoryStateClosestToPoint(tv.position()).momentum();
      GlobalVector ltp3Ref = tv.refittedTracks()[1].trajectoryStateClosestToPoint(tv.position()).momentum();
      float llE = sqrt(llp3Ref.mag2()+llMass*llMass);
      float ltE = sqrt(ltp3Ref.mag2()+ltMass*ltMass);
      //this is not particulalrly numerically stable :(
      LorentzVector llp4Ref(llp3Ref.x(), llp3Ref.y(), llp3Ref.z(), llE);
      LorentzVector ltp4Ref(ltp3Ref.x(), ltp3Ref.y(), ltp3Ref.z(), ltE);
      
      (*hyp_FVFit_p4)[iH]      = llp4Ref+ltp4Ref; 
      (*hyp_FVFit_status)[iH] = 1;
    } else {
      (*hyp_FVFit_status)[iH] = 0;
    }
    (*hyp_FVFit_v4)[iH] = LorentzVector(tv.position().x(), tv.position().y(), tv.position().z(), 0);
  }
	
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  ev.put(hyp_FVFit_status , branchprefix+"status" );
  ev.put(hyp_FVFit_ndf    , branchprefix+"ndf"    );
  ev.put(hyp_FVFit_chi2ndf, branchprefix+"chi2ndf");
  ev.put(hyp_FVFit_prob   , branchprefix+"prob"   );
  ev.put(hyp_FVFit_v4cxx  , branchprefix+"v4cxx"  );
  ev.put(hyp_FVFit_v4cxy  , branchprefix+"v4cxy"  );
  ev.put(hyp_FVFit_v4cyy  , branchprefix+"v4cyy"  );
  ev.put(hyp_FVFit_v4czz  , branchprefix+"v4czz"  );
  ev.put(hyp_FVFit_v4czx  , branchprefix+"v4czx"  );
  ev.put(hyp_FVFit_v4czy  , branchprefix+"v4czy"  );
  ev.put(hyp_FVFit_p4     , branchprefix+"p4"     );
  ev.put(hyp_FVFit_v4     , branchprefix+"v4"     );
}

//define this as a plug-in 
DEFINE_FWK_MODULE(HypDilepVertexMaker);
