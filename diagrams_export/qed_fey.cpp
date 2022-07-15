#include "marty.h"
#include <fstream>
using namespace std;
using namespace csl;
using namespace mty;

// The problem with this is, that the virtual particles are not labeled correclty
// e.g. vertex Vertex V_0 : AntiParticle OnShell e(X_2) AntiParticle OffShell e(V_0) Particle OffShell A(V_0)
// Here A(V_0) does not make sense, since it's not V_0 -> V_0 (loop), but it's actually A going from V_0 to some other vertex that I don't know. I'm not even sure if this representation is well-defined this way.





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


int main() {

    //  SM_Model model;

    Model QED_Model;

    Expr psi = constant_s("e");
    QED_Model.addGaugedGroup(group::Type::U1, "em", psi);



    QED_Model.init();

    Particle e = diracfermion_s("e", QED_Model);
    Particle mu = diracfermion_s("mu", QED_Model);
    Particle t = diracfermion_s("t", QED_Model);


    auto m_e = constant_s("m_e");
    auto m_mu = constant_s("m_mu");
    auto m_t = constant_s("m_t");


    e->setGroupRep("em", -1);
    mu->setGroupRep("em", -1);
    t->setGroupRep("em", -1);

    e->setMass(m_e);
    mu->setMass(m_mu);
    t->setMass(m_t);



    QED_Model.addParticle(e);
    QED_Model.addParticle(mu);
    QED_Model.addParticle(t);




    QED_Model.renameParticle("A_em", "A");
    //QED_Model.setGaugeChoice("A", gauge::Lorenz);
    QED_Model.refresh();







    auto ampl = QED_Model.computeAmplitude(
            Order::TreeLevel,
            {Incoming(e), Incoming(AntiPart(e)), Incoming("A"),
            Outgoing(e), Outgoing(AntiPart(e)), Outgoing("A")});


    auto diagrams = ampl.obtainGraphs();
    auto diagrams_built = Drawer::buildDiagrams(diagrams);
    cout << diagrams_built[0] << endl;

    if (!ampl.empty()) {

        std::vector<std::vector<Connection>> diagramTopologies = processAmplitudes(ampl);
        for (auto const &diagram : diagramTopologies) {
            cout << "\n";
            for (const Connection &conn : diagram) {
                cout << "Vertex " << conn.vertex << " : ";
                for (const SimpleField &field : conn.externalFields) {
                    cout << (field.p ? "Particle" : "AntiParticle") << (field.s ? " OnShell " : " OffShell ") <<field.name << "(" << field.vertex << ") ";
                }
                cout << " , " << endl;
            }
            cout << endl;
            cout << "--------------" << endl;
            cout << endl;
        }
        Show(ampl);
        //
        //
        // for (size_t n = 0; n != ampl.size(); n++) {
        //
        //       Expr  squaredAmpl = QED_Model.computeSquaredAmplitude(
        //         Amplitude(ampl.getOptions(), {ampl.getDiagrams()[n]}, ampl.getKinematics()));
        //       Evaluate(squaredAmpl, csl::eval::abbreviation);
        //       cout <<squaredAmpl << '\n';
        //     }
    }
}
