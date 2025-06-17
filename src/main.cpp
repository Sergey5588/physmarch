#include<iostream>
#include"glad.h"
#include<GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtc/type_ptr.hpp>


#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>



#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#include<string>
#define Up glm::vec3(0,-1,0)

//extern bool MOUSE_LOCK;
bool MOUSE_LOCK = true;
float SPEED = 0.01f;
int WIDTH = 800;
int HEIGHT = 800;

int ITERATIONS = 100;
// Vertices coordinates
GLfloat vertices[] =
{
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f

};

// Indices for vertices order
GLuint indices[] =
{
	0,1,2,
	2,3,0
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        MOUSE_LOCK = !MOUSE_LOCK;
    }
}

void HandleInput(GLFWwindow* window, glm::vec3& Orientation, glm::vec3& Position, int height, int width) {

	
	if (MOUSE_LOCK) {

		
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
		
		
			
		
		

		double mouseX = 0;
		double mouseY = 0;
		double sensitivity = 100.0f;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		
		float rotX = (float)(mouseX - (width / 2)) / width * sensitivity;
		float rotY = (float)(mouseY - (height / 2)) / height * sensitivity;

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), glm::normalize(glm::cross(Orientation, -Up)));
		Orientation = glm::rotate(Orientation, glm::radians(-rotX), Up);
		
		glfwSetCursorPos(window, width/2, height/2);
	}
	//std::cout << std::to_string(Orientation.x) +" "+ std::to_string(Orientation.y) +" "+ std::to_string(Orientation.z) << std::endl;

	


}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(MOUSE_LOCK) {

		SPEED *= 1.0 + yoffset * 0.05;
	}
}

int main()
{
	
	glm::vec3 Orientation = glm::vec3(0,0,1);
	glm::vec3 Position = glm::vec3(0,0,0);
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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ray_marching", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WIDTH, HEIGHT);



	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("./resources/main.vert", "./resources/main.frag");


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
	shaderProgram.Activate();
	
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
	
	
	
	// Main while loop

	
	glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);

	glfwSetScrollCallback(window, scroll_callback);
	
	while (!glfwWindowShouldClose(window))
	{	
		if(MOUSE_LOCK)
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::ShowDemoWindow();

		
		ImGui::Begin("Control panel");
		ImGui::SliderInt("Iterations", &ITERATIONS, 0, 1000);
		ImGui::End();

		
		
		
		// Spec
		// ify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		glUniform2f(glGetUniformLocation(shaderProgram.ID, "Resolution"), (float)WIDTH, (float)HEIGHT);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Orientation"), Orientation.x, Orientation.y, Orientation.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Cam_pos"), Position.x, Position.y, Position.z);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "Iterations"), ITERATIONS);
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		HandleInput(window, Orientation, Position, HEIGHT, WIDTH);
		// Take care of all GLFW events

		


		
		
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
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
