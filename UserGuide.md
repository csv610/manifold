# Manifold User Guide

A comprehensive guide to the Manifold geometry library for creating and operating on manifold triangle meshes.

---

# Chapter 1: Introduction

## Core Idea

Manifold is a C++ geometry library dedicated to creating and operating on **manifold triangle meshes** - watertight 3D solids that represent closed objects. Unlike general mesh libraries, Manifold guarantees that output meshes are always valid, closed solids suitable for 3D printing, CAD, and manufacturing.

The key insight is that a manifold mesh represents a solid object where:
- Every edge is shared by exactly two triangles
- The mesh forms a closed volume with no holes or gaps
- The mesh is orientable (has consistent inside/outside)

## Implementation

The core class is defined in `include/manifold/manifold.h`:

```cpp
class Manifold {
public:
  Manifold();                                      // Empty constructor
  Manifold(const MeshGL&);                         // From graphics mesh
  Manifold(const MeshGL64&);                       // From 64-bit mesh
  
  Error Status() const;                            // Check validity
  bool IsEmpty() const;
  size_t NumVert() const;
  size_t NumTri() const;
};
```

---

# Chapter 2: Core Data Structures

## Core Idea

Manifold uses two complementary data representations:
1. **MeshGL** - For input/output (graphics-friendly format)
2. **Manifold** - Internal representation (efficient operations)

MeshGL stores geometry in graphics-friendly format with interleaved vertex properties, while Manifold stores optimized internal topology.

## Implementation

### MeshGL Structure

```cpp
// Single-precision mesh (graphics use)
using MeshGL = MeshGLP<float>;
// Double-precision mesh (precision-critical)
using MeshGL64 = MeshGLP<double, uint64_t>;

template <typename Precision, typename I = uint32_t>
struct MeshGLP {
  I numProp;                                        // Properties per vertex (≥3)
  std::vector<Precision> vertProperties;              // Interleaved: x,y,z,[props...]
  std::vector<I> triVerts;                         // Triangle indices (3 per tri)
  std::vector<I> mergeFromVert;                   // Vertex merges for manifold
  std::vector<I> mergeToVert;
  std::vector<I> runIndex;                         // Triangle run starts
  std::vector<uint32_t> runOriginalID;              // Original mesh ID per run
  std::vector<Precision> runTransform;             // 3x4 transform per run
  std::vector<I> faceID;                         // Face IDs for simplification
  std::vector<Precision> halfedgeTangent;         // Tangents for smooth refine
  Precision tolerance;                           // Simplification tolerance
};
```

### Creating a Manifold from MeshGL

```cpp
#include <manifold/manifold.h>

using namespace manifold;

// Create a simple cube mesh
MeshGL cubeMesh;
cubeMesh.numProp = 3;
cubeMesh.vertProperties = {
  // 8 vertices of a cube
  -1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1,  // bottom face
  -1, -1,  1,   1, -1,  1,   1,  1,  1,  -1,  1,  1   // top face
};
cubeMesh.triVerts = {
  // Bottom face
  0, 1, 2,  0, 2, 3,
  // Top face
  4, 6, 5,  4, 7, 6,
  // Front face
  0, 4, 5,  0, 5, 1,
  // Back face
  2, 6, 7,  2, 7, 3,
  // Left face
  0, 3, 7,  0, 7, 4,
  // Right face  
  1, 5, 6,  1, 6, 2
};

// Create Manifold from mesh
Manifold cube(cubeMesh);

// Check status
if (cube.Status() != Manifold::Error::NoError) {
  std::cerr << "Invalid mesh: " << ToString(cube.Status()) << std::endl;
}
```

### Extracting MeshGL from Manifold

```cpp
// Get single-precision mesh
MeshGL output = cube.GetMeshGL();

// Get double-precision mesh  
MeshGL64 output64 = cube.GetMeshGL64();

// Get with specific normal channel
MeshGL withNormals = cube.GetMeshGL(normalIdx);
```

---

# Chapter 3: Primitive Constructors

## Core Idea

Manifold provides built-in constructors for common 3D primitives. These guarantee manifold output with correct topology.

## Implementation

### Available Primitives

