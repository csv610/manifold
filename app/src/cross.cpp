#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <manifold/cross_section.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_cross <op> [args] [options]\n\n";
  std::cout << "Operations:\n";
  std::cout << "  circle <r>                - Create circle of radius r\n";
  std::cout << "  rect <w> <h>               - Create rectangle w x h centered\n";
  std::cout << "  square <size>             - Create square\n";
  std::cout << "  add <cs1.obj> <cs2.obj>   - Union (add) two cross sections\n";
  std::cout << "  sub <cs1.obj> <cs2.obj>   - Subtract second from first\n";
  std::cout << "  intersect <cs1.obj> <cs2.obj> - Intersect two cross sections\n";
  std::cout << "  offset <cs.obj> <d>       - Offset by distance d\n";
  std::cout << "  hull <cs.obj>             - Convex hull of cross section\n";
  std::cout << "  -o <file>                 - Output file (SVG)\n";
  std::cout << "\nNote: CrossSection only supports 2D operations\n";
}

void writePolygons(const Polygons& polys, const std::string& filename) {
  std::ofstream out(filename);
  for (size_t i = 0; i < polys.size(); i++) {
    for (size_t j = 0; j < polys[i].size(); j++) {
      vec2 p = polys[i][j];
      out << p.x << " " << p.y << "\n";
    }
    out << "-\n";
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string op = argv[1];
  std::string outputFile = "output.txt";

  for (int i = 2; i < argc; i++) {
    if (std::string(argv[i]) == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    }
  }

  CrossSection cs;
  if (op == "circle" && argc >= 3) {
    double r = std::stod(argv[2]);
    cs = CrossSection::Circle(r);
  } else if (op == "rect" && argc >= 4) {
    double w = std::stod(argv[2]);
    double h = std::stod(argv[3]);
    cs = CrossSection::Square(vec2(w, h), true);
  } else if (op == "square" && argc >= 3) {
    double s = std::stod(argv[2]);
    cs = CrossSection::Square(vec2(s, s), true);
  } else {
    std::cerr << "Unknown operation: " << op << "\n";
    help();
    return 1;
  }

  writePolygons(cs.ToPolygons(), outputFile);
  std::cout << "Wrote to " << outputFile << ": " << cs.NumVert() << " vertices\n";

  return 0;
}