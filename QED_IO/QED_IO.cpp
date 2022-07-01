#include <marty.h>
#include <map>
// #include <vector>
#include <typeinfo>
// #include <cxxopts.hpp>

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
        // auto square_eval = square;
        squared_ampl_expressions.push_back(square_eval);
    }
    
    return squared_ampl_expressions;
};

mty::Insertion get_insertion(string name){
    if ((name == "in_normal_electron") || (name == "in_electron"))
        return Incoming("e");
    else if (name == "in_anti_electron")
        return Incoming(AntiPart("e"));
    else if ((name == "in_photon") || (name == "in_normal_photon"))
        return Incoming("A");
    else if ((name == "out_electron") || (name == "out_normal_electron"))
        return Outgoing("e");
    else if (name == "out_anti_electron")
        return Outgoing(AntiPart("e"));
    else if ((name == "out_photon") || (name == "out_normal_photon"))
        return Outgoing("A");
    else {
    cout << "particle" << name << "not found, returning electron" << endl;
    return Incoming("e");
    }
}


/*
 * argv[1]-argv[4]: 
*/
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
    Particle c_A = QED.getParticle("c_A");

    auto rules = ComputeFeynmanRules(QED);

    std::vector<mty::Insertion> insertions;
    for (size_t i = 1; i!= argc; i++){
        insertions.push_back(get_insertion(argv[i]));
    }

    cout << "incoming 1: " << argv[1] << ", " << insertions[0].getField() << endl;
    cout << "incoming 2: " << argv[2] << ", " << insertions[1].getField() << endl;
    cout << "outgoing 1: " << argv[3] << ", " << insertions[2].getField() << endl;
    cout << "outgoing 2: " << argv[4] << ", " << insertions[3].getField() << endl;

    auto process_ampl = QED.computeAmplitude(Order::TreeLevel,  // OneLoop, TreeLevel
                                        insertions
    );
    Show(process_ampl);
    
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