```cpp
class Manifold {
public:
  // Tetrahedron - 4-faced pyramid
  static Manifold Tetrahedron();
  
  // Cube with configurable size and centering
  static Manifold Cube(vec3 size = vec3(1.0), bool center = false);
  
  // Cylinder with different radii at ends (for cones)
  static Manifold Cylinder(
    double height,           // Height of cylinder
    double radiusLow,      // Radius at bottom
    double radiusHigh = -1.0,  // -1 means same as radiusLow
    int circularSegments = 0,  // 0 = auto
    bool center = false
  );
  
  // Sphere with configurable segments
  static Manifold Sphere(double radius, int circularSegments = 0);
};
```

### Usage Examples

```cpp
// Basic primitives
Manifold cube = Manifold::Cube();
Manifold sphere = Manifold::Sphere(1.0);
Manifold cylinder = Manifold::Cylinder(2.0, 1.0);

// Custom-sized cube (2x1x0.5), centered at origin
Manifold box = Manifold::Cube(vec3(2.0, 1.0, 0.5), true);

// Cone (cylinder with different radii)
Manifold cone = Manifold::Cylinder(2.0, 0.0, 1.0);  // point at bottom

// Cylinder with 32 segments
Manifold pipe = Manifold::Cylinder(1.0, 0.5, 0.5, 32);

// Sphere with high detail
Manifold ball = Manifold::Sphere(1.0, 64);  // 64 segments
```

### Extrude and Revolve

For creating 3D from 2D polygons:

```cpp
// Extrude a 2D polygon to 3D
static Manifold Extrude(
  const Polygons& crossSection,  // 2D polygon with holes
  double height,                // Extrusion distance
  int nDivisions = 0,          // Vertical divisions
  double twistDegrees = 0.0,    // Twist during extrusion
  vec2 scaleTop = vec2(1.0)    // Scale factor at top
);

// Revolve a 2D polygon around an axis
static Manifold Revolve(
  const Polygons& crossSection,
  int circularSegments = 0,
  double revolveDegrees = 360.0
);
```

Example:

```cpp
using namespace manifold;

// Create a square with a circular hole
Polygons profile;
profile.push_back({               // Outer square
  vec2(-1, -1), vec2(1, -1), vec2(1, 1), vec2(-1, 1)
});
profile[0].push_back(vec2(-0.5, -0.5));  // Mark as hole (last point)

// Create extruded shape
Manifold extruded = Manifold::Extrude(profile, 2.0);

// Create revolved vase
Manifold vase = Manifold::Revolve(profile, 32, 270.0);
```

---

# Chapter 4: Boolean Operations

## Core Idea

Boolean operations combine two manifold meshes into new shapes. Manifold guarantees the output is always a valid manifold mesh.

The three fundamental operations are:
- **Union (Add)** - Combine volumes
- **Subtract (Difference)** - Remove one volume from another
- **Intersect** - Keep only overlapping volume

## Implementation

### Boolean Operations API

```cpp
class Manifold {
public:
  // Generic Boolean operation
  Manifold Boolean(const Manifold& second, OpType op) const;
  
  // Batch Boolean for multiple operands
  static Manifold BatchBoolean(
    const std::vector<Manifold>& manifolds,
    OpType op
  );
  
  // Operator shortcuts
  Manifold operator+(const Manifold&) const;  // Union
  Manifold operator-(const Manifold&) const;  // Subtract
  Manifold operator^(const Manifold&) const;  // Intersect
  
  // In-place operators
  Manifold& operator+=(const Manifold&);
  Manifold& operator-=(const Manifold&);
  Manifold& operator^=(const Manifold&);
};

enum class OpType { Add, Subtract, Intersect };
```

### Usage Examples

```cpp
// Create two shapes
Manifold cube = Manifold::Cube(vec3(1.0));
Manifold sphere = Manifold::Sphere(0.6);

// Union - combine shapes
Manifold combined = cube + sphere;

// Subtract - drill a hole
Manifold hollow = cube - sphere;

// Intersect - find overlap
Manifold overlap = cube ^ sphere;

// Using Boolean method directly
Manifold result = cube.Boolean(sphere, OpType::Subtract);

// Batch union of multiple shapes
std::vector<Manifold> shapes = {
  Manifold::Cube(vec3(1.0)),
  Manifold::Sphere(0.5),
  Manifold::Cylinder(1.0, 0.3)
};
Manifold merged = Manifold::BatchBoolean(shapes, OpType::Add);
```

