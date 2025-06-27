Raymarching game engine made by me and my friend [@MAX-TS](https://github.com/MAX-TS) (he helped with materials and lighting)

Also, check website for our project: [physmarch.com](https://physmarch.com)! (MAX-TS make this, and I make main project)

> [!NOTE]
> We used AI to help checking code for logic erros, some code explanation and help with tools, but all the code was written by humans!

## Gallery
![Demo1](https://github.com/Sergey5588/physmarch/blob/master/demos/demo1.gif)
[Second demo (youtube)](https://youtu.be/ahUss3JYl2I)

## Compile from source

> [!WARNING]
> Only linux X11 (Xwayland also works) is supported! (wayland session as well as other operating systems are not tested. I'd be happy if you became a tester :wink: .)

First, install following dependencies with your package manager:

```opengl cmake glfw```

then run this command:

```
git clone https://github.com/Sergey5588/physmarch &&
cd physmarch &&
mkdir build &&
cd build &&
cmake .. &&
make
```
## Features
- Precise 3D fractal rendering, such as Mandelbulb
- Precise ray reflections (no RTX required!)
- No polygons, pure maths!
##### Supported primitives
- Plane
- Sphere
- Box
- Torus
- Triangular prism

> Taken from: [marklundin/glsl-sdf-primitives](https://github.com/marklundin/glsl-sdf-primitives)
##### Supported operations
- :cry:
## Default controls
- WASD - move
- Q and E - up and down respectively
- Mouse wheel - change speed of the camera
- Esc - lock / unlock cursor
## To do
- [x] basic raymarching
- [ ] operations between shapes
- [ ] texturing
- [x] shading
- [ ] game engine API
- [ ] proper scene system
- [ ] shape editor
- [ ] physics engine

## License

[MIT](https://github.com/Sergey5588/physmarch/blob/master/LICENSE)
