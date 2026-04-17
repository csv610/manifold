#include <manifold/manifold.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "args.h"

using namespace manifold;

Manifold readObj(const std::string& f) {
  std::ifstream in(f);
  return Manifold::ReadOBJ(in);
}

int main(int argc, char* argv[]) {
  Args args = Args::parse(argc, argv);
  if (args.count() < 1 || args.get("h") == "true") {
    std::cout << "Usage: manifold_boolean <op> <inputs...> [options]\n\n";
    std::cout << "Operations:\n";
    std::cout << "  add         - Union (2 inputs)\n";
    std::cout << "  subtract   - Difference (2 inputs)\n";
    std::cout << "  intersect  - Intersection (2 inputs)\n";
    std::cout << "  min        - Minkowski sum (2 inputs)\n";
    std::cout << "  max        - Minkowski difference (2 inputs)\n";
    std::cout << "  batch      - Batch boolean (multiple inputs)\n";
    std::cout << "  split      - Split by mesh (2 inputs)\n";
    std::cout << "  splitplane - Split by plane (input + nx ny nz d)\n";
    std::cout << "  trimplane  - Trim by plane (input + nx ny nz d)\n";
    std::cout << "  mingap    - Min gap between meshes (2 inputs)\n";
    std::cout << "\nOptions: --o <file> --search <len>\n";
    return 1;
  }

  std::string op = args.next();
  std::string out = args.get("o", "output.obj");
  double searchLen = 1.0;

  if (op == "mingap") {
    if (args.count() < 2) {
      std::cerr << "Needs 2 input files\n";
      return 1;
    }
    std::string f1 = args.next(), f2 = args.next();
    searchLen = std::stod(args.get("search", "1.0"));
    Manifold m1 = readObj(f1), m2 = readObj(f2);
    double gap = m1.MinGap(m2, searchLen);
    std::cout << "Min gap: " << gap << "\n";
    return 0;
  }

  if (op == "splitplane" || op == "trimplane") {
    if (args.count() < 4) {
      std::cerr << "Needs input + nx ny nz d\n";
      return 1;
    }
    std::string f = args.next();
    double nx = std::stod(args.next());
    double ny = std::stod(args.next());
    double nz = std::stod(args.next());
    double d = std::stod(args.next());
    Manifold m = readObj(f);
    vec3 normal(nx, ny, nz);
    if (op == "splitplane") {
      auto result = m.SplitByPlane(normal, d);
      std::cout << "Positive: " << result.first.NumTri() << " tris\n";
      std::cout << "Negative: " << result.second.NumTri() << " tris\n";
      std::ofstream of1(out), of2(out + ".neg.obj");
      WriteOBJ(of1, result.first.GetMeshGL64());
      WriteOBJ(of2, result.second.GetMeshGL64());
    } else {
      m = m.TrimByPlane(normal, d);
      std::cout << "Result: " << m.NumTri() << " tris\n";
      std::ofstream of(out);
      WriteOBJ(of, m.GetMeshGL64());
    }
    std::cout << "Wrote to " << out << "\n";
    return 0;
  }

  if (op == "split") {
    if (args.count() < 2) {
      std::cerr << "Needs 2 input files\n";
      return 1;
    }
    std::string f1 = args.next(), f2 = args.next();
    Manifold m1 = readObj(f1), m2 = readObj(f2);
    auto result = m1.Split(m2);
    std::cout << "Inside: " << result.first.NumTri() << " tris\n";
    std::cout << "Outside: " << result.second.NumTri() << " tris\n";
    std::ofstream of1(out), of2(out + ".outside.obj");
    WriteOBJ(of1, result.first.GetMeshGL64());
    WriteOBJ(of2, result.second.GetMeshGL64());
    std::cout << "Wrote to " << out << "\n";
    return 0;
  }

  if (op == "batch") {
    std::vector<Manifold> meshes;
    while (args.count() > 0) {
      std::string f = args.next();
      meshes.push_back(readObj(f));
    }
    if (meshes.size() < 2) {
      std::cerr << "Needs multiple input files\n";
      return 1;
    }
    Manifold r = Manifold::BatchBoolean(meshes, OpType::Add);
    std::cout << "Result: " << r.NumTri() << " tris, " << r.Volume()
              << " volume\n";
    std::ofstream of(out);
    WriteOBJ(of, r.GetMeshGL64());
    std::cout << "Wrote to " << out << "\n";
    return 0;
  }

  if (args.count() < 2) {
    std::cerr << "Needs 2 input files\n";
    return 1;
  }

  std::string f1 = args.next(), f2 = args.next();
  Manifold m1 = readObj(f1), m2 = readObj(f2);
  Manifold r;
  if (op == "add")
    r = m1.Boolean(m2, OpType::Add);
  else if (op == "subtract")
    r = m1.Boolean(m2, OpType::Subtract);
  else if (op == "intersect")
    r = m1.Boolean(m2, OpType::Intersect);
  else if (op == "min")
    r = m1.MinkowskiSum(m2);
  else if (op == "max")
    r = m1.MinkowskiDifference(m2);
  else {
    std::cerr << "Unknown op: " << op << "\n";
    return 1;
  }

  std::cout << "Result: " << r.NumTri() << " tris, " << r.Volume()
            << " volume\n";
  std::ofstream of(out);
  WriteOBJ(of, r.GetMeshGL64());
  std::cout << "Wrote to " << out << "\n";
  return 0;
}