### Advanced Boolean Operations

```cpp
class Manifold {
public:
  // Split one manifold by another (returns both parts)
  std::pair<Manifold, Manifold> Split(const Manifold&) const;
  
  // Split by infinite plane
  std::pair<Manifold, Manifold> SplitByPlane(vec3 normal, double originOffset) const;
  
  // Keep portion on one side of plane
  Manifold TrimByPlane(vec3 normal, double originOffset) const;
  
  // Minkowski sum (sweep one shape along another)
  Manifold MinkowskiSum(const Manifold&) const;
  
  // Minkowski difference
  Manifold MinkowskiDifference(const Manifold&) const;
};
```

Example:

```cpp
// Split a cube with a sphere
Manifold cube = Manifold::Cube(vec3(2.0));
Manifold sphere = Manifold::Sphere(0.8);

auto [part1, part2] = cube.Split(sphere);
// part1 contains the sphere's volume
// part2 contains the rest

// Split by plane at z=0
auto [front, back] = cube.SplitByPlane(vec3(0, 0, 1), 0.0);

// Trim to one side
Manifold frontHalf = cube.TrimByPlane(vec3(0, 0, 1), 0.0);

// Create a hollowed-out shape using Minkowski
Manifold box = Manifold::Cube(vec3(2.0));
Manifold tube = Manifold::Cylinder(3.0, 0.5);
Manifold swept = box.MinkowskiSum(tube);
```

---

# Chapter 5: Transformations

## Core Idea

Manifold supports affine transformations that preserve manifoldness. Transformations can be chained to produce complex operations.

## Implementation

### Transformation API

```cpp
class Manifold {
public:
  // Translation
  Manifold Translate(vec3) const;
  
  // Uniform or non-uniform scaling
  Manifold Scale(vec3) const;
  
  // Rotation around axes (degrees)
  Manifold Rotate(
    double xDegrees, 
    double yDegrees = 0.0, 
    double zDegrees = 0.0
  ) const;
  
  // Mirror across plane through origin
  Manifold Mirror(vec3) const;
  
  // Full 3x4 affine transform
  Manifold Transform(const mat3x4&) const;
  
  // Custom vertex manipulation
  Manifold Warp(std::function<void(vec3&)>) const;
  Manifold WarpBatch(std::function<void(VecView<vec3>)>) const;
  
  // Simplification
  Manifold Simplify(double tolerance = 0) const;
  Manifold SetTolerance(double) const;
};
```

### Usage Examples

```cpp
Manifold cube = Manifold::Cube();

// Move 5 units in +Z direction
Manifold moved = cube.Translate(vec3(0, 0, 5));

// Scale uniformly by 2x
Manifold big = cube.Scale(vec3(2.0, 2.0, 2.0));

// Non-uniform scale
Manifold flat = cube.Scale(vec3(1.0, 1.0, 0.1));

// Rotate 45 degrees around Y axis
Manifold turned = cube.Rotate(0, 45, 0);

// Rotate around multiple axes
Manifold rotated = cube.Rotate(30, 45, 60);

// Mirror across XZ plane (reflect Y)
Manifold reflected = cube.Mirror(vec3(0, 1, 0));

// Custom transformation matrix
mat3x4 transform = la::identity;
transform.col(3) = vec4(1, 2, 3, 1);  // translation
Manifold transformed = cube.Transform(transform);
```

### Custom Warping

```cpp
// Warp each vertex individually
Manifold warped = cube.Warp([](vec3& v) {
  // Twist: rotate based on Y height
  double angle = v.y * 0.5;
  double c = std::cos(angle);
  double s = std::sin(angle);
  vec3 rotated(c * v.x - s * v.z, v.y, s * v.x + c * v.z);
  v = rotated;
});

// Batch warp (more efficient for many vertices)
Manifold bent = cube.WarpBatch([](VecView<vec3> verts) {
  for (auto& v : verts) {
    v.y += v.x * v.x * 0.1;  // Parabolic bend
  }
});
```

---

# Chapter 6: Vertex Properties

## Core Idea

Vertex properties allow storing additional data per vertex beyond position - normals, colors, UV coordinates, etc. Manifold preserves properties through all operations.

