#include <iostream>
#include <fstream>
#include <manifold/manifold.h>
#include "args.h"

using namespace manifold;

Manifold readObj(const std::string& f) { 
  std::ifstream in(f); 
  return Manifold::ReadOBJ(in); 
}

int main(int argc, char* argv[]) {
  Args args = Args::parse(argc, argv);
  if (args.count() < 2 || args.get("h") == "true") {
    std::cout << "Usage: manifold_boolean <op> <input1.obj> <input2.obj> [options]\n\n";
    std::cout << "Operations:\n";
    std::cout << "  add         - Union\n";
    std::cout << "  subtract   - Difference\n";  
    std::cout << "  intersect  - Intersection\n";
    std::cout << "  min       - Minkowski sum\n";
    std::cout << "  max       - Minkowski difference\n";
    std::cout << "\nOptions: --o <file>\n";
    return 1;
  }

  std::string op = args.next();
  std::string f1 = args.next(), f2 = args.next();
  std::string out = args.get("o", "output.obj");

  Manifold m1 = readObj(f1), m2 = readObj(f2);
  Manifold r;
  if (op == "add") r = m1.Boolean(m2, OpType::Add);
  else if (op == "subtract") r = m1.Boolean(m2, OpType::Subtract);
  else if (op == "intersect") r = m1.Boolean(m2, OpType::Intersect);
  else if (op == "min") r = m1.MinkowskiSum(m2);
  else if (op == "max") r = m1.MinkowskiDifference(m2);
  else { std::cerr << "Unknown op: " << op << "\n"; return 1; }

  std::cout << "Result: " << r.NumTri() << " tris, " << r.Volume() << " volume\n";
  std::ofstream of(out);
  WriteOBJ(of, r.GetMeshGL64());
  std::cout << "Wrote to " << out << "\n";
  return 0;
}