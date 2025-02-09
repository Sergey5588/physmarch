#include<iostream>
#include"glad.h"
#include<GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtx/matrix_transform_2d.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#include<string>
#define Up glm::vec3(0,-1,0)



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


void HandleInput(GLFWwindow* window, glm::vec3& Orientation, glm::vec3& Position, int height, int width) {
	
	float speed = 10.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += Orientation * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= Orientation * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= glm::normalize(glm::cross(Orientation, Up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += glm::normalize(glm::cross(Orientation, Up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		Position -= Up * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		Position += Up * speed;
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
	//std::cout << std::to_string(Orientation.x) +" "+ std::to_string(Orientation.y) +" "+ std::to_string(Orientation.z) << std::endl;

	


}



int main()
{
	int WIDTH = 800;
	int HEIGHT = 800;
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
	glUniform2f(glGetUniformLocation(shaderProgram.ID, "Resolution"), 800.0f, 800.0f);

	
	
	// Main while loop

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window))
	{
		// Spec
		// ify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Orientation"), Orientation.x, Orientation.y, Orientation.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "Cam_pos"), Position.x, Position.y, Position.z);

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		HandleInput(window, Orientation, Position, HEIGHT, WIDTH);
		// Take care of all GLFW events
		glfwPollEvents();
		
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}