## Implementation

### Properties API

```cpp
class Manifold {
public:
  // Set custom properties
  Manifold SetProperties(
    int numProp,                    // Properties per vertex
    std::function<void(
      double* prop,                 // Output property array
      vec3 position,                // Vertex position
      const double* existingProps   // Existing properties
    )> propFunc
  ) const;
  
  // Calculate curvature properties
  Manifold CalculateCurvature(
    int gaussianIdx,               // Channel for Gaussian curvature
    int meanIdx                      // Channel for mean curvature
  ) const;
  
  // Calculate vertex normals
  Manifold CalculateNormals(
    int normalIdx,                  // Channel to store normals (≥3)
    double minSharpAngle = 60       // Angle for sharp edges
  ) const;
};
```

### Example

```cpp
// Create manifold with color properties
Manifold cube = Manifold::Cube();

// Add RGB color (3 properties per vertex)
Manifold colored = cube.SetProperties(6,  // 3 (pos) + 3 (color)
  [](double* prop, vec3 pos, const double* existing) {
    prop[0] = pos.x;  // x
    prop[1] = pos.y;  // y  
    prop[2] = pos.z;  // z
    prop[3] = 1.0;    // r (red)
    prop[4] = 0.0;   // g (green)
    prop[5] = 0.0;   // b (blue)
  }
);

// Calculate vertex normals
Manifold withNormals = cube.CalculateNormals(3);  // Store at channel 3

// Get the mesh
MeshGL out = withNormals.GetMeshGL(3);  // normalIdx = 3
```

---

# Chapter 7: Smoothing and Refinement

## Core Idea

Manifold can refine meshes (increase triangle count) while preserving manifoldness. Smooth refinement creates curved surfaces from flat triangles.

## Implementation

### Smoothing API

```cpp
class Manifold {
public:
  // Refine by dividing each triangle into 4
  Manifold Refine(int factor) const;
  
  // Refine to target edge length
  Manifold RefineToLength(double length) const;
  
  // Refine to target tolerance
  Manifold RefineToTolerance(double tolerance) const;
  
  // Apply curvature-based smoothing
  Manifold SmoothByNormals(int normalIdx) const;
  
  // Smooth while preserving sharp edges
  Manifold SmoothOut(
    double minSharpAngle = 60,
    double minSmoothness = 0
  ) const;
  
  // Create smooth mesh from faceted input
  static Manifold Smooth(
    const MeshGL& mesh,
    const std::vector<Smoothness>& sharpenedEdges = {}
  );
};
```

### Usage Examples

```cpp
Manifold cube = Manifold::Cube();

// Refine: each triangle becomes 4 triangles
Manifold refined = cube.Refine(2);  // 4x triangles (2 divisions)

// Refine to specific edge length
Manifold smoothCube = cube.RefineToLength(0.1);

// Use existing normals for smooth surface
Manifold sphere = Manifold::Sphere(1.0, 3);  // Low-poly sphere
Manifold smoothSphere = sphere.SmoothByNormals(3);

// Smooth with sharp edges at 70 degree angles
Manifold smoothed = sphere.SmoothOut(70, 0.0);

// Create smooth from faceted mesh
MeshGL faceted = cube.GetMeshGL();
std::vector<Smoothness> edges;
edges.push_back({0, 1.0});  // Sharpen first edge with factor 1.0
Manifold smooth = Manifold::Smooth(faceted, edges);
```

---

# Chapter 8: Convex Hull

## Core Idea

The convex hull is the smallest convex body containing all given points. Manifold computes exact convex hulls of points or manifolds.

## Implementation

```cpp
class Manifold {
public:
  // Convex hull of a single manifold
  Manifold Hull() const;
  
  // Convex hull of multiple manifolds
  static Manifold Hull(const std::vector<Manifold>& manifolds);
  
  // Convex hull of points
  static Manifold Hull(const std::vector<vec3>& pts);
};
```

### Example

