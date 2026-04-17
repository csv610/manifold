#include <iostream>
#include <fstream>
#include <string>
#include <manifold/manifold.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_warp <input.obj> <warp_type> [args] [-o output.obj]\n\n";
  std::cout << "Warp Types:\n";
  std::cout << "  twist <degrees>        - Twist around Y axis\n";
  std::cout << "  bend <degrees>        - Bend around X axis\n";
  std::cout << "  taper <scale>         - Taper in Y direction\n";
  std::cout << "  bulge <scale>       - Bulge outward\n";
  std::cout << "  shell <thickness>    - Create shell (offset surface)\n";
  std::cout << "  -info                 - Show mesh info\n";
  std::cout << "  -o <file>           - Output OBJ file\n";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    help();
    return 1;
  }

  std::string inputFile = argv[1];
  std::string warpType = argv[2];
  std::string outputFile = "output.obj";
  bool showInfo = false;

  std::ifstream in(inputFile);
  Manifold m = Manifold::ReadOBJ(in);
  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error reading " << inputFile << "\n";
    return 1;
  }

  for (int i = 3; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-info") {
      showInfo = true;
    } else if (warpType == "twist" && i + 1 < argc) {
      double degrees = std::stod(argv[++i]);
      double rad = degrees * M_PI / 180.0;
      m = m.Warp([rad](vec3& p) {
        double c = std::cos(p.y * rad);
        double s = std::sin(p.y * rad);
        double x = p.x * c - p.z * s;
        double z = p.x * s + p.z * c;
        p.x = x;
        p.z = z;
      });
    } else if (warpType == "bend" && i + 1 < argc) {
      double degrees = std::stod(argv[++i]);
      double rad = degrees * M_PI / 180.0;
      m = m.Warp([rad](vec3& p) {
        double c = std::cos(p.x * rad);
        double s = std::sin(p.x * rad);
        double newY = s * p.y;
        double newZ = c * p.y - 1;
        p.y = newY;
        p.z = newZ;
      });
    } else if (warpType == "taper" && i + 1 < argc) {
      double scale = std::stod(argv[++i]);
      m = m.Warp([scale](vec3& p) {
        p.x *= 1 + p.y * (scale - 1);
        p.z *= 1 + p.y * (scale - 1);
      });
    } else if (warpType == "bulge" && i + 1 < argc) {
      double scale = std::stod(argv[++i]);
      m = m.Warp([scale](vec3& p) {
        double len = length(p);
        if (len > 0.001) {
          double newLen = len * (1 + scale * len * len);
          p = p * (newLen / len);
        }
      });
    } else if (warpType == "shell" && i + 1 < argc) {
      double thickness = std::stod(argv[++i]);
      m = m.Translate(vec3(0, thickness, 0)) + m.Translate(vec3(0, -thickness, 0));
    }
  }

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Warp error: " << (int)m.Status() << "\n";
    return 1;
  }

  if (showInfo) {
    std::cout << "Vertices: " << m.NumVert() << "\n";
    std::cout << "Triangles: " << m.NumTri() << "\n";
    std::cout << "Volume: " << m.Volume() << "\n";
  }

  std::ofstream out(outputFile);
  m.WriteOBJ(out);
  std::cout << "Wrote to " << outputFile << "\n";

  return 0;
}