#include "marty.h"
#include <map>
#include <fstream>
#include <typeinfo>
#include <cxxopts.hpp>
#include <stdexcept>
#include <fstream>
// using namespace std;
using namespace csl;
using namespace mty;
using namespace std;

using std::cout; using std::cin;
using std::endl; using std::string;
using std::map; using std::copy;


vector<string> split(const string& i_str, const string& i_delim)
    // from https://stackoverflow.com/a/57346888 
    // split a string at each appearance of i_delim
    // like python `string.split()`
{
    vector<string> result;
    
    size_t found = i_str.find(i_delim);
    size_t startIndex = 0;

    while(found != string::npos)
    {
        result.push_back(string(i_str.begin()+startIndex, i_str.begin()+found));
        startIndex = found + i_delim.size();
        found = i_str.find(i_delim, startIndex);
    }
    if(startIndex != i_str.size())
        result.push_back(string(i_str.begin()+startIndex, i_str.end()));
    return result;      
}

bool isInternalVertex(csl::Tensor const &X)
{
    const auto &name = X->getName();
    return !name.empty() && name[0] == 'V';
}

struct SimpleField
{
    std::string name;
    std::string vertex;
    bool p;
    bool s;
};

SimpleField convertField(mty::QuantumField const &field)
{
    std::string name = field.getName();
    std::string vertexName = field.getPoint()->getName();
    bool p = field.isParticle();
    bool s = field.isOnShell();
    return {name, vertexName, p, s};
}

// Connection objets represents external fields that connect
// to the same internal vertex
struct Connection {
    std::string vertex;
    std::vector<SimpleField> externalFields; // List of external fields connected
};

// Returns the list of connections corresponding to the diagram's topology
std::vector<Connection> getDiagramConnections(
        std::vector<std::shared_ptr<mty::wick::Node>> const &nodes
        )
{
    std::unordered_map<std::string, Connection> connections;
    for (const auto &node : nodes) {
        auto field = *node->field;
        auto partner = *node->partner.lock()->field;
        bool fieldInternal = isInternalVertex(field.getPoint());
        bool partnerInternal = isInternalVertex(partner.getPoint());
        if (partnerInternal)
        {
            csl::Tensor vertex;
            if (fieldInternal)
                vertex = field.getPoint();
            else
                vertex = partner.getPoint();
            auto pos = connections.find(vertex->getName());
            if (pos == connections.end()) {
                connections[vertex->getName()] = Connection{vertex->getName(), {convertField(field)}};
            }
            else {
                connections[vertex->getName()].externalFields.push_back(convertField(field));
            }
        }
        if (fieldInternal)
        {
            csl::Tensor vertex;
            if (partnerInternal)
                vertex = partner.getPoint();
            else
                vertex = field.getPoint();
            auto pos = connections.find(vertex->getName());
            if (pos == connections.end()) {
                connections[vertex->getName()] = Connection{vertex->getName(), {convertField(partner)}};
            }
            else {
                connections[vertex->getName()].externalFields.push_back(convertField(partner));
            }
        }
    }
    std::vector<Connection> res;
    res.reserve(connections.size());
    for (const auto &el : connections) {
        res.push_back(std::move(el.second));
    }

    return res;
}

// Determines if a list of connections representing a diagram
// is a s-channel. True if external fields on X_1 and X_2 are
// found in the same connection (corresponds to momenta p_1 and p_2)
bool isSChannel(std::vector<Connection> const &topology)
{
    for (const auto &conn : topology) {
        if (conn.externalFields.size() != 2) {
            continue;
        }
        auto nameA = conn.externalFields[0].vertex;
        auto nameB = conn.externalFields[1].vertex;
        if ((nameA == "X_1" && nameB == "X_2")
                || (nameA == "X_2" && nameB == "X_1")) {
            return true;
        }
    }
    return false;
}

