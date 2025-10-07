#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#ifndef __EMSCRIPTEN__
#include"glad.h"
#else
#include <GLES3/gl3.h>
#endif

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	void Delete();
};

#endif