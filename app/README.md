# Manifold CLI Tools

Command-line utilities for the Manifold geometry library.

## Building

```bash
mkdir build && cd build
cmake .. -DMANIFOLD_TEST=ON -DMANIFOLD_CROSS_SECTION=ON
cmake --build . --target manifold_primitives manifold_boolean manifold_transform manifold_extrude manifold_smooth manifold_hull manifold_slice manifold_warp manifold_levelset manifold_info manifold_cross manifold_properties manifold_meshid
```

## Tools

### manifold_primitives

Create 3D primitive shapes.

```bash
manifold_primitives <type> [options]
```

Types:
- `cube [size]` - Create a cube
- `sphere [radius]` - Create a sphere
- `cylinder [height] [radius]` - Create a cylinder
- `torus [r1] [r2]` - Create a torus
- `tetrahedron` - Create a tetrahedron

Options:
- `--o <file>` - Output OBJ file
- `--center` - Center the primitive

Example:
```bash
manifold_primitives cube 2 --o cube.obj
manifold_primitives sphere 1.5 --o sphere.obj
```

---

### manifold_boolean

Boolean operations on meshes.

```bash
manifold_boolean <op> <inputs...> [options]
```

Operations:
- `add` - Union of two meshes
- `subtract` - Difference (first minus second)
- `intersect` - Intersection
- `min` - Minkowski sum
- `max` - Minkowski difference
- `batch` - Batch boolean (multiple inputs)
- `split` - Split first mesh by second
- `splitplane` - Split by plane (input + nx ny nz d)
- `trimplane` - Trim by plane (input + nx ny nz d)
- `mingap` - Find minimum gap between meshes

Options:
- `--o <file>` - Output OBJ file
- `--search <len>` - Search length for mingap

Example:
```bash
manifold_boolean add cube.obj sphere.obj --o union.obj
manifold_boolean batch a.obj b.obj c.obj --o combined.obj
manifold_boolean splitplane cube.obj 0 0 1 0 --o cut.obj
manifold_boolean mingap obj1.obj obj2.obj --search 10
```

---

### manifold_transform

Affine transformations on meshes.

```bash
manifold_transform <input.obj> [transforms...] [-o output.obj]
```

Transforms:
- `translate <x> <y> <z>` - Translate mesh
- `scale <x> <y> <z>` - Scale mesh
- `rotate <x> <y> <z>` - Rotate in degrees
- `mirror <x> <y> <z>` - Mirror across plane
- `tolerance <val>` - Set precision tolerance

Options:
- `-o <file>` - Output OBJ file
- `-info` - Show mesh info

Example:
```bash
manifold_transform cube.obj translate 1 0 0 --o moved.obj
manifold_transform sphere.obj scale 2 2 2 --o big_sphere.obj
manifold_transform model.obj rotate 0 0 45 --o rotated.obj
```

---

### manifold_extrude

Extrude or revolve 2D polygons into 3D.

```bash
manifold_extrude <poly> [options] [-o output.obj]
```

Polygons:
- `square` - Default unit square at origin
- `circle <radius>` - Circle with given radius
- `ring <r1> <r2>` - Ring between two radii
- `star <points> <r1> <r2>` - Star shape

Options:
- `-height <h>` - Extrusion height (default 1.0)
- `-revolve` - Revolve instead of extrude
- `-twist <degrees>` - Twist the top
- `-scale <x> <y>` - Scale top (extrude only)
- `-divisions <n>` - Number of divisions
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_extrude circle 1 -height 2 --o cylinder.obj
manifold_extrude star 5 1 2 -height 0.5 --o gear.obj
manifold_extrude circle 1 -revolve --o sphere_half.obj
```

---

### manifold_smooth

Mesh refinement and smoothing.

```bash
manifold_smooth <input.obj> [options] [-o output.obj]
```

Options:
- `-refine <n>` - Refine n times (doubles triangles)
- `-length <len>` - Refine to edge length
- `-tolerance <tol>` - Refine to tolerance
- `-normals` - Calculate smooth normals
- `-smooth` - Smooth sharp edges
- `-info` - Show mesh info
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_smooth cube.obj -refine 2 --o smooth_cube.obj
manifold_smooth sphere.obj -normals --o smooth_sphere.obj
```

