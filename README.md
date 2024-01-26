# L1 Ntuple production

## Environment setup
```bash
cmsrel CMSSW_13_3_0
cd CMSSW_13_3_0/src
cmsenv
git cms-init
git cms-addpkg L1Trigger/L1TCalorimeter
git cms-addpkg L1Trigger/L1TNtuples
git cms-addpkg L1Trigger/Configuration
git cms-addpkg L1Trigger/L1TGlobal
git cms-addpkg L1Trigger/L1TCommon
git cms-addpkg L1Trigger/L1TZDC
mkdir L1Trigger/L1TZDC/data
cd L1Trigger/L1TZDC/data
wget https://raw.githubusercontent.com/cms-data/L1Trigger-L1TCalorimeter/master/zdcLUT_HI_v0_1.txt
cd -
git clone https://github.com/cms-l1t-offline/L1Trigger-L1TCalorimeter.git L1Trigger/L1TCalorimeter/data
git cms-checkdeps -A -a
scram b -j 8 
```

Using the LLR machines to produce the ntuples it is necessary to reset a specific environment variable:
```bash
export SITECONFIG_PATH=/cvmfs/cms.cern.ch/SITECONF/local
```

In any case it is better to check what's written in [this](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideL1TStage2Instructions) twiki page, which should contain the latest configuration recipe.

### Re-emulating Run 3 data (2023D) with new (latest 2023) calibrations
```bash
cmsDriver.py l1Ntuple -s RAW2DIGI --python_filename=data.py -n 100 --no_output --era=Run3 --data --conditions=130X_dataRun3_Prompt_v4 --customise=L1Trigger/Configuration/customiseReEmul.L1TReEmulFromRAW  --customise=L1Trigger/L1TNtuples/customiseL1Ntuple.L1NtupleRAWEMU --customise=L1Trigger/Configuration/customiseSettings.L1TSettingsToCaloParams_2023_v0_4 --filein=/store/data/Run2023D/EphemeralZeroBias0/RAW/v1/000/370/293/00000/0545057e-416f-49e0-8ffb-fdca37061d4e.root
```

### Re-emulating Run-3 MC (Run3Winter2024 campaign) with latest 2023 calibrations
```bash
cmsDriver.py l1Ntuple -s RAW2DIGI --python_filename=mc.py -n 100 --no_output --era=Run3 --mc --conditions=133X_mcRun3_2024_realistic_v8 --customise=L1Trigger/Configuration/customiseReEmul.L1TReEmulMCFromRAWSimHcalTP --customise=L1Trigger/L1TNtuples/customiseL1Ntuple.L1NtupleRAWEMUGEN_MC --customise=L1Trigger/Configuration/customiseSettings.L1TSettingsToCaloParams_2023_v0_4 --filein=/store/mc/Run3Winter24Digi/SingleNeutrino_Pt-2To20-gun/GEN-SIM-RAW/133X_mcRun3_2024_realistic_v8-v2/2540000/038bda40-23b3-4038-a546-6397626ae3e2.root
```

