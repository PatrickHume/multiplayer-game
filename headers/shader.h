#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// Reads a text file and outputs its contents to a string.
std::string getFileContents(const char* filename);

// Shader loads shader programs from input files.
// The shaders are activated before rendering.
class Shader
{
public:
	Shader();
	~Shader();
	// Loads both a vertex shader and a fragment shader and creates a shader program.
	void Load(const char* vertexFile, const char* fragmentFile);
	// Activates the shader program.
	void Activate();
	// Deletes the shader program.
	void Delete();
	// Reference ID for the shader program
	GLuint ID;
private:
	// Checks a shader/shader program for compile errors.
	void checkForCompileErrors(unsigned int shader, const char* type);
	// Records whether load has been successfully called.
	bool loaded = false;
};
#endif