```cpp
// Hull of a manifold
Manifold sphere = Manifold::Sphere(1.0);
Manifold hull = sphere.Hull();  // Same as sphere

// Hull of multiple shapes
std::vector<Manifold> shapes = {
  Manifold::Translate(vec3(0, 0, 0)),
  Manifold::Translate(vec3(1, 1, 1)),
  Manifold::Translate(vec3(-1, 1, -1))
};
Manifold combinedHull = Manifold::Hull(shapes);

// Hull from points
std::vector<vec3> points = {
  vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0),
  vec3(0, 0, 1), vec3(1, 1, 0), vec3(1, 0, 1),
  vec3(0, 1, 1), vec3(1, 1, 1)
};
Manifold pointHull = Manifold::Hull(points);
```

---

# Chapter 9: Level Set (SDF) Modeling

## Core Idea

Level set functions (Signed Distance Functions, SDF) define implicit surfaces. Manifold can convert any SDF into a manifold mesh using marching cubes.

## Implementation

```cpp
class Manifold {
public:
  // Create manifold from SDF
  static Manifold LevelSet(
    std::function<double(vec3)> sdf,    // SDF function: returns distance
    Box bounds,                        // Bounding box
    double edgeLength,                // Target edge length
    double level = 0,                 // Isosurface level (0 = surface)
    double tolerance = -1,              // Detail level (-1 = default)
    bool canParallel = true            // Enable parallelization
  );
};

struct Box {
  vec3 min;
  vec3 max;
  
  Box() : min(la::max), max(la::min) {}
  Box(vec3 min, vec3 max) : min(min), max(max) {}
  vec3 Center() const { return (min + max) / 2; }
  vec3 Size() const { return max - min; }
  bool IsEmpty() const { return min.x >= max.x; }
};
```

### Example

```cpp
using namespace manifold;

// SDF for a sphere
auto sphereSDF = [](vec3 p) -> double {
  return 1.0 - la::length(p);  // Radius = 1
};

Box bounds(vec3(-2), vec3(2));
Manifold sphere = Manifold::LevelSet(
  sphereSDF,
  bounds,
  0.1,      // Edge length
  0.0        // Isosurface at distance 0
);

// SDF for union of two shapes
auto unionSDF = [](vec3 p) -> double {
  double d1 = 1.0 - la::length(p - vec3(-0.5, 0, 0));
  double d2 = 1.0 - la::length(p - vec3(0.5, 0, 0));
  return std::min(d1, d2);  // Union = min
};

// SDF for smooth union
auto smoothUnionSDF = [](vec3 p) -> double {
  double d1 = 1.0 - la::length(p - vec3(-0.5, 0, 0));
  double d2 = 1.0 - la::length(p - vec3(0.5, 0, 0));
  double k = 0.5;  // Smoothness parameter
  double h = std::max(k - std::abs(d1 - d2), 0.0) / k;
  return std::min(d1, d2) - h * h * k * 0.25;
};
```

---

# Chapter 10: Information and Analysis

## Core Idea

Manifold provides comprehensive mesh analysis functions.

## Implementation

```cpp
class Manifold {
public:
  // Basic information
  Error Status() const;
  bool IsEmpty() const;
  size_t NumVert() const;
  size_t NumEdge() const;
  size_t NumTri() const;
  size_t NumProp() const;
  size_t NumPropVert() const;
  Box BoundingBox() const;
  int Genus() const;
  double GetTolerance() const;
  
  // Measurements
  double SurfaceArea() const;
  double Volume() const;
  
  // Find minimum gap to another manifold
  double MinGap(const Manifold& other, double searchLength) const;
  
  // Ray casting
  std::vector<RayHit> RayCast(vec3 origin, vec3 direction) const;
  
  // Decompose into individual manifolds
  std::vector<Manifold> Decompose() const;
};
```

### Example

```cpp
Manifold cube = Manifold::Cube();

// Check validity
if (cube.Status() != Manifold::Error::NoError) {
  std::cerr << "Error: " << ToString(cube.Status()) << std::endl;
}

// Get statistics
std::cout << "Vertices: " << cube.NumVert() << std::endl;
std::cout << "Triangles: " << cube.NumTri() << std::endl;
std::cout << "Edges: " << cube.NumEdge() << std::endl;
std::cout << "Surface area: " << cube.SurfaceArea() << std::endl;
std::cout << "Volume: " << cube.Volume() << std::endl;
std::cout << "Bounding box: " << cube.BoundingBox().min 
          << " to " << cube.BoundingBox().max << std::endl;
std::cout << "Genus: " << cube.Genus() << std::endl;  // 0 for cube

// Ray casting
vec3 origin(0, 0, -5);
vec3 direction(0, 0, 1);
auto hits = cube.RayCast(origin, direction);
for (const auto& hit : hits) {
  std::cout << "Hit at distance: " << hit.distance << std::endl;
}
```

