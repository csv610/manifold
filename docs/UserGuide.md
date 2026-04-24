# A Student's Guide to Manifold Geometry
**A Comprehensive Introduction to Robust 3D Modeling with the Manifold Library**

---

## Preface: Why Manifold Geometry?

In the world of computer graphics, we often deal with "polygon soups"—collections of triangles that might look like a 3D object but lack the structural integrity required for the physical world. If you want to 3D print a model, run a physics simulation, or perform precise engineering calculations, your mesh must be **manifold**.

A manifold mesh is "watertight." It has a clearly defined inside and outside, no holes, and no self-intersections. The Manifold library is designed to make these robust operations easy, ensuring that every operation you perform results in a valid, physical-ready object.

This guide is designed for undergraduate students in Computer Science, Engineering, or Mathematics. We will move from basic concepts to complex algorithms, building a foundation in digital geometry processing.

---

## Chapter 1: Geometric Foundations

### 1.1 What is a Manifold?
In topology, a 2-manifold is a surface where every point has a neighborhood that looks like a flat disk. In the context of 3D meshes, this translates to three simple rules:
1. **Edge Manifoldness**: Every edge is shared by exactly two triangles.
2. **Vertex Manifoldness**: The triangles surrounding a vertex form a single, closed "fan."
3. **Watertightness**: The mesh forms a closed volume with no gaps or boundaries.

### 1.2 The "Soup" vs. The "Solid"
General graphics libraries (like Three.js or OpenGL) treat meshes as a collection of triangles for rendering. The Manifold library treats them as **solids**. When you subtract a sphere from a cube in Manifold, it doesn't just delete triangles; it recalculates the topology to ensure the resulting "hole" is capped with new geometry.

---

## Chapter 2: Data Representations

Manifold uses two primary data structures to balance performance and ease of use.

### 2.1 MeshGL: The Graphics Interface
`MeshGL` is designed to be compatible with modern graphics APIs. It uses interleaved arrays (x, y, z, followed by optional properties like colors or normals).

```cpp
struct MeshGL {
  uint32_t numProp;                   // Number of properties per vertex (min 3: x,y,z)
  std::vector<float> vertProperties;  // [x1, y1, z1, r1, g1, b1, x2, ...]
  std::vector<uint32_t> triVerts;     // [v1, v2, v3, v4, v5, v6, ...]
};
```

### 2.2 The Manifold Object: The Mathematical Engine
The `Manifold` class is an opaque, immutable object that stores the mesh in a highly optimized internal format. You convert `MeshGL` into a `Manifold` to perform operations, then convert it back to `MeshGL` for rendering.

**Conceptual Checkpoint**: Why use two formats? 
*Answer: Graphics cards need simple arrays (MeshGL), but geometric algorithms need complex topological links (Manifold).*

---

## Chapter 3: Primitive Construction

The simplest way to start is by using built-in mathematical primitives.

### 3.1 Basic Solids
```cpp
using namespace manifold;

// Create a 1x1x1 cube centered at the origin
Manifold cube = Manifold::Cube(vec3(1.0), true);

// Create a sphere with radius 1.0 and 32 segments
Manifold sphere = Manifold::Sphere(1.0, 32);

// Create a cylinder (height, radiusLow, radiusHigh)
Manifold cone = Manifold::Cylinder(2.0, 1.0, 0.0); 
```

### 3.2 From 2D to 3D: Extrusion and Revolution
You can create complex solids by "sweeping" 2D shapes (polygons) through 3D space.

*   **Extrude**: Pulls a 2D shape along the Z-axis.
*   **Revolve**: Spins a 2D shape around the Z-axis to create radially symmetric objects like vases or wheels.

---

## Chapter 4: Constructive Solid Geometry (CSG)

CSG is a modeling technique that uses Boolean operations to create complex shapes from simple ones. This is the "heart" of the Manifold library.

### 4.1 The Three Core Operations
1.  **Union (`+`)**: Joins two volumes into one.
2.  **Difference (`-`)**: Subtracts the second volume from the first.
3.  **Intersection (`^`)**: Keeps only the volume where both shapes overlap.

