Raymarching game engine made by me and my friend [@MAX-TS](https://github.com/MAX-TS)

Also, check website for our project: [physmarch.com](https://physmarch.com)!


## Gallery
![Demo1](https://github.com/Sergey5588/physmarch/blob/master/demos/demo1.gif)

## Compile from source
> :warning: Note: Only linux  X11 is supported! (wayland session as well as other operating systems are not tested. I'd be happy if you became a tester :wink: .)

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
##### Supported operations
- :cry:
## Default controls
- WASD - move
- Q and E - up and down respectively
## To do
- [x] basic raymarching
- [ ] operations between shapes
- [ ] texturing
- [ ] shading
- [ ] game engine API
- [ ] proper scene system
- [ ] shape editor
- [ ] physics engine

## License

[MIT](https://github.com/Sergey5588/physmarch/blob/master/LICENSE)