---

### manifold_hull

Convex hull operations.

```bash
manifold_hull <input.obj> [options] [-o output.obj]
```

Options:
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_hull points.obj --o hull.obj
```

---

### manifold_slice

Slice and project 3D meshes.

```bash
manifold_slice <input.obj> [options] [-o output.obj]
```

Options:
- `-slice <height>` - Slice at height (returns 2D polygon)
- `-project` - Project to XY plane
- `-split <height>` - Split at height
- `-o <file>` - Output file

Example:
```bash
manifold_slice cylinder.obj -slice 0 --o cross_section.txt
manifold_slice model.obj -project --o projected.obj
```

---

### manifold_warp

Custom vertex warping.

```bash
manifold_warp <input.obj> [warp...] [-o output.obj]
```

Warps:
- `twist <degrees>` - Twist around Z axis
- `bend <degrees>` - Bend around Y axis
- `bulge <scale>` - Bulge along Z
- `taper <scale>` - Taper along Z

Options:
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_warp cube.obj twist 90 --o twisted.obj
manifold_warp box.obj taper 0.5 --o tapered.obj
```

---

### manifold_levelset

Implicit SDF modeling.

```bash
manifold_levelset <sdf> [options] [-o output.obj]
```

SDFs:
- `sphere <radius>`
- `box <x> <y> <z>`
- `torus <r1> <r2>`
- `capsule <radius> <height>`
- `gyroid <scale>`
- `schwarz <scale>`

Options:
- `-bounds <x> <y> <z>` - Bounds (default 2 2 2)
- `-edge <len>` - Max edge length (default 0.1)
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_levelset sphere 1 -edge 0.05 --o implicit_sphere.obj
manifold_levelset gyroid 0.5 -edge 0.02 --o gyroid.obj
```

---

### manifold_info

Mesh analysis and information.

```bash
manifold_info <input.obj> [options]
```

Options:
- `-all` - Show all info
- `-decompose` - Decompose into components
- `-simplify <tol>` - Simplify mesh
- `-raycast <x> <y> <z> <x2> <y2> <z2>` - Ray cast
- `-o <file>` - Output file (for decompose/simplify)

Example:
```bash
manifold_info model.obj -all
manifold_info complex.obj -simplify 0.01 --o simplified.obj
manifold_info model.obj -raycast 0 0 0 1 0 0
```

---

### manifold_cross

2D CrossSection operations.

```bash
manifold_cross <op> [args] [options]
```

Operations:
- `circle <r>` - Create circle of radius r
- `rect <w> <h>` - Create rectangle w x h
- `square <size>` - Create square
- `add <cs1.obj> <cs2.obj>` - Union
- `sub <cs1.obj> <cs2.obj>` - Subtract
- `intersect <cs1.obj> <cs2.obj>` - Intersect
- `offset <cs.obj> <d>` - Offset by distance d
- `hull <cs.obj>` - Convex hull

Options:
- `-o <file>` - Output file (SVG)

Example:
```bash
manifold_cross circle 1 -o circle.cs
manifold_cross add a.cs b.cs -o union.cs
```

---

### manifold_properties

Vertex properties manipulation.

```bash
manifold_properties <input.obj> [options] [-o output.obj]
```

Options:
- `-normal` - Calculate smooth normals
- `-curvature` - Calculate curvature (gaussian & mean)
- `-color` - Add vertex colors based on normals
- `-info` - Show properties info
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_properties sphere.obj -normal --o smooth.obj
manifold_properties model.obj -curvature --o with_curvature.obj
```

---

### manifold_meshid

Mesh ID management.

```bash
manifold_meshid <input.obj> [options] [-o output.obj]
```

Options:
- `-id` - Show OriginalID
- `-reserve <n>` - Reserve IDs for n meshes
- `-original` - Mark as original
- `-info` - Show mesh info
- `-o <file>` - Output OBJ file

Example:
```bash
manifold_meshid model.obj -id
manifold_meshid model.obj -original --o original.obj
```

---

## License

These CLI tools are provided separately from the main Manifold library. See LICENSE file for details.