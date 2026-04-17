#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_levelset <sdf_type> [args] [options]\n\n";
  std::cout << "SDF Types:\n";
  std::cout << "  sphere <r>              - Sphere radius (default 1)\n";
  std::cout << "  box <x> <y> <z>        - Box dimensions\n";
  std::cout << "  torus <r1> <r2>        - Torus (major, minor radius)\n";
  std::cout << "  capsule <r> <h>        - Capsule (radius, height)\n";
  std::cout << "  gyroid <scale>           - Gyroid implicit surface\n";
  std::cout << "  schwarz <scale>        - Schwarz-P surface\n";
  std::cout << "\nOptions:\n";
  std::cout << "  -bound <x> <y> <z>    - Bounding box half-size (default 2)\n";
  std::cout << "  -edge <len>            - Grid edge length (default 0.1)\n";
  std::cout << "  -o <file>             - Output OBJ file\n";
}

double sphereSDF(vec3 p, double r) {
  return length(p) - r;
}

double boxSDF(vec3 p, vec3 b) {
  vec3 d = abs(p) - b;
  return length(vec3(std::max(d.x, 0.0), std::max(d.y, 0.0), std::max(d.z, 0.0))) + std::min({d.x, d.y, d.z, 0.0});
}

double torusSDF(vec3 p, double r1, double r2) {
  vec2 q = vec2(length(vec2(p.x, p.z)) - r1, p.y);
  return length(q) - r2;
}

double capsuleSDF(vec3 p, double r, double h) {
  vec3 q = p;
  q.y -= std::clamp(q.y, -h, h);
  return length(q) - r;
}

double gyroidSDF(vec3 p, double scale) {
  p *= scale;
  return std::sin(p.x) * std::cos(p.y) + std::sin(p.y) * std::cos(p.z) + std::sin(p.z) * std::cos(p.x);
}

double schwarzSDF(vec3 p, double scale) {
  p *= scale;
  return std::cos(p.x) + std::cos(p.y) + std::cos(p.z);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string type = argv[1];
  std::string outputFile = "output.obj";
  Box bounds = Box({vec3(-2, -2, -2), vec3(2, 2, 2)});
  double edgeLength = 0.1;
  std::function<double(vec3)> sdf;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-bound" && i + 3 < argc) {
      double x = std::stod(argv[++i]);
      double y = std::stod(argv[++i]);
      double z = std::stod(argv[++i]);
      bounds = Box({vec3(-x, -y, -z), vec3(x, y, z)});
    } else if (arg == "-edge" && i + 1 < argc) {
      edgeLength = std::stod(argv[++i]);
    }
  }

  if (type == "sphere") {
    double r = (argc >= 3) ? std::stod(argv[2]) : 1.0;
    sdf = [r](vec3 p) { return sphereSDF(p, r); };
  } else if (type == "box") {
    double x = 1.0, y = 1.0, z = 1.0;
    if (argc >= 5) { x = std::stod(argv[2]); y = std::stod(argv[3]); z = std::stod(argv[4]); }
    else if (argc >= 3) x = y = z = std::stod(argv[2]);
    vec3 b(x, y, z);
    sdf = [b](vec3 p) { return boxSDF(p, b); };
  } else if (type == "torus") {
    double r1 = 1.0, r2 = 0.3;
    if (argc >= 4) { r1 = std::stod(argv[2]); r2 = std::stod(argv[3]); }
    sdf = [r1, r2](vec3 p) { return torusSDF(p, r1, r2); };
  } else if (type == "capsule") {
    double r = 0.5, h = 1.0;
    if (argc >= 4) { r = std::stod(argv[2]); h = std::stod(argv[3]); }
    sdf = [r, h](vec3 p) { return capsuleSDF(p, r, h); };
  } else if (type == "gyroid") {
    double s = (argc >= 3) ? std::stod(argv[2]) : 1.0;
    sdf = [s](vec3 p) { return gyroidSDF(p, s); };
  } else if (type == "schwarz") {
    double s = (argc >= 3) ? std::stod(argv[2]) : 1.0;
    sdf = [s](vec3 p) { return schwarzSDF(p, s); };
  } else {
    std::cerr << "Unknown SDF type: " << type << "\n";
    help();
    return 1;
  }

  Manifold m = Manifold::LevelSet(sdf, bounds, edgeLength);

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "LevelSet error: " << (int)m.Status() << "\n";
    return 1;
  }

  std::cout << "Created: " << m.NumTri() << " triangles, " << m.NumVert() << " vertices\n";
  std::cout << "Volume: " << m.Volume() << "\n";

  std::ofstream out(outputFile);
  m.WriteOBJ(out);
  std::cout << "Wrote to " << outputFile << "\n";

  return 0;
}