// Processes an amplitude, finding the topologies
// and printing the connections. Detects s-channel diagrams
std::vector<std::vector<Connection>> processAmplitudes(Amplitude const &ampl)
{
    // Get diagrams
    std::vector<FeynmanDiagram> const &diagrams = ampl.getDiagrams();
    std::vector<std::vector<Connection>> topologies;
    topologies.reserve(diagrams.size());
    for (auto const &diag : diagrams) {
        //std::cout << "\n************\n";
        //std::cout << "New diagram:\n";
        // Getes the nodes of the diagram
        // (contains edges by looking at the nodes' partners)
        auto nodes = diag.getDiagram()->getNodes();
        auto connections = getDiagramConnections(nodes);
        topologies.push_back(std::move(connections));
    }
    return topologies;
}

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


void export_diagrams_str(mty::Amplitude ampl, std::ofstream &stream) {
    std::vector<std::vector<Connection>> diagramTopologies = processAmplitudes(ampl);
    for (auto const &diagram : diagramTopologies) {
        for (const Connection &conn : diagram) {
            stream << "Vertex " << conn.vertex << ":";
            for (const SimpleField &field : conn.externalFields) {
                stream << (field.p ? "Particle" : "AntiParticle") << (field.s ? " OnShell " : " OffShell ") <<field.name << "(" << field.vertex << "),";
            }
            stream << endl;
        }
        stream << "--------------" << endl;
    }
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
    // ToDo:
    // Write function nicer with `split`!
    auto name_split = split(name, "_");
    if (name_split[0] == "OffShell"){
        auto name_new = name.substr(9);
        // cout << "OffShell: " << name_new << endl;
        auto ret = OffShell(get_insertion(name_new));
        // cout << "isOnShell: " << ret.isOnShell() << endl;
        return ret;
    }
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
        throw std::invalid_argument("received unknown particle "+name);
        // return Incoming("e");
    }
}


void print_help_func(){
    cout << "help" << endl;

    cout << "--help: print this help" << endl;
    cout << "--particles=in_electron,in_anti_electron,out_photon: insertion arbitrary amount of insertion particles, separated by comma, no space." << endl;
    cout << "--famplitudes: file where the amplitudes should be saved, default: out/ampl.txt" << endl;
    cout << "--famplitudes_raw: file where the raw amplitudes should be saved, default: out/ampl_raw.txt" << endl;
    cout << "--fsqamplitudes: file where the squared amplitudes should be saved, default: out/ampl_sq.txt" << endl;
    cout << "--fsqamplitudes_raw: file where the raw squared amplitudes should be saved, default: out/ampl_sq_raw.txt" << endl;
    cout << "--fdiagrams_str: file where the diagrams strings should be saved, default: out/ampl_sq_raw.txt" << endl;
    cout << "--diagrams: If diagrams should be shown, default: false" << endl;
    cout << "--append: If files should be appended or replaced" << endl;
}



