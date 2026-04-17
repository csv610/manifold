#include <manifold/manifold.h>

#include <fstream>
#include <iostream>
#include <string>

using namespace manifold;

void help() {
  std::cout
      << "Usage: manifold_meshid <input.obj> [options] [-o output.obj]\n\n";
  std::cout << "Options:\n";
  std::cout << "  -id                    - Show OriginalID\n";
  std::cout << "  -reserve <n>          - Reserve IDs for n meshes\n";
  std::cout << "  -original             - Mark as original\n";
  std::cout << "  -info                 - Show mesh info\n";
  std::cout << "  -o <file>             - Output OBJ file\n";
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
  bool showId = false;
  bool doOriginal = false;
  bool showInfo = false;
  int reserveCount = 0;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-id") {
      showId = true;
    } else if (arg == "-original") {
      doOriginal = true;
    } else if (arg == "-info") {
      showInfo = true;
    } else if (arg == "-reserve" && i + 1 < argc) {
      reserveCount = std::stoi(argv[++i]);
    }
  }

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << ": " << (int)m.Status()
              << "\n";
    return 1;
  }

  if (reserveCount > 0) {
    Manifold::ReserveIDs(reserveCount);
    std::cout << "Reserved " << reserveCount << " IDs\n";
  }

  if (showId) {
    std::cout << "OriginalID: " << m.OriginalID() << "\n";
  }

  if (doOriginal) {
    m = m.AsOriginal();
  }

  if (showInfo) {
    std::cout << "Vertices: " << m.NumVert() << "\n";
    std::cout << "Triangles: " << m.NumTri() << "\n";
    std::cout << "OriginalID: " << m.OriginalID() << "\n";
    std::cout << "Tolerance: " << m.GetTolerance() << "\n";
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