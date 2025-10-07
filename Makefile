ALL: emscripten

emscripten:
	emcc src/main.cpp src/EBO.cpp src/VAO.cpp src/VBO.cpp src/shaderClass.cpp src/UBO.cpp src/imgui/imgui_demo.cpp src/imgui/imgui_draw.cpp src/imgui/imgui_impl_glfw.cpp src/imgui/imgui_impl_opengl3.cpp src/imgui/imgui_stdlib.cpp src/imgui/imgui_tables.cpp src/imgui/imgui_widgets.cpp src/imgui/imgui.cpp -sUSE_GLFW=3 -sUSE_WEBGL2=1 -I D:/ProjectsD/Physmarch/physmarch/include/ -I D:/ProjectsD/Physmarch/physmarch/include/imgui/ --preload-file resources/main.vert --preload-file resources/main.frag -s WASM=1 -std=c++1z -o a.html