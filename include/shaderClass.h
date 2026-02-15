#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#ifndef __EMSCRIPTEN__
#include"glad.h"
#else
#include <GLES3/gl3.h>
#endif
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include"UBO.h"
std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	GLuint fragmentShader;
	GLuint vertexShader;
	std::string fragmentCode;
	UBO* materials;
	UBO* objects;
	
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	void RecompileShader(std::string fragment_shader);

	void BindBufferBases();
	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	void CheckShader(GLuint shader);
};
#endif