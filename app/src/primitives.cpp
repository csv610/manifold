#include <iostream>
#include <fstream>
#include <manifold/manifold.h>
#include "args.h"

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_primitives <type> [options]\n\n";
  std::cout << "Types:\n";
  std::cout << "  cube [size]           - Create a cube\n";
  std::cout << "  sphere [radius]      - Create a sphere\n";
  std::cout << "  cylinder [height] [radius] - Create a cylinder\n";
  std::cout << "  torus [r1] [r2]    - Create a torus\n";
  std::cout << "  tetrahedron        - Create a tetrahedron\n";
  std::cout << "\nOptions:\n";
  std::cout << "  --o <file>         - Output OBJ file\n";
  std::cout << "  --center          - Center the primitive\n";
}

int main(int argc, char* argv[]) {
  Args args = Args::parse(argc, argv);
  if (args.count() == 0 || args.get("h") == "true") { help(); return 1; }

  std::string type = args.next();
  std::string output = args.get("o", "output.obj");

  Manifold m;
  double size = args.getf("size", 1.0);
  bool center = args.get("center") == "true";

  if (type == "cube") m = Manifold::Cube(vec3(size), center);
  else if (type == "sphere") m = Manifold::Sphere(args.getf("radius", 1.0), 0);
  else if (type == "cylinder") {
    double r = args.getf("radius", 1.0);
    m = Manifold::Cylinder(args.getf("height", 1.0), r, r, 0, center);
  } else if (type == "torus") {
    double r1 = args.getf("r1", 0.5), r2 = args.getf("r2", 1.0);
    Polygons circle;
    circle.push_back({});
    for (int i = 0; i <= 32; i++) {
      double th = 2 * M_PI * i / 32;
      circle[0].push_back({vec2(r2 * cos(th), r2 * sin(th))});
    }
    m = Manifold::Revolve(circle, 0, 360.0);
  } else if (type == "tetrahedron") m = Manifold::Tetrahedron();
  else { std::cerr << "Unknown type: " << type << "\n"; help(); return 1; }

  if (m.Status() != Manifold::Error::NoError) return 1;

  std::cout << "Created " << type << ": " << m.NumTri() << " tris, " << m.NumVert() << " verts\n";
  std::ofstream out(output);
  WriteOBJ(out, m.GetMeshGL64());
  std::cout << "Wrote to " << output << "\n";
  return 0;
}