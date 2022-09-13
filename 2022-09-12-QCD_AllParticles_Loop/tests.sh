#!/bin/bash

./QCD_AllParticles_IO.x --particles in_up,in_anti_up,out_down,out_anti_down
# ./QED_AllParticles_IO.x --particles in_anti_electron,in_electron,out_electron,out_anti_electron
# ./QED_AllParticles_IO.x --particles in_muon,in_muon,out_muon,out_muon
# ./QED_AllParticles_IO.x --particles in_anti_muon,in_muon,out_muon,out_anti_muon
# ./QED_AllParticles_IO.x --particles in_tau,in_tau,out_tau,out_tau
# ./QED_AllParticles_IO.x --particles in_anti_tau,in_tau,out_tau,out_anti_tau
./QCD_AllParticles_IO.x --particles in_up,in_up,out_up,out_up
./QCD_AllParticles_IO.x --particles in_anti_up,in_up,out_up,out_anti_up
./QCD_AllParticles_IO.x --particles in_down,in_down,out_down,out_down
./QCD_AllParticles_IO.x --particles in_anti_down,in_down,out_down,out_anti_down
./QCD_AllParticles_IO.x --particles in_strange,in_strange,out_strange,out_strange
./QCD_AllParticles_IO.x --particles in_anti_strange,in_strange,out_strange,out_anti_strange
./QCD_AllParticles_IO.x --particles in_charm,in_charm,out_charm,out_charm
./QCD_AllParticles_IO.x --particles in_anti_charm,in_charm,out_charm,out_anti_charm
./QCD_AllParticles_IO.x --particles in_bottom,in_bottom,out_bottom,out_bottom
./QCD_AllParticles_IO.x --particles in_anti_bottom,in_bottom,out_bottom,out_anti_bottom
./QCD_AllParticles_IO.x --particles in_top,in_top,out_top,out_top
./QCD_AllParticles_IO.x --particles in_anti_top,in_top,out_top,out_anti_top
