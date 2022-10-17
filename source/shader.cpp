#include"../headers/shader.h"

// Reads a text file and outputs its contents to a string.
std::string getFileContents(const char* filename)
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

// The default constructor is empty as shaders are defined as attributes in World.
Shader::Shader()
{
	std::cout << "Created Shader Program" << std::endl;
}

// The destructor.
Shader::~Shader()
{
	if(loaded){
		glDeleteProgram(ID);
		std::cout << "Deleted Shader Program" << std::endl;
	}
}

// Loads both a vertex shader and a fragment shader and creates a shader program.
void Shader::Load(const char* vertexFile, const char* fragmentFile)
{
	// Store the contents of each file.
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);
	// Convert into char arrays for passing to OpenGL.
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create a vertex shader object.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach the vertex shader data to the shader object.
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the shader.
	glCompileShader(vertexShader);
	// Check for compilation errors.
	checkForCompileErrors(vertexShader, "VERTEX");

	// Create a fragment shader object.
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach the fragment shader data to the shader object.
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the shader.
	glCompileShader(fragmentShader);
	// Check for compilation errors.
	checkForCompileErrors(fragmentShader, "FRAGMENT");

	// Create a shader program.
	ID = glCreateProgram();
	// Record existance of program.
	loaded = true;

	// Attach the shaders to the shader program.
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	// Link the shaders together in the program.
	glLinkProgram(ID);
	// Check for compilation errors.
	checkForCompileErrors(ID, "PROGRAM");    

	// Delete the vertex and fragment shader objects now
	// the shader program has been made.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// Activates the shader program.
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the shader program.
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// Checks a shader/shader program for compile errors.
void Shader::checkForCompileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation.
	GLint hasCompiled;
	// Char array to store error message in.
	char infoLog[1024];
	// If type is vertex shader or fragment shader:
	if (strncmp(type,"PROGRAM",8) != 0)
	{// Check for errors in shader compiling.
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{// Get and output the shader info log.
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << infoLog << std::endl;
		}
	}
	else // If type is program:
	{// Check for errors in shaders linking in program.
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{// Get and output the program info log.
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << infoLog << std::endl;
		}
	}
}