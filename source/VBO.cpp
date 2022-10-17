#include"../headers/VBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO()
{
	glGenBuffers(1, &ID);
}
VBO::~VBO(){
	glDeleteBuffers(1, &ID);
	std::cout << "Deleted VBO" << std::endl;
}
void VBO::addVertices(std::vector<Vertex>& vertices)
{
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
void VBO::UpdateInstanceMatrices(std::vector<glm::mat4>& matrices)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), matrices.data(), GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
