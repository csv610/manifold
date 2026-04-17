#include <manifold/manifold.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

using namespace manifold;

void help() {
  std::cout
      << "Usage: manifold_properties <input.obj> [options] [-o output.obj]\n\n";
  std::cout << "Options:\n";
  std::cout << "  -normal              - Calculate smooth normals\n";
  std::cout << "  -curvature         - Calculate curvature (gaussian & mean)\n";
  std::cout << "  -color             - Add vertex colors based on normals\n";
  std::cout << "  -info              - Show properties info\n";
  std::cout << "  -o <file>         - Output OBJ file\n";
}

Manifold readObj(const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    std::cerr << "Cannot open " << filename << "\n";
    return Manifold();
  }
  return Manifold::ReadOBJ(in);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string inputFile = argv[1];
  std::string outputFile = "output.obj";
  bool showInfo = false;
  bool doNormals = false;
  bool doCurvature = false;
  bool doColor = false;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-info") {
      showInfo = true;
    } else if (arg == "-normal") {
      doNormals = true;
    } else if (arg == "-curvature") {
      doCurvature = true;
    } else if (arg == "-color") {
      doColor = true;
    }
  }

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << ": " << (int)m.Status()
              << "\n";
    return 1;
  }

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error: " << (int)m.Status() << "\n";
    return 1;
  }

  if (doCurvature) {
    m = m.CalculateCurvature(3, 4);
  }

  if (doNormals || doColor) {
    m = m.CalculateNormals(3);
  }

  if (doColor && m.NumProp() < 6) {
    m = m.CalculateNormals(3);
    m = m.SetProperties(6, [](double* out, vec3 pos, const double* in) {
      out[0] = 0.5 + 0.5 * in[0];
      out[1] = 0.5 + 0.5 * in[1];
      out[2] = 0.5 + 0.5 * in[2];
      out[3] = in[3];
      out[4] = in[4];
      out[5] = in[5];
    });
  }

  if (showInfo) {
    std::cout << "Properties: " << m.NumProp() << "\n";
    std::cout << "Property verts: " << m.NumPropVert() << "\n";
  }

  std::ofstream out(outputFile);
  if (!m.WriteOBJ(out)) {
    std::cerr << "Failed to write " << outputFile << "\n";
    return 1;
  }
  std::cout << "Wrote to " << outputFile << ": " << m.NumTri()
            << " triangles\n";

  return 0;
}