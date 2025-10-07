#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#ifndef __EMSCRIPTEN__
#include"glad.h"
#else
#include <GLES3/gl3.h>
#endif

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif