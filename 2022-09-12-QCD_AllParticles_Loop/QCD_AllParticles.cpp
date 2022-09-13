#include "marty.h"
#include <fstream>
using namespace std;
using namespace csl;
using namespace mty;

int main() {

 Model QCD_Model;

 QCD_Model.addGaugedGroup(group::Type::SU, "C", 3, csl::constant_s("g"));
 QCD_Model.init();


 Particle u = diracfermion_s("u", QCD_Model);
 Particle d = diracfermion_s("d", QCD_Model);
 Particle s = diracfermion_s("s", QCD_Model);
 Particle t = diracfermion_s("t", QCD_Model);
 Particle c = diracfermion_s("c", QCD_Model);
 Particle b = diracfermion_s("b", QCD_Model);
 SetMass(u, "m_u");
 SetMass(d, "m_d");
 SetMass(s, "m_s");
 SetMass(t, "m_t");
 SetMass(c, "m_c");
 SetMass(b, "m_b");

 SetGroupRep(u, "C", {1, 0});  //This is color Triplet, see P.65
 SetGroupRep(d, "C", {1, 0});
 SetGroupRep(s, "C", {1, 0});
 SetGroupRep(t, "C", {1, 0});
 SetGroupRep(c, "C", {1, 0});
 SetGroupRep(b, "C", {1, 0});

 QCD_Model.addParticle(u);
 QCD_Model.addParticle(d);
 QCD_Model.addParticle(s);
 QCD_Model.addParticle(t);
 QCD_Model.addParticle(c);
 QCD_Model.addParticle(b);


 QCD_Model.renameParticle("A_C", "G");
 Particle gluon = GetParticle(QCD_Model, "G");

 auto ampl = QCD_Model.computeAmplitude(Order::TreeLevel,
     {
     Incoming(u),
     Incoming(AntiPart(u)),
     Outgoing(d),
     Outgoing(AntiPart(d))
     });

 Display(ampl);
 Show(ampl);
}
