#include <marty.h>
#include <map>
// #include <vector>

using namespace csl;
using namespace mty;

using std::cout; using std::cin;
using std::endl; using std::string;
using std::map; using std::copy;

std::vector<csl::Expr> square_amplitude_indivually(mty::Amplitude process_ampl, mty::Model& model){
    auto opts = process_ampl.getOptions();
    auto kinematics = process_ampl.getKinematics();
    std::vector<mty::FeynmanDiagram> diagrams = process_ampl.getDiagrams();
    std::vector<csl::Expr> squared_ampl_expressions = {};

    for(size_t i=0; i!=diagrams.size(); i++){
        std::vector<mty::FeynmanDiagram> diagram = {diagrams[i]};
        auto ampl = mty::Amplitude(opts, diagram, kinematics);
        auto square = model.computeSquaredAmplitude(ampl);
        auto square_eval = Evaluated(square, eval::abbreviation);
        squared_ampl_expressions.push_back(square_eval);
    }
    
    return squared_ampl_expressions;
};

int main(int argc, char *argv[])
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

    auto rules = ComputeFeynmanRules(QED);

    map<string, Particle> particles_dic = {
        {"electron", electron},
        {"photon", photon},
    };

    auto pin1 = particles_dic[argv[1]];
    auto pin2 = particles_dic[argv[2]];
    auto pout1 = particles_dic[argv[3]];
    auto pout2 = particles_dic[argv[4]];

    cout << "incoming 1: " << argv[1] << ", " << pin1 << endl;
    cout << "incoming 2: " << argv[2] << ", " << pin2 << endl;
    cout << "outgoing 1: " << argv[3] << ", " << pout1 << endl;
    cout << "outgoing 2: " << argv[4] << ", " << pout2 << endl;


    auto process_ampl = QED.computeAmplitude(Order::TreeLevel,
                                        {Incoming(pin1),
                                         Incoming(pin2),
                                         Outgoing(pout1),
                                         Outgoing(pout2)});


    std::vector<csl::Expr> ampl_expressions = {};
    for (size_t i = 0; i!=process_ampl.size(); i++){
        auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
        ampl_expressions.push_back(diagram_ampl_eval);
    }

    
    std::vector<csl::Expr> squared_ampl_expressions = square_amplitude_indivually(process_ampl, QED);

    std::cout << "AMPLITUDES:" << std::endl;
    for(size_t i=0; i!=ampl_expressions.size(); i++){
        cout << ampl_expressions[i] << endl;
    }

    std::cout << "SQUARED AMPLITUDES:" << std::endl;
    for(size_t i=0; i!=squared_ampl_expressions.size(); i++){
        cout << squared_ampl_expressions[i] << endl;
    }

    return 0;
}
