#include <marty.h>
#include <sstream>

using namespace std;
using namespace csl;
// using namespace mty;

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

  Expr aa = constant_s("a");
  Expr f = sin_s(aa);
  Expr g = 2 + cos_s(3*aa*f + aa);
  cout << "g = " << g << endl;

  stringstream my_stream(ios::in|ios::out);
  to_prefix_notation(g, my_stream);

  cout << my_stream.str() << endl;
  return 0;
}