---

# Chapter 11: Mesh ID and Relationships

## Core Idea

Manifold tracks the relationship between output meshes and their input meshes through IDs. This allows preserving materials and other per-object properties through operations.

## Implementation

```cpp
class Manifold {
public:
  // Get the original ID (for material lookup)
  int OriginalID() const;
  
  // Get manifold as an "original" for future reference
  Manifold AsOriginal() const;
  
  // Reserve unique IDs for batch operations
  static uint32_t ReserveIDs(uint32_t count);
};
```

### Example

```cpp
// Create two manifolds with different IDs
uint32_t boxID = Manifold::ReserveIDs(1);
uint32_t sphereID = Manifold::ReserveIDs(1);

Manifold box = Manifold::Cube(vec3(1.0));
Manifold sphere = Manifold::Sphere(0.5);

// Combine them
Manifold combined = box + sphere;

// Get output mesh with run information
MeshGL out = combined.GetMeshGL();

// Each run has its original ID preserved
for (size_t i = 0; i < out.NumRun(); i++) {
  uint32_t id = out.runOriginalID[i];
  std::cout << "Run " << i << " has ID " << id << std::endl;
  
  // Look up material for this ID
  auto it = materials.find(id);
  if (it != materials.end()) {
    applyMaterial(triangles[i], it->second);
  }
}
```

---

# Chapter 12: CrossSection (2D)

## Core Idea

The CrossSection class provides 2D polygon operations using Clipper2. It's the 2D analogue of Manifold.

## Implementation

```cpp
class CrossSection {
public:
  // Constructors
  CrossSection();  // Empty
  CrossSection(const Polygons& polygons);
  CrossSection(const Polygons& polygons, double accuracy);
  
  // Boolean operations
  CrossSection Boolean(const CrossSection&, OpType) const;
  static CrossSection BatchBoolean(const std::vector<CrossSection>&, OpType);
  
  // Transformations
  CrossSection Translate(vec2) const;
  CrossSection Scale(vec2) const;
  CrossSection Rotate(double degrees) const;
  CrossSection Mirror(vec2) const;
  
  // Offset operations
  CrossSection Offset(double delta, 
    JoinType join = JoinType::Square, 
    double miterLimit = 2.0) const;
  
  // Hull
  CrossSection Hull() const;
  
  // Decompose into polygons
  std::vector<Polygons> Decompose() const;
  
  // Access
  Polygons ToPolygons() const;
  Box BoundingBox() const;
  bool IsEmpty() const;
};

enum class JoinType { Square, Round, Miter };
```

### Example

```cpp
using namespace manifold;

// Create 2D polygon with hole
Polygons square;
square.push_back({               // Outer square (CCW)
  vec2(-1, -1), vec2(1, -1), vec2(1, 1), vec2(-1, 1)
});
square.push_back({               // Inner hole (CW)
  vec2(-0.5, -0.5), vec2(-0.5, 0.5), 
  vec2(0.5, 0.5), vec2(0.5, -0.5)
});

CrossSection shape(square);

// Offset to create thick border
CrossSection thick = shape.Offset(0.1, JoinType::Round);

// Boolean: union with another shape
CrossSection other(vec2(2, 0), vec2(3, 1));
CrossSection combined = shape.Boolean(other, OpType::Add);

// Rotate 45 degrees
CrossSection rotated = shape.Rotate(45);

// Decompose into separate polygons
auto parts = combined.Decompose();
```

---

# Chapter 13: Input and Output

## Core Idea

Manifold uses its own OBJ variant for lossless round-tripping with full precision and metadata.

## Implementation

```cpp
class Manifold {
public:
  // Read from stream
  static Manifold ReadOBJ(std::istream& stream);
  
  // Write to stream
  bool WriteOBJ(std::ostream& stream) const;
};
```

### Example

