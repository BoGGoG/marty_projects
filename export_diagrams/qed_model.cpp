#include "marty.h"
#include <fstream>
using namespace std;
using namespace csl;
using namespace mty;






int main() {

Model QED_Model;

Expr psi = constant_s("e");
QED_Model.addGaugedGroup(group::Type::U1, "em", psi);



QED_Model.init();

Particle e = diracfermion_s("e", QED_Model);
Particle mu = diracfermion_s("mu", QED_Model);
Particle t = diracfermion_s("t", QED_Model);
Particle u = diracfermion_s("u", QED_Model);
Particle d = diracfermion_s("d", QED_Model);
Particle s = diracfermion_s("s", QED_Model);
Particle tt = diracfermion_s("tt", QED_Model);
Particle c = diracfermion_s("c", QED_Model);
Particle b = diracfermion_s("b", QED_Model);



auto m_e = constant_s("m_e");
auto m_mu = constant_s("m_mu");
auto m_t = constant_s("m_t");
auto m_u = constant_s("m_u");
auto m_d = constant_s("m_d");
auto m_s = constant_s("m_s");
auto m_tt = constant_s("m_tt");
auto m_c = constant_s("m_c");
auto m_b = constant_s("m_b");


e->setGroupRep("em", -1);
mu->setGroupRep("em", -1);
t->setGroupRep("em", -1);
u->setGroupRep("em", {2,3});
d->setGroupRep("em", {-1,3});
s->setGroupRep("em", {-1,3});
tt->setGroupRep("em", {2,3});
c->setGroupRep("em", {2,3});
b->setGroupRep("em", {-1,3});


e->setMass(m_e);
mu->setMass(m_mu);
t->setMass(m_t);
u->setMass(m_u);
d->setMass(m_d);
s->setMass(m_s);
tt->setMass(m_tt);
c->setMass(m_c);
b->setMass(m_b);


QED_Model.addParticle(e);
QED_Model.addParticle(mu);
QED_Model.addParticle(t);
QED_Model.addParticle(u);
QED_Model.addParticle(d);
QED_Model.addParticle(s);
QED_Model.addParticle(tt);
QED_Model.addParticle(c);
QED_Model.addParticle(b);



QED_Model.renameParticle("A_em", "A");
QED_Model.refresh();



 auto ampl = QED_Model.computeAmplitude(
        Order::TreeLevel,
        {Incoming(e), Incoming(e),
        Outgoing(e), Outgoing("A") ,Outgoing(e) }
      );

   for (size_t n = 0; n != ampl.size(); n++) {
                    Expr amp = ampl.expression(n);
                   Evaluate(amp, csl::eval::abbreviation);
                    csl::option::printIndexIds = false;
                    cout << amp << '\n';


                  Expr  squaredAmpl = QED_Model.computeSquaredAmplitude(
                    Amplitude(ampl.getOptions(), {ampl.getDiagrams()[n]}, ampl.getKinematics())
                   );
                   Evaluate(squaredAmpl, csl::eval::abbreviation);

                    cout << squaredAmpl << '\n';

                }


 return 0;

}
