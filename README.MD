# Stanford Ray Tracer
This is an extension of the ray tracer designed by 
Professor Ron Fedkiw at Stanford University. This ray tracer
is capable of emulating point light, area light, directional 
light, and spot light. It is intended to be used with Blender 2.8+.

## Installation
1. Clone this repository on a Linux machine.
2. Run `sh setup.sh`

## Usage
### Blender
Create a scene in Blender 2.8+ by placing objects at the locations you want, in the orientations you want, with the textures you want. Place lights and test out the scene by rendering it, but you will install lights separately in the ray tracer. When the scene looks good in Blender, Export the scene into a Wavefront (.obj + .mtl) file. Materials must be "Principled BSDF".

### Ray Tracer
Ray Tracer automatically imports models and textures respectively from the .obj file and the .mtl file. You need to place the camera and the lights in `src/raytracer.cpp`.

#### FOV Conversion:
Blender uses horizontal FOV while the ray tracer uses vertical FOV. Convert using https://themetalmuncher.github.io/fov-calc/.

#### Position Conversion:
If the location of an object is expressed as (X, Y, Z) in Blender, then the corresponding location in the ray tracer is (X, Z, -Y). If the rotation of an object is expressed as (X, Y, Z) in Blender, then the corresponding rotation in the ray tracer is (X - 90, Z, -Y).