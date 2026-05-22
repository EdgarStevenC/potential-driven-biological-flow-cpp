# Potential-driven biological flow in a synthetic 3D conduit — v1.2

A compact C++17 reproducibility demo for **geometry- and potential-driven transport** in a synthetic 3D biological conduit-like domain.

This version keeps the numerical/scientific-computing focus of v1.0, but adds a smoother **C++ surface renderer** for the conduit and generates more frames for a more fluid GIF preview.

The aim is not to reproduce a specific physiological system. The demo illustrates a transferable modeling workflow where:

- geometry defines a biological conduit-like domain;
- a pressure/potential field imposes a driving force;
- transported markers move under potential-driven flow with dispersion;
- a lightweight C++ renderer generates rotating 3D preview frames;
- VTK-compatible outputs support scientific visualization.

## Conceptual model

```text
u = -K(x) grad(P)
partial_t c + div(c u) = D laplacian(c) + S
```

where `P` is a pressure/potential field, `K(x)` is an effective geometry-dependent conductance, `u` is the induced transport direction, and `c` is a transported marker.

This is a first-contact C++ demonstration for scientific-computing workflows related to numerical modeling of living systems. It emphasizes clean implementation, geometric abstraction, potential-driven transport, surface rendering, and reproducible visualization.

## Build and run

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./potential_flow_demo ../output
```

Generated files:

```text
output/frames/frame_0000.ppm ...
output/vtk/conduit_surface_v12.vtk
docs/demo_preview_v1_2.gif
```

## Generate GIF

Using the included Python helper:

```bash
python3 scripts/make_gif.py output/frames docs/demo_preview_v1_2.gif
```

Or using ImageMagick:

```bash
magick -delay 4 -loop 0 output/frames/frame_*.ppm docs/demo_preview_v1_2.gif
```

## Versioning note

- **v1.0**: original C++ demo with particle/point-based conduit rendering.
- **v1.2**: smoother C++ surface rendering and more frames for a more continuous rotating preview.
