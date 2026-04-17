#include <iostream>
#include <fstream>
#include <string>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_smooth <input.obj> [options] [-o output.obj]\n\n";
  std::cout << "Options:\n";
  std::cout << "  -refine <n>              - Refine n times (increases triangle count)\n";
  std::cout << "  -length <len>            - Refine to edge length\n";
  std::cout << "  -tolerance <tol>        - Refine to tolerance\n";
  std::cout << "  -normals                 - Calculate smooth normals\n";
  std::cout << "  -smooth                 - Smooth out sharp edges\n";
  std::cout << "  -info                   - Show mesh info\n";
  std::cout << "  -o <file>               - Output OBJ file\n";
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
  std::cout << "Surface area: " << m.SurfaceArea() << "\n";
  std::cout << "Genus: " << m.Genus() << "\n";
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
  bool doSmooth = false;
  int refineTimes = 0;
  double refineLength = 0;
  double refineTol = 0;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-info") {
      showInfo = true;
    } else if (arg == "-normals") {
      doNormals = true;
    } else if (arg == "-smooth") {
      doSmooth = true;
    } else if (arg == "-refine" && i + 1 < argc) {
      refineTimes = std::stoi(argv[++i]);
    } else if (arg == "-length" && i + 1 < argc) {
      refineLength = std::stod(argv[++i]);
    } else if (arg == "-tolerance" && i + 1 < argc) {
      refineTol = std::stod(argv[++i]);
    }
  }

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << ": " << (int)m.Status() << "\n";
    return 1;
  }

  if (refineTimes > 0) {
    m = m.Refine(refineTimes);
  } else if (refineLength > 0) {
    m = m.RefineToLength(refineLength);
  } else if (refineTol > 0) {
    m = m.RefineToTolerance(refineTol);
  }

  if (doNormals) {
    m = m.CalculateNormals(3);
  }

  if (doSmooth) {
    m = m.SmoothOut(60, 0);
  }

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error processing mesh: " << (int)m.Status() << "\n";
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
  std::cout << "Wrote to " << outputFile << ": " << m.NumTri() << " triangles\n";

  return 0;
}