```cpp
#include <fstream>

// Writing
Manifold cube = Manifold::Cube();
std::ofstream out("cube.obj");
cube.WriteOBJ(out);
out.close();

// Reading
std::ifstream in("cube.obj");
Manifold loaded = Manifold::ReadOBJ(in);
in.close();

if (loaded.Status() != Manifold::Error::NoError) {
  std::cerr << "Failed to read cube.obj" << std::endl;
}
```

---

# Chapter 14: Best Practices

## Core Idea

Follow these practices for optimal results with Manifold.

## Guidelines

### 1. Check Error Status

```cpp
Manifold result = shapeA + shapeB;
if (result.Status() != Manifold::Error::NoError) {
  std::cerr << "Operation failed: " << ToString(result.Status()) << std::endl;
  // Handle error
}
```

### 2. Use Appropriate Precision

```cpp
// For 3D printing: use default precision is sufficient
Manifold print = cube.Boolean(sphere, OpType::Subtract);

// For precision-critical applications: verify tolerance
if (print.GetTolerance() > 0.001) {
  std::cout << "Warning: high tolerance" << std::endl;
}
```

### 3. Preserve OriginalIDs for Materials

```cpp
// Reserve IDs before creating manifolds
uint32_t matRed = Manifold::ReserveIDs(1);
uint32_t matBlue = Manifold::ReserveIDs(1);

// Create colored shapes
Manifold redPart = ...;
Manifold bluePart = ...;

// Combine
Manifold combined = redPart + bluePart;

// On output, look up materials by ID
MeshGL out = combined.GetMeshGL();
for (size_t i = 0; i < out.runOriginalID.size(); i++) {
  uint32_t id = out.runOriginalID[i];
  // Apply material based on ID
}
```

### 4. Use BatchBoolean for Multiple Shapes

```cpp
// Better: single operation
std::vector<Manifold> all = {a, b, c, d, e};
Manifold merged = Manifold::BatchBoolean(all, OpType::Add);

// Instead of: multiple operations
// Manifold merged = ((a + b) + c) + d + e;  // Less efficient
```

### 5. Choose Appropriate Detail Level

```cpp
// For preview: lower detail
Manifold preview = sphere.RefineToLength(0.5);

// For final: higher detail  
Manifold final = sphere.RefineToLength(0.05);

// For 3D printing: balance detail and file size
Manifold print = sphere.RefineToLength(0.1);
```

---

# Appendix A: Error Codes

| Error | Meaning |
|-------|---------|
| NoError | Success |
| NonFiniteVertex | NaN or Inf vertex |
| NotManifold | Not a valid 2-manifold |
| VertexOutOfBounds | Index out of range |
| PropertiesWrongLength | Property count mismatch |
| MissingPositionProperties | Position not provided |
| MergeVectorsDifferentLengths | Merge vectors unequal |
| MergeIndexOutOfBounds | Invalid merge index |
| TransformWrongLength | Transform matrix error |
| RunIndexWrongLength | Run index error |
| FaceIDWrongLength | Face ID error |
| InvalidConstruction | Construction failed |
| ResultTooLarge | Mesh too large |
| InvalidTangents | Invalid tangent data |

---

# Appendix B: CLI Tools

This fork provides command-line tools:

| Tool | Purpose |
|------|---------|
| `manifold_primitives` | Create 3D primitives |
| `manifold_boolean` | Boolean operations |
| `manifold_transform` | Transformations |
| `manifold_extrude` | Extrude/revolve polygons |
| `manifold_smooth` | Smoothing operations |
| `manifold_hull` | Convex hull |
| `manifold_slice` | Slice operations |
| `manifold_levelset` | SDF modeling |
| `manifold_info` | Mesh analysis |
| `manifold_cross` | 2D CrossSection |

See `app/README.md` for detailed usage.

---

# Appendix C: Building

## CMake Build

```bash
git clone https://github.com/csv610/manifold.git
cd manifold
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DMANIFOLD_TEST=ON ..
cmake --build . -j$(nproc)
ctest
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| MANIFOLD_PAR | OFF | Enable parallel (TBB) |
| MANIFOLD_CROSS_SECTION | ON | Enable 2D support |
| MANIFOLD_PYBIND | OFF | Python bindings |
| MANIFOLD_TEST | ON | Build tests |
| MANIFOLD_DEBUG | OFF | Debug mode |

---

*Manifold User Guide - Version 3.4.1*