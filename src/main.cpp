#include<iostream>
#ifndef __EMSCRIPTEN__
#include"glad.h"
#else
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>
#include<imgui/imgui_stdlib.h>


#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"UBO.h"
#include<string>
#define Up glm::vec3(0,-1,0)

#include"object.h"
#include"scene.h"
#include"networking.h"
#include"imgui_renderer.h"
#include <algorithm> //std::count
#include <functional>


std::vector<Object> scene = {
	Object{T_SPHERE, O_BASE, glm::vec3(0,3,0), glm::vec4(0), 0},
	Object{T_BOX, O_BASE, glm::vec3(0,1,0), glm::vec4(1), 0},
	Object{T_PRISM, O_BASE, glm::vec3(1, 0, 0), glm::vec4(1, 1, 0, 0), 2},
    Object{T_TORUS, O_BASE, glm::vec3(0, 0, 1), glm::vec4(0.5, 1, 0, 0), 2},
    Object{T_PLANE, O_BASE, glm::vec3(0, -1, 0), glm::vec4(0, 1, 0, 0), 2},
    Object{T_BULB, O_BASE, glm::vec3(0), glm::vec4(0), 1}
};
std::vector<Material> materials = {
    Material(glm::vec4(1,0,1,1), 1.0),
    Material(glm::vec4(0,1,0,1), 0.5),
    Material(glm::vec4(0,0.7,0,1), 1.0)
};
//int lengths[T__LENGTH] = {1, 1, 1, 1, 1, 1};

int lengths[max_object_type_count] = {};
std::vector<std::string> labels = {
	"Sphere",
	"Box",
	"Prism",
	"Torus",
	"Plane",
	"Bulb"
};
std::vector<std::string> material_names = {
	"Ping",
	"Reflective green",
	"Grass"
};
glm::vec2 last_mouse_pos;
//extern bool MOUSE_LOCK;
bool MOUSE_LOCK = true;
float SPEED = 0.01f;
int WIDTH = 800;
int HEIGHT = 800;

int ITERATIONS = 64;
int SHADOW_RAYS = 32;
// Vertices coordinates

GLFWwindow* window;
#ifndef __EMSCRIPTEN__
GLfloat vertices[] =
{
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f

};
#else
GLfloat vertices[] =
{
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};
#endif

// Indices for vertices order
GLuint indices[] =
{
	0,1,2,
	2,3,0
};

std::function<void()> loop;
void main_loop() { loop(); }


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        MOUSE_LOCK = !MOUSE_LOCK;
    }
}

void HandleInput(GLFWwindow* window, glm::vec3& Orientation, glm::vec3& Position, int height, int width)
{
	ImGuiIO& io = ImGui::GetIO();
	#ifndef __EMSCRIPTEN__
	bool condition = MOUSE_LOCK;
	#else
	bool condition = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	if (!condition)
	{
		double mouseX = 0;
		double mouseY = 0;

		glfwGetCursorPos(window, &mouseX, &mouseY);
		last_mouse_pos = glm::vec2(mouseX, mouseY);
	}
	#endif
	#ifndef __EMSCRIPTEN__
	if (condition) {
	#endif

		if (!io.WantCaptureKeyboard)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				Position += Orientation * SPEED;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				Position -= Orientation * SPEED;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				Position -= glm::normalize(glm::cross(Orientation, Up)) * SPEED;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				Position += glm::normalize(glm::cross(Orientation, Up)) * SPEED;
			}
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
				Position -= Up * SPEED;
			}
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
				Position += Up * SPEED;
			}
		}
	#ifdef __EMSCRIPTEN__
	if (condition) {
	#endif
		
			
		
		if (io.WantCaptureMouse)
			return;

		double mouseX = 0;
		double mouseY = 0;
		double sensitivity = 100.0f;

		glfwGetCursorPos(window, &mouseX, &mouseY);

		
		#ifndef __EMSCRIPTEN__
		float rotX = (float)(mouseX - (width / 2)) / width * sensitivity;
		float rotY = (float)(mouseY - (height / 2)) / height * sensitivity;

		glfwSetCursorPos(window, width/2, height/2);
		#else
		float rotX = (float)(mouseX - last_mouse_pos.x) / width * sensitivity;
		float rotY = (float)(mouseY - last_mouse_pos.y) / height * sensitivity;
		last_mouse_pos = glm::vec2(mouseX, mouseY);
		#endif
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), glm::normalize(glm::cross(Orientation, -Up)));
		Orientation = glm::rotate(Orientation, glm::radians(-rotX), Up);
	}
	//std::cout << std::to_string(Orientation.x) +" "+ std::to_string(Orientation.y) +" "+ std::to_string(Orientation.z) << std::endl;

	


}
#ifndef __EMSCRIPTEN__
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
	printf("callback called %d %d\n", width, height);
}
#else
void UpdateWindowSize()
{
	double cssWidth, cssHeight;
	emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
	// emscripten_get_element_size("#canvas");
	glViewport(0, 0, cssWidth, cssHeight);

	glfwSetWindowSize(window, cssWidth, cssHeight);
	emscripten_set_canvas_element_size("#canvas", cssWidth, cssHeight);
	//emscripten_set_canvas_size(uiEvent->windowInnerWidth, uiEvent->windowInnerHeight);
	//ImGuiIO& io = ImGui::GetIO();
  
	WIDTH = cssWidth;
	HEIGHT = cssHeight;
}
EM_BOOL emscWindowSizeChanged(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    
	UpdateWindowSize();
	return EM_TRUE;
	//  pp7v3ppd
}
#endif
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(MOUSE_LOCK) {

		SPEED *= 1.0 + yoffset * 0.05;
	} else {
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	}
}

