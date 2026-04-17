#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_hull <inputs> [options] [-o output.obj]\n\n";
  std::cout << "Convex hull operations:\n";
  std::cout << "  hull <file.obj> ...       - Compute convex hull of meshes\n";
  std::cout << "  points <x> <y> <z> ...  - Compute hull from points\n";
  std::cout << "  sphere <r>                - Hull of a sphere point cloud\n";
  std::cout << "  random <n>              - Hull of n random points\n";
  std::cout << "\nOptions:\n";
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

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string mode = argv[1];
  std::string outputFile = "output.obj";
  std::vector<Manifold> meshes;
  std::vector<vec3> points;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "sphere" && i + 1 < argc) {
      double r = std::stod(argv[++i]);
      int segs = 32;
      for (int j = 0; j < segs; j++) {
        for (int k = 0; k < segs; k++) {
          double theta = M_PI * j / (segs - 1);
          double phi = 2 * M_PI * k / (segs - 1);
          points.push_back(vec3(r * sin(theta) * cos(phi),
                              r * sin(theta) * sin(phi),
                              r * cos(theta)));
        }
      }
    } else if (arg == "random" && i + 1 < argc) {
      int n = std::stoi(argv[++i]);
      for (int j = 0; j < n; j++) {
        points.push_back(vec3((double)rand() / RAND_MAX,
                            (double)rand() / RAND_MAX,
                            (double)rand() / RAND_MAX));
      }
    } else if (arg[0] >= '0' && arg[0] <= '9' && mode == "points") {
      double x = std::stod(argv[i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      points.push_back(vec3(x, y, z));
    } else if (mode == "hull") {
      meshes.push_back(readObj(arg));
    }
  }

  Manifold result;
  if (mode == "hull" && !meshes.empty()) {
    result = Manifold::Hull(meshes);
  } else if (mode == "points" && !points.empty()) {
    result = Manifold::Hull(points);
  } else if ((mode == "sphere" || mode == "random") && !points.empty()) {
    result = Manifold::Hull(points);
  } else {
    std::cerr << "Invalid mode or no inputs\n";
    help();
    return 1;
  }

  if (result.Status() != Manifold::Error::NoError) {
    std::cerr << "Error creating hull: " << (int)result.Status() << "\n";
    return 1;
  }

  std::cout << "Result: " << result.NumTri() << " triangles, "
            << result.NumVert() << " vertices\n";

  std::ofstream out(outputFile);
  if (!result.WriteOBJ(out)) {
    std::cerr << "Failed to write " << outputFile << "\n";
    return 1;
  }
  std::cout << "Wrote to " << outputFile << "\n";

  return 0;
}