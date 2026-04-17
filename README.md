# Manifold

A geometry library for creating and operating on manifold triangle meshes.

This is a fork/port of the [original Manifold library](https://github.com/elalish/manifold) by Emmett Lalish.

## About

Manifold is a C++ geometry library dedicated to creating and operating on manifold triangle meshes. A manifold mesh represents a solid object, making it essential for manufacturing, CAD, structural analysis, and 3D printing.

## Features

- Guaranteed manifold output
- Mesh Boolean operations (union, subtract, intersect)
- Constructors for primitive shapes
- Level set function (SDF) evaluation
- Mesh refinement and smoothing
- 2D CrossSection support
- Multiple language bindings (Python, JS/WASM, C)

## Building

```bash
git clone https://github.com/csv610/manifold.git
cd manifold
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DMANIFOLD_TEST=ON ..
cmake --build .
ctest
```

### CMake Options

- `MANIFOLD_PAR=ON` - Enable parallelization (requires TBB)
- `MANIFOLD_CROSS_SECTION=ON` - Enable 2D support (requires Clipper2)
- `MANIFOLD_PYBIND=ON` - Enable Python bindings (requires nanobind)
- `MANIFOLD_TEST=ON` - Build tests (requires GTest)

## Bindings

- **Python**: `manifold3d` on PyPI
- **JS/TS**: `manifold-3d` on npm

## Documentation

- [C++ API](https://manifoldcad.org/docs/html/classmanifold_1_1_manifold.html)
- [Algorithm Docs](https://github.com/elalish/manifold/wiki/Manifold-Library)

## Contributions

This fork adds CLI tools for the Manifold library:

- **manifold_primitives** - Create 3D primitives (cube, sphere, cylinder, torus, tetrahedron)
- **manifold_boolean** - Boolean operations (union, subtract, intersect, Minkowski, batch, split, trim)
- **manifold_transform** - Affine transformations (translate, scale, rotate, mirror, warp, simplify, tolerance)
- **manifold_extrude** - Extrude and revolve 2D polygons
- **manifold_smooth** - Mesh refinement and smoothing
- **manifold_hull** - Convex hull operations
- **manifold_slice** - Slice, project, and split operations
- **manifold_warp** - Custom vertex warping (twist, bend, taper, bulge)
- **manifold_levelset** - Implicit SDF modeling
- **manifold_info** - Mesh analysis (stats, decompose, simplify, raycast)
- **manifold_cross** - 2D CrossSection operations
- **manifold_properties** - Vertex properties (normals, curvature, colors)
- **manifold_meshid** - Mesh ID management (OriginalID, ReserveIDs, AsOriginal)

See [app/README.md](app/README.md) for detailed usage.

## Manifold vs CGAL vs libigl

| Aspect | Manifold | CGAL | libigl |
|--------|---------|------|-------|
| **Focus** | Solid modeling (watertight meshes) | General computational geometry | Geometry processing |
| **Boolean Operations** | Robust, guaranteed manifold | Full support | Uses CGAL internally |
| **Precision** | Double-precision internal | Exact predicates available | Depends on Eigen |
| **Output** | Always manifold | Can fail on edge cases | Can fail on complex inputs |
| **Dependencies** | Minimal (optional) | Heavy | Eigen + optional CGAL |
| **License** | Apache 2.0 | GPL/commercial | GPL |

### Key Distinctions of Manifold

1. **Guaranteed Manifold Output** - Unlike CGAL/libigl, Manifold guarantees the output is always a valid watertight manifold mesh. This is particularly valuable for 3D printing and CAD.

2. **Minimal Dependencies** - No required dependencies (zero by default). CGAL has many heavy dependencies.

3. **Simpler API** - Like libigl, but with focus on solid objects rather than general triangle meshes.

4. **SDF/Level Set Support** - Built-in implicit surface evaluation (LevelSet function).

5. **2D CrossSection** - Integrated 2D polygon operations via Clipper2.

6. **CLI Tools** - This fork adds ready-to-use command-line tools.

7. **Vertex Properties** - Full support for arbitrary vertex properties for materials/textures.

### When to Use What

- **Manifold**: CAD, 3D printing, solid modeling where guaranteed manifold output is critical
- **CGAL**: General computational geometry, exact arithmetic needed, complex 2D/3D algorithms
- **libigl**: Geometry processing, research, visualization with Eigen integration

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

Same as the original project - see LICENSE file.