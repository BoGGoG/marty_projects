#include "marty.h"
#include <map>
#include <fstream>
#include <typeinfo>
#include <cxxopts.hpp>
#include <stdexcept>
// using namespace std;
using namespace csl;
using namespace mty;

using std::cout; using std::cin;
using std::endl; using std::string;
using std::map; using std::copy;


void to_prefix_notation_rec(Expr &expr, std::ofstream &stream) {
    int num_args = expr->size();
    if (num_args == 0){
        stream << expr << ";";
    } else {
        stream << expr->getType() << ";";
        stream << "(" << ";";
        for (size_t i = 0; i!=expr->size(); i++){
            auto arg = expr->getArgument(i);
            to_prefix_notation_rec(arg, stream);
        }
        stream << ")" << ";";
    }
}

void to_prefix_notation(Expr &expr, std::ofstream &stream){
    to_prefix_notation_rec(expr, stream);
    stream << endl;
}


std::vector<csl::Expr> square_amplitude_individually(mty::Amplitude process_ampl, mty::Model& model){
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
    // electron
    if ((name == "in_normal_electron") || (name == "in_electron"))
        return Incoming("e");
    else if (name == "in_anti_electron")
        return Incoming(AntiPart("e"));
    else if ((name == "out_electron") || (name == "out_normal_electron"))
        return Outgoing("e");
    else if (name == "out_anti_electron")
        return Outgoing(AntiPart("e"));

    // muon
    if ((name == "in_normal_muon") || (name == "in_muon"))
        return Incoming("mu");
    else if (name == "in_anti_muon")
        return Incoming(AntiPart("mu"));
    else if ((name == "out_muon") || (name == "out_normal_muon"))
        return Outgoing("mu");
    else if (name == "out_anti_muon")
        return Outgoing(AntiPart("mu"));

    // tau
    if ((name == "in_normal_tau") || (name == "in_tau"))
        return Incoming("t");
    else if (name == "in_anti_tau")
        return Incoming(AntiPart("t"));
    else if ((name == "out_tau") || (name == "out_normal_tau"))
        return Outgoing("t");
    else if (name == "out_anti_tau")
        return Outgoing(AntiPart("t"));

    // up
    if ((name == "in_normal_up") || (name == "in_up"))
        return Incoming("u");
    else if (name == "in_anti_up")
        return Incoming(AntiPart("u"));
    else if ((name == "out_up") || (name == "out_normal_up"))
        return Outgoing("u");
    else if (name == "out_anti_up")
        return Outgoing(AntiPart("u"));

    // down
    if ((name == "in_normal_down") || (name == "in_down"))
        return Incoming("d");
    else if (name == "in_anti_down")
        return Incoming(AntiPart("d"));
    else if ((name == "out_down") || (name == "out_normal_down"))
        return Outgoing("d");
    else if (name == "out_anti_down")
        return Outgoing(AntiPart("d"));

    // strange
    if ((name == "in_normal_strange") || (name == "in_strange"))
        return Incoming("s");
    else if (name == "in_anti_strange")
        return Incoming(AntiPart("s"));
    else if ((name == "out_strange") || (name == "out_normal_strange"))
        return Outgoing("s");
    else if (name == "out_anti_strange")
        return Outgoing(AntiPart("s"));

    // charm
    if ((name == "in_normal_charm") || (name == "in_charm"))
        return Incoming("c");
    else if (name == "in_anti_charm")
        return Incoming(AntiPart("c"));
    else if ((name == "out_charm") || (name == "out_normal_charm"))
        return Outgoing("c");
    else if (name == "out_anti_charm")
        return Outgoing(AntiPart("c"));

    // bottom
    if ((name == "in_normal_bottom") || (name == "in_bottom"))
        return Incoming("b");
    else if (name == "in_anti_bottom")
        return Incoming(AntiPart("b"));
    else if ((name == "out_bottom") || (name == "out_normal_bottom"))
        return Outgoing("b");
    else if (name == "out_anti_bottom")
        return Outgoing(AntiPart("b"));

    // top
    if ((name == "in_normal_top") || (name == "in_top"))
        return Incoming("tt");
    else if (name == "in_anti_top")
        return Incoming(AntiPart("tt"));
    else if ((name == "out_top") || (name == "out_normal_top"))
        return Outgoing("tt");
    else if (name == "out_anti_top")
        return Outgoing(AntiPart("tt"));

    else if ((name == "in_photon") || (name == "in_normal_photon"))
        return Incoming("A");
    else if ((name == "out_photon") || (name == "out_normal_photon"))
        return Outgoing("A");
    else {
        cout << "particle " << name << "not found" << endl;
        throw std::invalid_argument("received unknown particle"+name);
        // return Incoming("e");
    }
}


void print_help_func(){
    cout << "help" << endl;

    cout << "--help: print this help" << endl;
    cout << "--particles=in_electron,in_anti_electron,out_photon: insertion arbitrary amount of insertion particles, separated by comma, no space." << endl;
    cout << "--famplitudes: file where the amplitudes should be saved, default: out/ampl.txt" << endl;
    cout << "--fsqamplitudes: file where the squared amplitudes should be saved, default: out/ampl_sq.txt" << endl;
    cout << "--diagrams: If diagrams should be shown, default: false" << endl;
    cout << "--append: If files should be appended or replaced" << endl;
}



