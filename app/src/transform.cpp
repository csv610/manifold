#include <manifold/manifold.h>

#include <fstream>
#include <iostream>
#include <string>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_transform <input.obj> [transforms...] [-o "
               "output.obj]\n\n";
  std::cout << "Transforms:\n";
  std::cout << "  translate <x> <y> <z>     - Translate mesh\n";
  std::cout << "  scale <x> <y> <z>          - Scale mesh\n";
  std::cout
      << "  rotate <x> <y> <z>         - Rotate in degrees (x, y, z axes)\n";
  std::cout << "  mirror <x> <y> <z>         - Mirror across plane\n";
  std::cout << "  tolerance <val>            - Set tolerance\n";
  std::cout << "\nOptions:\n";
  std::cout << "  -o <file>                  - Output OBJ file\n";
  std::cout << "  -info                     - Show mesh info\n";
}

Manifold readObj(const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    std::cerr << "Cannot open " << filename << "\n";
    return Manifold();
  }
  return Manifold::ReadOBJ(in);
}

void printInfo(const Manifold& m) {
  std::cout << "Vertices: " << m.NumVert() << "\n";
  std::cout << "Triangles: " << m.NumTri() << "\n";
  std::cout << "Edges: " << m.NumEdge() << "\n";
  std::cout << "Volume: " << m.Volume() << "\n";
  std::cout << "Surface area: " << m.SurfaceArea() << "\n";
  std::cout << "Genus: " << m.Genus() << "\n";
  Box bb = m.BoundingBox();
  std::cout << "Bounding box: min(" << bb.min.x << "," << bb.min.y << ","
            << bb.min.z << ") max(" << bb.max.x << "," << bb.max.y << ","
            << bb.max.z << ")\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string inputFile = argv[1];
  std::string outputFile = "output.obj";
  bool showInfo = false;

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << ": " << (int)m.Status()
              << "\n";
    return 1;
  }

  int i = 2;
  while (i < argc) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-info") {
      showInfo = true;
    } else if (arg == "translate" && i + 3 < argc) {
      double x = std::stod(argv[++i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      m = m.Translate(vec3(x, y, z));
    } else if (arg == "scale" && i + 3 < argc) {
      double x = std::stod(argv[++i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      m = m.Scale(vec3(x, y, z));
    } else if (arg == "rotate" && i + 3 < argc) {
      double x = std::stod(argv[++i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      m = m.Rotate(x, y, z);
    } else if (arg == "mirror" && i + 3 < argc) {
      double x = std::stod(argv[++i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      m = m.Mirror(vec3(x, y, z));
    } else if (arg == "tolerance" && i + 1 < argc) {
      double tol = std::stod(argv[++i]);
      m = m.SetTolerance(tol);
      std::cout << "Set tolerance to " << tol << "\n";
    } else {
      std::cerr << "Unknown option: " << arg << "\n";
      help();
      return 1;
    }
    i++;
  }

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Transform error: " << (int)m.Status() << "\n";
    return 1;
  }

  if (showInfo) {
    printInfo(m);
  }

  std::ofstream out(outputFile);
  if (!m.WriteOBJ(out)) {
    std::cerr << "Failed to write " << outputFile << "\n";
    return 1;
  }
  std::cout << "Wrote to " << outputFile << "\n";

  return 0;
}