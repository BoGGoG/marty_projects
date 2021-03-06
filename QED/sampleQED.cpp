#include <marty.h>

using namespace csl;
using namespace mty;

int main()
{

    Model QED;
    AddGaugedGroup(QED, group::Type::U1, "U1_em", constant_s("e"));
    Init(QED);

    Particle electron = diracfermion_s("e", QED);
    SetMass(electron, "m_e");
    SetGroupRep(electron, "U1_em", {-1, 1});
    AddParticle(QED, electron);
    Rename(QED, "A_U1_em", "A");
    Particle photon = GetParticle(QED, "A");

    std::cout << QED << std::endl;

    auto rules = ComputeFeynmanRules(QED);
    // Display(rules);
    // Show(rules);

    std::cout << "--------------" << std::endl << std::endl;
    std::cout << "e+photon -> e+photon" << std::endl;
    auto process_ampl = QED.computeAmplitude(Order::TreeLevel,
                                        {Incoming(electron),
                                         Incoming(photon),
                                         Outgoing(electron),
                                         Outgoing(photon)});

    for (size_t i = 0; i!=process_ampl.size(); i++){
        std::cout << "i = " << i << std::endl;
        auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
        Display(diagram_ampl_eval);
    }


    std::cout << "--------------" << std::endl << std::endl;
    std::cout << "e+e -> e+e" << std::endl;
    process_ampl = QED.computeAmplitude(Order::TreeLevel,
                                     {Incoming(electron),
                                      Incoming(electron),
                                      Outgoing(electron),
                                      Outgoing(electron)});



    for (size_t i = 0; i!=process_ampl.size(); i++){
        std::cout << "i = " << i << std::endl;
        auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
        Display(diagram_ampl_eval);
    }
    // Display(diff);
    // Show(diff);
    // DisplayAbbreviations();

    return 0;
}
