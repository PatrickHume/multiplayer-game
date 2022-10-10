#include"../headers/EBO.h"

// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO()
{
	glGenBuffers(1, &ID);
}
void EBO::setIndices(std::vector<GLuint>& indices)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

// Binds the EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}