int main()
{
	
	const char* ObjAsStr[] = {
		"T_SPHERE",
		"T_BOX",
		"T_PRISM",
		"T_TORUS",
		"T_PLANE",
		"T_BULB"
	};
	glm::vec3 Orientation = glm::vec3(0,0,1);
	glm::vec3 Position = glm::vec3(0,1,-3);
	// Initialize GLFW
	glfwInit();
	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	window = glfwCreateWindow(WIDTH, HEIGHT, "Ray_marching", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	//Load GLAD so it configures OpenGL
	#ifndef __EMSCRIPTEN__
	gladLoadGL();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WIDTH, HEIGHT);
	#else
	UpdateWindowSize();
	//glfwSetWindowSize(window, cssHeight, cssWidth);
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, emscWindowSizeChanged);
	#endif
	
	



	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("./resources/main.vert", "./resources/main.frag");
	
	UBO UBO1(object_size,128);
	shaderProgram.objects = &UBO1;
	// UBO1.Bind();
	// UBO1.BindBase(shaderProgram.ID, 0, "UBO");

	UBO materialUBO(material_size, 128);
	shaderProgram.materials = &materialUBO;
	// materialUBO.Bind();
	// materialUBO.BindBase(shaderProgram.ID, 1, "materialUBO");
	shaderProgram.BindBufferBases();
	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();
	
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO to VAO
	VAO1.LinkVBO(VBO1, 0);
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	UBO1.UnBind();
	materialUBO.UnBind();
	shaderProgram.Activate();
	
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    

    ImGui_ImplOpenGL3_Init();
	
	
	
	// Main while loop
	
	
	glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
	glfwSetKeyCallback(window, key_callback);
	
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	glfwSetScrollCallback(window, scroll_callback);

	int selected = -1;
	int selected_obj_type = 0;
	std::string new_name = "null";

	NetworkData network_data;

	Scene render_scene{};
	render_scene.objects = scene;
	render_scene.materials = materials;
	render_scene.labels = labels;
	render_scene.material_names = material_names;
	render_scene.network_data = &network_data;
	render_scene.LoadFromLink();
	render_scene.update_ln();
	render_scene.SetupLoadListener();
	render_scene.custom_objects = {};

	ImguiRenderer ui_renderer(render_scene, network_data, &shaderProgram, Position, ITERATIONS, SHADOW_RAYS);
	
    // Setup Platform/Renderer backends
	loop = [&]
	{
		#ifndef __EMSCRIPTEN__
			if(MOUSE_LOCK)
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		#endif
		glfwPollEvents();
		
		ui_renderer.imgui_start_frame();
		ui_renderer.render_top_bar();
		ui_renderer.imgui_render_control_panel();
		ui_renderer.imgui_render_scene_editor();
		ui_renderer.RenderTextEditor();
		
		

		glm::vec3 poss[render_scene.objects.size()];
		glm::vec4 args[render_scene.objects.size()];
		for (int i = 0; i < render_scene.objects.size(); ++i) {
			poss[i] = render_scene.objects[i].pos;
			args[i] = render_scene.objects[i].args;
		}
		// Spec
		// ify the color of the background
		// glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClearColor(1,0,0,1);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		glUniform2f(glGetUniformLocation(shaderProgram.ID, "Resolution"), (float)WIDTH, (float)HEIGHT);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Orientation"), Orientation.x, Orientation.y, Orientation.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Cam_pos"), Position.x, Position.y, Position.z);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "Iterations"), ITERATIONS);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "Shadow_rays"), SHADOW_RAYS);
		glUniform3fv(glGetUniformLocation(shaderProgram.ID, "Positions"),render_scene.objects.size() , glm::value_ptr(poss[0]));
		glUniform4fv(glGetUniformLocation(shaderProgram.ID, "Arguments"), render_scene.objects.size(), glm::value_ptr(args[0]));
		glUniform1iv(glGetUniformLocation(shaderProgram.ID, "lengths"), max_object_type_count, &render_scene.lengths[0]);
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Bind the UBO so OpenGL knows to use it
		UBO1.Bind();
		UBO1.WriteData(render_scene.objects, object_size);

		materialUBO.Bind();
		materialUBO.WriteData(render_scene.materials, material_size);
		// Draw primitives, number of indices, datatype of indices, index of indices
#ifndef __EMSCRIPTEN__
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
#else
		glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		
		HandleInput(window, Orientation, Position, HEIGHT, WIDTH);
		// Take care of all GLFW events
	};
	
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
	emscripten_set_main_loop_timing(EM_TIMING_RAF, 60);
#else
	glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window))
        main_loop();
#endif






	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	UBO1.Delete();
	materialUBO.Delete();
	shaderProgram.Delete();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();


	
	return 0;
}
