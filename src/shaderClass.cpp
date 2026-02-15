#include"shaderClass.h"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = get_file_contents(vertexFile);
	fragmentCode = get_file_contents(fragmentFile);

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create Vertex Shader Object and get its reference
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	
	//we will need this shader later
	//glDeleteShader(fragmentShader);

}

void Shader::RecompileShader(std::string fragment_shader_string)
{
	const char* fragment_shader_source = fragment_shader_string.c_str();
	
	glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragmentShader);
	CheckShader(fragmentShader);
	glLinkProgram(ID);
	BindBufferBases();
}

void Shader::BindBufferBases()
{
	objects->Bind();
	objects->BindBase(ID, 0, "UBO");
	materials->Bind();
	materials->BindBase(ID, 1, "materialUBO");
}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::CheckShader(GLuint shader)
{
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); // Check compile status

	if (compiled == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length); // Get the length of the info log

		// Allocate buffer for the log
		GLchar* infoLog = new GLchar[length]; 

		// Retrieve the log
		glGetShaderInfoLog(shader, length, NULL, infoLog); 

		printf("ERROR: Shader compilation failed!\n");
		printf("%s\n", infoLog); // Print the error messages

		// Clean up
		delete[] infoLog;
		glDeleteShader(shader); // Delete the shader, as it's useless

		// Handle error as needed (e.g., exit application, throw exception)
	}
}