int main(int argc, char *argv[]){
    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    options.add_options()
      ("h,help", "Print help", cxxopts::value<bool>()->default_value("false")) // a bool parameter
      ("a,famplitudes", "File name for amplitudes", cxxopts::value<std::string>()->default_value("out/ampl.txt"))
      ("s,fsqamplitudes", "File name for squared amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_sq.txt"))
      ("r,fsqamplitudes_raw", "File name for raw squared amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_sq_raw.txt"))
      ("i,finsertions", "File name for insertions", cxxopts::value<std::string>()->default_value("out/insertions.txt"))
      ("d,diagrams", "Show diagrams", cxxopts::value<bool>()->default_value("false"))
      ("p,particles", "Insertion particles", cxxopts::value<std::vector<std::string>>())
      ("e,append", "append to files (extend)", cxxopts::value<bool>()->default_value("false"))
      ;

    auto opts = options.parse(argc, argv);
    auto print_help = opts["help"].as<bool>();
    auto print_diagrams = opts["diagrams"].as<bool>();
    auto append_files = opts["append"].as<bool>();
    auto particles_strings = opts["particles"].as<std::vector<std::string>>();
    auto amplitudes_file = opts["famplitudes"].as<std::string>();
    auto sqamplitudes_file = opts["fsqamplitudes"].as<std::string>();
    auto sqamplitudes_raw_file = opts["fsqamplitudes_raw"].as<std::string>();
    auto insertions_file = opts["finsertions"].as<std::string>();

    if (print_help){
        print_help_func();
        return 0;
    };
    cout << "Will export amplitudes to " << amplitudes_file << endl;
    cout << "Will export squared amplitudes to " << sqamplitudes_file << endl;
    cout << "Will export raw squared amplitudes to " << sqamplitudes_raw_file << endl;
    if (append_files)
        cout << "Files will be appended if they exist." << endl;
    else
        cout << "Files will be overwritten if they exist." << endl;


    Model QED;
    Expr psi = constant_s("e");
    QED.addGaugedGroup(group::Type::U1, "em", psi);



    QED.init();

    Particle e = diracfermion_s("e", QED);
    Particle mu = diracfermion_s("mu", QED);
    Particle t = diracfermion_s("t", QED);
    Particle u = diracfermion_s("u", QED);
    Particle d = diracfermion_s("d", QED);
    Particle s = diracfermion_s("s", QED);
    Particle tt = diracfermion_s("tt", QED);
    Particle c = diracfermion_s("c", QED);
    Particle b = diracfermion_s("b", QED);



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


    QED.addParticle(e);
    QED.addParticle(mu);
    QED.addParticle(t);
    QED.addParticle(u);
    QED.addParticle(d);
    QED.addParticle(s);
    QED.addParticle(tt);
    QED.addParticle(c);
    QED.addParticle(b);



    QED.renameParticle("A_em", "A");
    QED.refresh();



    auto rules = ComputeFeynmanRules(QED);

    std::vector<mty::Insertion> insertions;
    for (size_t i = 0; i!= particles_strings.size(); i++){
    insertions.push_back(get_insertion(particles_strings[i]));
    }

    for (size_t i = 0; i!= particles_strings.size(); i++){
    cout << particles_strings[i] << ", " << insertions[i].getField() << endl;
    }

    auto process_ampl = QED.computeAmplitude(Order::TreeLevel,  // OneLoop, TreeLevel
                                    insertions
    );
    std::vector<csl::Expr> ampl_expressions = {};
    std::vector<csl::Expr> squared_ampl_expressions = square_amplitude_individually(process_ampl, QED);

    for (size_t i = 0; i!=process_ampl.size(); i++){
    auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
    ampl_expressions.push_back(diagram_ampl_eval);
    }

    std::cout << "AMPLITUDES:" << std::endl;
    for(size_t i=0; i!=ampl_expressions.size(); i++){
    cout << ampl_expressions[i] << endl;
    }
    std::cout << "SQUARED AMPLITUDES:" << std::endl;
    for(size_t i=0; i!=squared_ampl_expressions.size(); i++){
    cout << squared_ampl_expressions[i] << endl;
    }

    if (print_diagrams){
    Show(process_ampl);
    }


    // EXPORT TO FILES

    if (ampl_expressions.size() == 0){
    // don't create files if no amplitudes for process
    return 0;
    }
    std::ofstream ampl_file_handle;
    std::ofstream insertions_file_handle;
    std::ofstream sqampl_file_handle;
    std::ofstream sqampl_raw_file_handle;
    if (append_files){
    ampl_file_handle.open(amplitudes_file, std::ios_base::app);
    sqampl_file_handle.open(sqamplitudes_file, std::ios_base::app);
    sqampl_raw_file_handle.open(sqamplitudes_raw_file, std::ios_base::app);
    insertions_file_handle.open(insertions_file, std::ios_base::app);
    }
    else{
    ampl_file_handle.open(amplitudes_file);
    sqampl_file_handle.open(sqamplitudes_file);
    sqampl_raw_file_handle.open(sqamplitudes_raw_file);
    insertions_file_handle.open(insertions_file);
    }

    for(size_t i=0; i!=ampl_expressions.size(); i++){
    // ampl_file_handle << ampl_expressions[i] << endl;
    // sqampl_file_handle << squared_ampl_expressions[i] << endl;
    // insertions_file_handle << particles_strings;
    to_prefix_notation(ampl_expressions[i], ampl_file_handle);
    to_prefix_notation(squared_ampl_expressions[i], sqampl_file_handle);
    sqampl_raw_file_handle << squared_ampl_expressions[i] << endl;
    insertions_file_handle << particles_strings << endl;
    }

    ampl_file_handle.close();
    sqampl_file_handle.close();
    sqampl_raw_file_handle.close();
    insertions_file_handle.close();

    return 0;
}
