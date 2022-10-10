#include"../headers/mesh.h"

Mesh::Mesh(
    std::vector <Vertex>& vertices, 
    std::vector <GLuint>& indices,  
    struct Material& material,
    glm::mat4 matrix)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::material = material;

    Mesh::matrix = matrix;

    glm::decompose(Mesh::matrix, Mesh::scale, Mesh::quaternion, Mesh::translation, Mesh::skew, Mesh::perspective);
    Mesh::rotation=glm::toMat4(glm::normalize(glm::conjugate(Mesh::quaternion)));

    // Generates Vertex Array Object and binds it
	VAO.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
    EBO.Bind();
	EBO.setIndices(indices);

	// Links VBO to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float)));
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*) (6 * sizeof(float)));
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*) (9 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::setInstanceMatrices(std::vector<glm::mat4>& matrices){
    instanceVBO.Bind();
    instanceVBO.UpdateInstanceMatrices(matrices);
    VAO.Bind();
	VAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*) 0);
    VAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*) (1 * sizeof(glm::vec4)));
    VAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*) (2 * sizeof(glm::vec4)));
    VAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*) (3 * sizeof(glm::vec4)));    
    glVertexAttribDivisor(4,1);
    glVertexAttribDivisor(5,1);
    glVertexAttribDivisor(6,1);
    glVertexAttribDivisor(7,1);
    VAO.Unbind();
    instanceVBO.Unbind();
}

void Mesh::drawSimple(
    Shader& shader, 
    Camera& camera)
{
    shader.Activate();
    VAO.Bind();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::drawInstanced(
    Shader& shader, 
    Camera& camera,
    std::vector<Texture>& textures,
    unsigned int numInstances)
{
    shader.Activate();
    VAO.Bind();

    textures[material.baseColorTexture].Bind();
    textures[material.metallicRoughnessTexture].Bind();

    textures[material.baseColorTexture].texUnit(shader, "diffuseTex");
    textures[material.metallicRoughnessTexture].texUnit(shader, "specularTex");

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    glUniform1i(glGetUniformLocation(shader.ID, "doubleSided"),     material.doubleSided);
    glUniform4fv(glGetUniformLocation(shader.ID, "baseColorFactor"), 1, glm::value_ptr(material.baseColorFactor));
    glUniform1f(glGetUniformLocation(shader.ID, "metallicFactor"),  material.metallicFactor);
    glUniform1f(glGetUniformLocation(shader.ID, "roughnessFactor"), material.roughnessFactor);

    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, numInstances);
}

void Mesh::Draw(
    Shader& shader, 
    Camera& camera,
    std::vector<Texture>& textures)
{
    shader.Activate();
    VAO.Bind();

    textures[material.baseColorTexture].Bind();
    textures[material.metallicRoughnessTexture].Bind();

    textures[material.baseColorTexture].texUnit(shader, "diffuseTex");
    textures[material.metallicRoughnessTexture].texUnit(shader, "specularTex");

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
    // all the material attributes must be sent to the shader
    //glUniform1i(glGetUniformLocation(shader.ID, "hasBaseColorTex"), material.hasBaseColorTex);
    glUniform1i(glGetUniformLocation(shader.ID, "doubleSided"),     material.doubleSided);
    glUniform4fv(glGetUniformLocation(shader.ID, "baseColorFactor"), 1, glm::value_ptr(material.baseColorFactor));
    glUniform1f(glGetUniformLocation(shader.ID, "metallicFactor"),  material.metallicFactor);
    glUniform1f(glGetUniformLocation(shader.ID, "roughnessFactor"), material.roughnessFactor);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::drawOutline(
    Shader& outlineShader, 
    Camera& camera,
    std::vector<Texture>& textures
){
    outlineShader.Activate();
    VAO.Bind();
    // this could be in next class up...
    glUniform1f(glGetUniformLocation(outlineShader.ID, "outlineThickness"), 0.05f);
    
    glUniformMatrix4fv(glGetUniformLocation(outlineShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
    glUniformMatrix4fv(glGetUniformLocation(outlineShader.ID, "modelRotation"), 1, GL_FALSE, glm::value_ptr(rotation));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