int main(int argc, char *argv[]){
    auto export_insertions = false;  // useless anyways ... it's already in the file name

    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    options.add_options()
      ("h,help", "Print help", cxxopts::value<bool>()->default_value("false")) // a bool parameter
      ("a,famplitudes", "File name for amplitudes", cxxopts::value<std::string>()->default_value("out/ampl.txt"))
      ("s,fsqamplitudes", "File name for squared amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_sq.txt"))
      ("r,fsqamplitudes_raw", "File name for raw squared amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_sq_raw.txt"))
      ("t,famplitudes_raw", "File name for raw amplitudes", cxxopts::value<std::string>()->default_value("out/ampl_raw.txt"))
      ("i,finsertions", "File name for insertions. This is a remnant and will not do anything any more!", cxxopts::value<std::string>()->default_value("out/insertions.txt"))
      ("d,diagrams", "Show diagrams", cxxopts::value<bool>()->default_value("false"))
      ("b,fdiagrams_str", "File name for insertions", cxxopts::value<std::string>()->default_value("out/diagrams.txt"))
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
    auto amplitudes_raw_file = opts["famplitudes_raw"].as<std::string>();
    auto diagrams_file = opts["fdiagrams_str"].as<std::string>();

    if (print_help){
        print_help_func();
        return 0;
    };
    cout << "Will export raw amplitudes to " << amplitudes_raw_file << endl;
    cout << "Will export amplitudes to " << amplitudes_file << endl;
    cout << "Will export squared amplitudes to " << sqamplitudes_file << endl;
    cout << "Will export raw squared amplitudes to " << sqamplitudes_raw_file << endl;
    cout << "Will export diagrams to " << diagrams_file << endl;
    if (append_files)
        cout << "Files will be appended if they exist." << endl;
    else
        cout << "Files will be overwritten if they exist." << endl;


    Model QCD_Model;
    QCD_Model.addGaugedGroup(group::Type::SU, "C", 3, csl::constant_s("g"));
    QCD_Model.init();


    Particle u = diracfermion_s("u", QCD_Model);
    Particle d = diracfermion_s("d", QCD_Model);
    Particle s = diracfermion_s("s", QCD_Model);
    Particle t = diracfermion_s("tt", QCD_Model);
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

    QCD_Model.refresh();


    auto rules = ComputeFeynmanRules(QCD_Model);

    std::vector<mty::Insertion> insertions;
    for (size_t i = 0; i!= particles_strings.size(); i++){
    insertions.push_back(get_insertion(particles_strings[i]));
    }

    // for (size_t i = 0; i!= particles_strings.size(); i++){
    // cout << particles_strings[i] << ", " << insertions[i].getField() << endl;
    // }

    auto process_ampl = QCD_Model.computeAmplitude(Order::TreeLevel,  // OneLoop, TreeLevel
                                    insertions
    );
    std::vector<csl::Expr> ampl_expressions = {};
    std::vector<csl::Expr> squared_ampl_expressions = square_amplitude_individually(process_ampl, QCD_Model);

    for (size_t i = 0; i!=process_ampl.size(); i++){
    auto diagram_ampl_eval = Evaluated(process_ampl.expression(i), eval::abbreviation);
    ampl_expressions.push_back(diagram_ampl_eval);
    }

    // std::cout << "AMPLITUDES:" << std::endl;
    // for(size_t i=0; i!=ampl_expressions.size(); i++){
    // cout << ampl_expressions[i] << endl;
    // }
    // std::cout << "SQUARED AMPLITUDES:" << std::endl;
    // for(size_t i=0; i!=squared_ampl_expressions.size(); i++){
    // cout << squared_ampl_expressions[i] << endl;
    // }

    if (print_diagrams){
    Show(process_ampl);
    }


    // EXPORT TO FILES

    if (ampl_expressions.size() == 0){
    // don't create files if no amplitudes for process
    return 0;
    }
    std::ofstream ampl_file_handle;
    std::ofstream sqampl_file_handle;
    std::ofstream sqampl_raw_file_handle;
    std::ofstream ampl_raw_file_handle;
    std::ofstream diagrams_file_handle;
    if (append_files){
    ampl_file_handle.open(amplitudes_file, std::ios_base::app);
    sqampl_file_handle.open(sqamplitudes_file, std::ios_base::app);
    sqampl_raw_file_handle.open(sqamplitudes_raw_file, std::ios_base::app);
    ampl_raw_file_handle.open(amplitudes_raw_file, std::ios_base::app);
    diagrams_file_handle.open(diagrams_file, std::ios_base::app);
    }
    else{
    ampl_file_handle.open(amplitudes_file);
    sqampl_file_handle.open(sqamplitudes_file);
    sqampl_raw_file_handle.open(sqamplitudes_raw_file);
    ampl_raw_file_handle.open(amplitudes_raw_file);
    diagrams_file_handle.open(diagrams_file);
    }

    for(size_t i=0; i!=ampl_expressions.size(); i++){
        to_prefix_notation(ampl_expressions[i], ampl_file_handle);
        to_prefix_notation(squared_ampl_expressions[i], sqampl_file_handle);
        sqampl_raw_file_handle << squared_ampl_expressions[i] << endl;
        ampl_raw_file_handle << ampl_expressions[i] << endl;
    }
    export_diagrams_str(process_ampl, diagrams_file_handle);

    ampl_file_handle.close();
    sqampl_file_handle.close();
    sqampl_raw_file_handle.close();
    ampl_raw_file_handle.close();
    diagrams_file_handle.close();

    return 0;
}
