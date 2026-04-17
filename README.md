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

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

Same as the original project - see LICENSE file.