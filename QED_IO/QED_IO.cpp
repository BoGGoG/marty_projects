#include <marty.h>
#include <map>
// #include <vector>
#include <typeinfo>
#include <cxxopts.hpp>
#include <fstream>

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


void print_help_func(){
    cout << "help" << endl;

    cout << "--help: print this help" << endl;
    cout << "--particles=in_electron,in_anti_electron,out_photon: insertion arbitrary amount of insertion particles, separated by comma, no space." << endl;
    cout << "--famplitudes: file where the amplitudes should be saved, default: out/ampl.txt" << endl;
    cout << "--fsqamplitudes: file where the squared amplitudes should be saved, default: out/ampl_sq.txt" << endl;
    cout << "--diagrams: If diagrams should be shown, default: false" << endl;
    cout << "--append: If files should be appended or replaced" << endl;
}


int main(int argc, char *argv[])
{
    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    options.add_options()
      ("h,help", "Print help", cxxopts::value<bool>()->default_value("false")) // a bool parameter
      ("a,famplitudes", "File name for amplitudes", cxxopts::value<std::string>()->default_value("out/ampl.txt"))
      ("s,fsqamplitudes", "File name for squared amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_sq.txt"))
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
    auto insertions_file = opts["finsertions"].as<std::string>();

    if (print_help){
        print_help_func();
        return 0;
    };
    cout << "Will export amplitudes to " << amplitudes_file << endl;
    cout << "Will export squared amplitudes to " << sqamplitudes_file << endl;
    if (append_files)
        cout << "Files will be appended if they exist." << endl;
    else
        cout << "Files will be overwritten if they exist." << endl;


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
    std::vector<csl::Expr> squared_ampl_expressions = square_amplitude_indivually(process_ampl, QED);

    for (size_t i = 0; i!=process_ampl.size(); i++){
        auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
        ampl_expressions.push_back(diagram_ampl_eval);
    }

    // std::cout << "AMPLITUDES:" << std::endl;
    // for(size_t i=0; i!=ampl_expressions.size(); i++){
    //     cout << ampl_expressions[i] << endl;
    // }
    // std::cout << "SQUARED AMPLITUDES:" << std::endl;
    // for(size_t i=0; i!=squared_ampl_expressions.size(); i++){
    //     cout << squared_ampl_expressions[i] << endl;
    // }
    //
    if (print_diagrams){
        Show(process_ampl);
    }


    // EXPORT TO FILES
    if (ampl_expressions.size() == 0){
        return 0;
    }
    std::ofstream ampl_file_handle;
    std::ofstream insertions_file_handle;
    std::ofstream sqampl_file_handle;
    if (append_files){
        ampl_file_handle.open(amplitudes_file, std::ios_base::app);
        sqampl_file_handle.open(sqamplitudes_file, std::ios_base::app);
        insertions_file_handle.open(insertions_file, std::ios_base::app);
    }
    else{
        ampl_file_handle.open(amplitudes_file);
        sqampl_file_handle.open(sqamplitudes_file);
        insertions_file_handle.open(insertions_file);
    }

    for(size_t i=0; i!=ampl_expressions.size(); i++){
        ampl_file_handle << ampl_expressions[i] << endl;
        sqampl_file_handle << squared_ampl_expressions[i] << endl;
        insertions_file_handle << particles_strings;
        cout << "saved amplitude to file" << endl;
    }
    ampl_file_handle.close();
    sqampl_file_handle.close();
    insertions_file_handle.close();

    return 0;
}