```cpp
Manifold base = Manifold::Cube(vec3(2.0), true);
Manifold cutter = Manifold::Sphere(1.2, 32);

// Create a cube with a spherical hole
Manifold result = base - cutter;
```

### 4.2 Mathematical Robustness
Manifold uses symbolic computations and exact arithmetic to handle "degenerate" cases—for example, when a sphere just barely touches the face of a cube. This prevents the "cracks" and "glitches" common in other CSG libraries.

---

## Chapter 5: Spatial Transformations

Objects are transformed using 4x4 matrices, though the library provides helper functions for common operations.

### 5.1 Linear Transformations
*   **Translate**: Shifts the object in space.
*   **Rotate**: Turns the object (in degrees) around the X, Y, or Z axis.
*   **Scale**: Increases or decreases size.

```cpp
// Chain transformations together
Manifold shape = Manifold::Cube()
    .Scale(vec3(2.0, 0.5, 1.0))
    .Rotate(0, 45, 0)
    .Translate(vec3(10, 0, 0));
```

### 5.2 Non-Linear Warping
The `Warp` function allows you to define a mathematical function that moves every vertex of a mesh. This can be used for twisting, bending, or creating organic shapes.

---

## Chapter 6: Surface Refinement and Smoothing

A raw mesh often looks "faceted" (you can see the flat triangles). Manifold provides tools to smooth these surfaces while maintaining manifoldness.

### 6.1 Subdivisions
`Refine(n)` splits every triangle into smaller triangles. This increases the "resolution" of your mesh.

### 6.2 Curvature Smoothing
`SmoothByNormals` or `SmoothOut` uses the normals of the mesh to "puff out" the surface, turning a low-poly cube into a rounded, smooth-edged box.

---

## Chapter 7: Advanced Geometric Algorithms

### 7.1 Convex Hull
The convex hull is the "shrink-wrap" of a shape. It is the smallest convex volume that contains all the points of the input.
```cpp
Manifold messyPoints = ...;
Manifold hull = messyPoints.Hull();
```

### 7.2 Signed Distance Fields (SDFs)
An SDF is a function that tells you how far any point in space is from the surface of an object. Manifold can convert these abstract functions into concrete meshes using the `LevelSet` function.

---

## Chapter 8: Analysis and Metadata

Manifold isn't just for making shapes; it's for analyzing them.

### 8.1 Physical Properties
```cpp
double v = shape.Volume();
double a = shape.SurfaceArea();
Box bounds = shape.BoundingBox();
```

### 8.2 The Genus
In topology, the **genus** of a surface is the number of "holes" (like the hole in a donut). A sphere has genus 0, a torus has genus 1. Manifold can calculate this value to help you understand the topology of your model.

---

## Appendix: Implementation Checklist for Students

When integrating Manifold into your project, follow this workflow:
1.  **Define your 2D profiles** using `Polygons`.
2.  **Lift to 3D** using `Extrude`, `Revolve`, or primitives.
3.  **Compose** your model using Boolean operations.
4.  **Transform** and **Smooth** as needed.
5.  **Analyze** the results (Volume, Genus).
6.  **Export** to `MeshGL` for visualization.

---

## Further Exploration: Advanced Reading

For students interested in the high-level algorithms and mathematical proofs behind the Manifold library, the following research papers (available in this directory) are recommended:

*   **Robust Boolean Operations**: See `RobustBoolean.pdf` for the core logic of how Manifold handles edge cases and precision.
*   **Spatial Indexing**: See `ParallelBVH.pdf` to learn how the library uses Bounding Volume Hierarchies to speed up geometric queries.
*   **Triangle Interpolation**: See `TriangleInterpolation.pdf` for the mathematics behind surface smoothing.
*   **Triangulation Algorithms**: See `Triangulation.pdf` for details on how polygons are converted into triangle meshes.

---
*Generated for the Manifold Geometry Project - Version 3.4.1*
