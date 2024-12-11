# Simple Physically Based Renderer using Ray Tracing

This is a simple physically based renderer that uses ray tracing algorithms to render 3D scenes with realistic lighting and shading. The renderer supports basic ray tracing features such as ray-sphere intersection, ray-plane intersection, and ray-triangle intersection, as well as more advanced features such as Monte Carlo path tracing, importance sampling, and physically based shading models.

## Dependencies

- CMake

## Compiling

For Windows:

```
mkdir build
cd build
cmake ..
```

For Linux / MacOS:

```
mkdir build
cd build
cmake ..
make -j8
```

## Running

The path to scene config (required to be a JSON file) and the path of the output image are passed using command line arguments as follows:
```bash
./build/render <scene_path> <out_path> <num_samples> <sampling_strategy>
```

Sample scene configurations can be found in the `scenes` directory, with scenes having different lighting configurations to test the renderer. Custom scenes can also be created using the provided simple renderer exporter for Blender. Instructions for using the Blender exporter can be found in the `scenes` directory.

The number of samples can be any positive integer - the higher the number, the less noisy the image but the longer it takes to render. A good starting point is 100 samples.

The sampling strategy can be one of the following:
- 'uniform': Uniform Hemisphere Sampling (faster but more noisy images, ideal for basic ray distribution / debugging)
- 'cosine': Cosine Weighted Sampling (slightly slower but more accurate lighting, ideal for basic lighting and diffuse surfaces)
- 'importance': Importance Light Sampling (slowest but most accurate, ideal for complex lighting)
