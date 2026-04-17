#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <manifold/manifold.h>
#include <manifold/polygon.h>

using namespace manifold;

void help() {
  std::cout << "Usage: manifold_extrude <poly> [options] [-o output.obj]\n\n";
  std::cout << "Polygons (XY profile to extrude/revolve):\n";
  std::cout << "  square                    - Default square (unit size at origin)\n";
  std::cout << "  circle <radius>           - Circle with given radius\n";
  std::cout << "  ring <r1> <r2>          - Ring between two radii\n";
  std::cout << "  star <points> <r1> <r2>  - Star shape\n";
  std::cout << "\nOptions:\n";
  std::cout << "  -height <h>             - Extrusion height (default 1.0)\n";
  std::cout << "  -revolve                 - instead of extruding, revolve 360 degrees\n";
  std::cout << "  -twist <degrees>        - Twist the top (extrude only)\n";
  std::cout << "  -scale <x> <y>         - Scale top (extrude only, default 1.0 1.0)\n";
  std::cout << "  -divisions <n>         - Number of divisions (default 0)\n";
  std::cout << "  -o <file>              - Output OBJ file\n";
}

Polygons createSquare() {
  Polygons polys;
  polys.push_back({{vec2(-0.5, -0.5)}, {vec2(0.5, -0.5)}, {vec2(0.5, 0.5)}, {vec2(-0.5, 0.5)}});
  return polys;
}

Polygons createCircle(double radius, int segments = 64) {
  Polygons polys;
  polys.push_back({});
  auto& poly = polys[0];
  for (int i = 0; i <= segments; i++) {
    double theta = 2 * M_PI * i / segments;
    poly.push_back({vec2(radius * cos(theta), radius * sin(theta))});
  }
  return polys;
}

Polygons createRing(double r1, double r2, int segments = 64) {
  Polygons polys;
  polys.push_back({});
  auto& outer = polys[0];
  for (int i = 0; i <= segments; i++) {
    double theta = 2 * M_PI * i / segments;
    outer.push_back({vec2(r2 * cos(theta), r2 * sin(theta))});
  }
  Polygons inner;
  inner.push_back({});
  auto& innerPoly = inner[0];
  for (int i = segments; i >= 0; i--) {
    double theta = 2 * M_PI * i / segments;
    innerPoly.push_back({vec2(r1 * cos(theta), r1 * sin(theta))});
  }
  polys.push_back(innerPoly);
  return polys;
}

Polygons createStar(int points, double innerR, double outerR, int segments = 64) {
  Polygons polys;
  polys.push_back({});
  auto& poly = polys[0];
  for (int i = 0; i < points * segments; i++) {
    double theta = 2 * M_PI * i / (points * segments);
    double r = (i % 2 == 0) ? outerR : innerR;
    poly.push_back({vec2(r * cos(theta), r * sin(theta))});
  }
  return polys;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    help();
    return 1;
  }

  std::string polyType = argv[1];
  std::string outputFile = "output.obj";
  double height = 1.0;
  double twist = 0.0;
  double scaleX = 1.0, scaleY = 1.0;
  int divisions = 0;
  bool revolve = false;
  double radius = 1.0;
  double r1 = 0.5, r2 = 1.0;
  int points = 5;

  for (int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg == "-height" && i + 1 < argc) {
      height = std::stod(argv[++i]);
    } else if (arg == "-twist" && i + 1 < argc) {
      twist = std::stod(argv[++i]);
    } else if (arg == "-scale" && i + 2 < argc) {
      scaleX = std::stod(argv[++i]);
      scaleY = std::stod(argv[++i]);
    } else if (arg == "-divisions" && i + 1 < argc) {
      divisions = std::stoi(argv[++i]);
    } else if (arg == "-revolve") {
      revolve = true;
    } else if (arg == "circle" && i + 1 < argc) {
      radius = std::stod(argv[++i]);
    } else if (arg == "ring" && i + 2 < argc) {
      r1 = std::stod(argv[++i]);
      r2 = std::stod(argv[++i]);
    } else if (arg == "star" && i + 3 < argc) {
      points = std::stoi(argv[++i]);
      r1 = std::stod(argv[++i]);
      r2 = std::stod(argv[++i]);
    } else if (i == 2 && (arg[0] >= '0' && arg[0] <= '9')) {
      radius = std::stod(argv[i]);
    }
  }

  Polygons polys;
  if (polyType == "square") {
    polys = createSquare();
  } else if (polyType == "circle") {
    polys = createCircle(radius);
  } else if (polyType == "ring") {
    polys = createRing(r1, r2);
  } else if (polyType == "star") {
    polys = createStar(points, r1, r2);
  } else {
    std::cerr << "Unknown polygon type: " << polyType << "\n";
    help();
    return 1;
  }

  Manifold m;
  if (revolve) {
    m = Manifold::Revolve(polys, 0, 360.0);
  } else {
    m = Manifold::Extrude(polys, height, divisions, twist, vec2(scaleX, scaleY));
  }

  if (m.Status() != Manifold::Error::NoError) {
    std::cerr << "Error creating mesh: " << (int)m.Status() << "\n";
    return 1;
  }

  std::cout << "Created: " << m.NumTri() << " triangles, " << m.NumVert() << " vertices\n";
  std::cout << "Volume: " << m.Volume() << "\n";

  std::ofstream out(outputFile);
  if (!m.WriteOBJ(out)) {
    std::cerr << "Failed to write " << outputFile << "\n";
    return 1;
  }
  std::cout << "Wrote to " << outputFile << "\n";

  return 0;
}