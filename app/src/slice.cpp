#include <iostream>
#include <fstream>
#include <string>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_slice <input.obj> [options]\n\n";
  std::cout << "Operations:\n";
  std::cout << "  slice <height>             - Slice mesh at height (returns 2D polygons)\n";
  std::cout << "  project                 - Project to XY plane (returns 2D polygons)\n";
  std::cout << "  split <obj> <obj>        - Split mesh into two parts\n";
  std::cout << "  split-plane <nx> <ny> <nz> <off> - Split by plane\n";
  std::cout << "  trim <nx> <ny> <nz> <off> - Trim by plane\n";
  std::cout << "\nOptions:\n";
  std::cout << "  -o <file>               - Output OBJ file\n";
  std::cout << "  -poly <file>            - Output polygons file\n";
}

Manifold readObj(const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    return Manifold();
  }
  return Manifold::ReadOBJ(in);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string mode = argv[1];
  std::string inputFile;
  std::string outputFile = "output.obj";
  std::string polyFile;

  if (mode == "slice" || mode == "project" || mode == "split" || mode == "split-plane" || mode == "trim") {
    inputFile = argv[2];
  }

  Manifold m = readObj(inputFile);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << "\n";
    return 1;
  }

  if (mode == "slice" && argc >= 3) {
    double height = std::stod(argv[2]);
    Polygons polys = m.Slice(height);
    std::cout << "Slice at z=" << height << ":\n";
    for (size_t i = 0; i < polys.size(); i++) {
      std::cout << "Contour " << i << ": " << polys[i].size() << " vertices\n";
    }
  } else if (mode == "project") {
    Polygons polys = m.Project();
    std::cout << "Projected to 2D:\n";
    for (size_t i = 0; i < polys.size(); i++) {
      std::cout << "Contour " << i << ": " << polys[i].size() << " vertices\n";
    }
  } else if (mode == "split" && argc >= 4) {
    std::string inputFile2 = argv[3];
    Manifold m2 = readObj(inputFile2);
    auto result = m.Split(m2);
    std::cout << "First part: " << result.first.NumTri() << " triangles\n";
    std::cout << "Second part: " << result.second.NumTri() << " triangles\n";
  } else if (mode == "split-plane" && argc >= 7) {
    double nx = std::stod(argv[3]);
    double ny = std::stod(argv[4]);
    double nz = std::stod(argv[5]);
    double off = std::stod(argv[6]);
    auto result = m.SplitByPlane(vec3(nx, ny, nz), off);
    std::cout << "Part 1: " << result.first.NumTri() << " triangles\n";
    std::cout << "Part 2: " << result.second.NumTri() << " triangles\n";
  } else if (mode == "trim-plane" && argc >= 7) {
    double nx = std::stod(argv[3]);
    double ny = std::stod(argv[4]);
    double nz = std::stod(argv[5]);
    double off = std::stod(argv[6]);
    m = m.TrimByPlane(vec3(nx, ny, nz), off);
    std::cout << "Result: " << m.NumTri() << " triangles\n";
  } else {
    help();
    return 1;
  }

  if (!outputFile.empty()) {
    std::ofstream out(outputFile);
    if (m.WriteOBJ(out)) {
      std::cout << "Wrote to " << outputFile << "\n";
    }
  }

  return 0;
}