#include <iostream>
#include <fstream>
#include <string>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_info <input.obj> [options]\n\n";
  std::cout << "Options:\n";
  std::cout << "  -all                    - Show all info\n";
  std::cout << "  -decompose             - Decompose into components\n";
  std::cout << "  -simplify <tol>       - Simplify mesh\n";
  std::cout << "  -raycast <x> <y> <z> <x2> <y2> <z2> - Ray cast\n";
  std::cout << "  -o <file>            - Output file (for decompose/simplify)\n";
}

Manifold readObj(const std::string& filename) {
  std::ifstream in(filename);
  return Manifold::ReadOBJ(in);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string inputFile = argv[1];
  std::string outputFile = "output.obj";
  bool doAll = false, doDecompose = false, doSimplify = false, doRaycast = false;
  double simplifyTol = 0;
  vec3 rayOrigin, rayEndpoint;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) outputFile = argv[++i];
    else if (arg == "-all") doAll = true;
    else if (arg == "-decompose") doDecompose = true;
    else if (arg == "-simplify" && i + 1 < argc) { doSimplify = true; simplifyTol = std::stod(argv[++i]); }
    else if (arg == "-raycast" && i + 6 < argc) {
      doRaycast = true;
      rayOrigin = vec3(std::stod(argv[++i]), std::stod(argv[++i]), std::stod(argv[++i]));
      rayEndpoint = vec3(std::stod(argv[++i]), std::stod(argv[++i]), std::stod(argv[++i]));
    }
  }

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << "\n";
    return 1;
  }

  std::cout << "=== Mesh Info ===\n";
  std::cout << "Vertices: " << m.NumVert() << "\n";
  std::cout << "Triangles: " << m.NumTri() << "\n";
  std::cout << "Edges: " << m.NumEdge() << "\n";
  std::cout << "Volume: " << m.Volume() << "\n";
  std::cout << "Surface Area: " << m.SurfaceArea() << "\n";
  std::cout << "Genus: " << m.Genus() << "\n";
  std::cout << "Is Empty: " << (m.IsEmpty() ? "yes" : "no") << "\n";
  Box bb = m.BoundingBox();
  std::cout << "BBox: (" << bb.min.x << "," << bb.min.y << "," << bb.min.z << ") - ("
            << bb.max.x << "," << bb.max.y << "," << bb.max.z << ")\n";

  if (doDecompose) {
    auto comps = m.Decompose();
    if (comps.size() > 1) {
      std::cout << "\nDecomposed into " << comps.size() << " components:\n";
      for (size_t i = 0; i < comps.size(); i++) {
        std::string out = outputFile;
        if (comps.size() > 1) out = outputFile + "_" + std::to_string(i) + ".obj";
        std::ofstream of(out);
        comps[i].WriteOBJ(of);
        std::cout << "  Component " << i << ": " << comps[i].NumTri() << " tris -> " << out << "\n";
      }
    }
  }

  if (doSimplify) {
    std::cout << "\nSimplifying to tolerance " << simplifyTol << "...\n";
    m = m.Simplify(simplifyTol);
    std::cout << "Result: " << m.NumTri() << " triangles (was " << m.NumTri() << ")\n";
    std::ofstream of(outputFile);
    m.WriteOBJ(of);
    std::cout << "Wrote to " << outputFile << "\n";
  }

  if (doRaycast) {
    auto hits = m.RayCast(rayOrigin, rayEndpoint);
    std::cout << "\nRay cast: " << hits.size() << " hits\n";
    for (size_t i = 0; i < hits.size() && i < 5; i++) {
      std::cout << "  Hit " << i << " at distance " << hits[i].distance << "\n";
    }
  }

  return 0;
}