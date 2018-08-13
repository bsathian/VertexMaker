import FWCore.ParameterSet.Config as cms
import CMS3.NtupleMaker.configProcessName as configProcessName

hypDilepVertexMaker = cms.EDProducer("HypDilepVertexMaker",
	aliasPrefix = cms.untracked.string("hyp_FVFit"),
                                   recomuonsInputTag = cms.InputTag("slimmedMuons","",configProcessName.name),
                                   cms2muonsInputTag = cms.InputTag("muonMaker","musp4"),
                                   recoelectronsInputTag = cms.InputTag("slimmedElectrons","",configProcessName.name),
                                   cms2electronsInputTag = cms.InputTag("electronMaker","elsp4"),
                                   hypllIdInputTag = cms.InputTag("hypDilepMaker","hypllid"),
                                   hypllIndexInputTag = cms.InputTag("hypDilepMaker","hypllindex"),
                                   hypltIdInputTag = cms.InputTag("hypDilepMaker","hypltid"),
                                   hypltIndexInputTag = cms.InputTag("hypDilepMaker","hypltindex")
                                   )
