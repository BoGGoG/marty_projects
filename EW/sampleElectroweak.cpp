#include <marty.h>
#include <sstream>

using namespace std;
using namespace csl;
using namespace mty;

// To be changed by the user if needed !
std::string path_to_generated_library = ".";

void to_prefix_notation_rec(Expr &expr, stringstream &stream) {
    int num_args = expr->size();
  if (num_args == 0){
    stream << expr << ",";
  } else {
    stream << expr->getType() << ",";
    stream << "(" << ",";
    for (size_t i = 0; i!=expr->size(); i++){
      auto arg = expr->getArgument(i);
     to_prefix_notation_rec(arg, stream);
    }
    stream << ")" << ",";
  }
}

void to_prefix_notation(Expr &expr, stringstream &stream){
    stream << "[";
    to_prefix_notation_rec(expr, stream);
    stream << "]";
    stream << endl;
}


int main() {

    // Model building
    mty::option::searchAbreviations = true;

    Model toyModel;
    toyModel.addGaugedGroup(group::Type::SU, "L", 2);
    toyModel.addGaugedGroup(group::Type::U1, "Y");
    toyModel.init();

    toyModel.renameParticle("A_L", "W");
    toyModel.renameParticle("A_Y", "B");

    Particle Q_L = weylfermion_s("Q_L", toyModel, Chirality::Left);
    Particle u_R = weylfermion_s("u_R", toyModel, Chirality::Right);
    Particle d_R = weylfermion_s("d_R", toyModel, Chirality::Right);

    Q_L->setGroupRep("L", 1); 
    Q_L->setGroupRep("Y", {1, 6});
    u_R->setGroupRep("Y", {2, 3});
    d_R->setGroupRep("Y", {-1, 3});

    toyModel.addParticle(Q_L);
    toyModel.addParticle(u_R);
    toyModel.addParticle(d_R);

    toyModel.breakGaugeSymmetry("Y");
    toyModel.breakGaugeSymmetry("L");

    toyModel.renameParticle("Q_L_1", "u_L");
    toyModel.renameParticle("Q_L_2", "d_L");

    // cout << toyModel << endl;


    // Higgs mechanism simulated
    Particle W_1 = toyModel.getParticle("W_1");
    Particle W_2 = toyModel.getParticle("W_2");
    Particle F_W_1 = W_1->getFieldStrength();
    Particle F_W_2 = W_2->getFieldStrength();
    Particle W = W_1->generateSimilar("W");
    W->setSelfConjugate(false);
    Index mu = MinkowskiIndex();
    Index nu = MinkowskiIndex();
    // W_1 goes to (W+ + W-) / sqrt(2)
    toyModel.replace(W_1, (W(mu) + GetComplexConjugate(W(mu))) / sqrt_s(2));
    toyModel.replace(F_W_1, (W({mu, nu}) + GetComplexConjugate(W({mu, nu}))) / sqrt_s(2));
    // W_2 goes to i*(W+ - W-) / sqrt(2)
    toyModel.replace(W_2, CSL_I * (W({mu}) - GetComplexConjugate(W({mu}))) / sqrt_s(2));
    toyModel.replace(F_W_2, CSL_I * (W({mu, nu}) - GetComplexConjugate(W({mu, nu}))) / sqrt_s(2));

    Expr thetaW = constant_s("thetaW");
    Expr cW = cos_s(thetaW);
    Expr sW = sin_s(thetaW);
    // We give the rotation matrix explicitly here, between double curly braces
    toyModel.rotateFields(
            {"W_3", "B"},
            {"Z"  , "A"},
            {{cW , sW},
             {-sW, cW}}
            );
    Expr e = constant_s("e");
    Expr gY = toyModel.getScalarCoupling("g_Y");
    Expr gL = toyModel.getScalarCoupling("g_L");
    toyModel.replace(gY, e / cW);
    toyModel.replace(gL, e / sW);

    Expr M_W = constant_s("M_W");
    Expr m_u = constant_s("m_u");
    Expr m_d = constant_s("m_d");

    toyModel.addBosonicMass("W", M_W);
    toyModel.addFermionicMass("u_L", "u_R", m_u);
    toyModel.addFermionicMass("d_L", "d_R", m_d);

    toyModel.refresh();
    // cout << toyModel << endl;

    auto rules = toyModel.getFeynmanRules();
    // Display(rules); // Displays expressions in terminal
    // Show(rules); // Shows diagrams in the application
    //
    //
    auto res = toyModel.computeAmplitude(
       Order::TreeLevel,
       {Incoming("u"), Incoming(AntiPart("u")),
       Outgoing("d"), Outgoing(AntiPart("d"))}
       );
    //
    // Display(res);
    // DisplayAbbreviations();
    // Show(res);
    //


    Expr squared_ampl = toyModel.computeSquaredAmplitude(res);
    cout << "total squared amplitude:" <<endl;
    cout << squared_ampl << endl;
    //
    // mty::Library myLib("toy", path_to_generated_library);
    // myLib.addFunction("squared_ampl", squared_ampl);
    // myLib.build();
    //
    
    cout << "=== Terms:" << endl;
    for (size_t i = 0; i!= res.size(); i++) {
        Expr &term = res.expression(i);
        cout << "i = " << i << endl;
        // cout << term << endl;
        cout << Evaluated(term, eval::abbreviation) << endl;
    }

    // cout << "------------" << endl;
    // cout << "Going through expression i=0:" << endl;
    // auto expr0 = res.expression(0);
    // cout << "expr0 : " << expr0 << endl;
    // cout << "expr0 type: " <<  expr0->getType() << endl;
    // cout << "expr0 arg 0: " << expr0->getArgument() << endl;
    // cout << "expr0 arg 1: " << expr0->getArgument(1) << endl;
    // cout << "expr0 arg 2: " << expr0->getArgument(2) << endl;

    // cout << "expr0 arg 2 arg 0" << expr0->getArgument(2)->getArgument(0) << endl;

    cout << "=== Abbreviations:" << endl;
    DisplayAbbreviations();
    

    cout << "=== prefix notation:" << endl;

    for (size_t i = 0; i!= res.size(); i++){
        auto expr_eval = Evaluated(res.expression(i), eval::abbreviation);
        // cout << exp0_eval  << endl;
        stringstream my_stream(ios::in|ios::out);
        to_prefix_notation(expr_eval, my_stream);
        cout << my_stream.str() << endl;
    }
    
    return 0;
}
