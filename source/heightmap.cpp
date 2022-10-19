#include"../headers/heightmap.h"
// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
Heightmap::Heightmap(const char *filename)
{
    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;

    tesselationShader = std::make_shared<Shader>(
        "resources/shaders/tesselation/passthrough.vs", 
        "resources/shaders/tesselation/height.fs", 
        nullptr,
        "resources/shaders/tesselation/controlShader.tcs", 
        "resources/shaders/tesselation/evaluationShader.tes"); 

    texture = std::make_shared<Texture>(filename, 0);
    width = texture->getWidth();
    height = texture->getHeight();

    for(unsigned int i = 0; i <= rez-1; i++){
        for(unsigned int j = 0; j <= rez-1; j++){
            vertices.push_back(-width/2.0f + width*i/(float)rez);       // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez);     // v.z
            vertices.push_back(i / (float)rez);                         // u
            vertices.push_back(j / (float)rez);                         // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);   // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*j/(float)rez);     // v.z
            vertices.push_back((i+1) / (float)rez);                     // u
            vertices.push_back(j / (float)rez);                         // v

            vertices.push_back(-width/2.0f + width*i/(float)rez);       // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back(i / (float)rez);                         // u
            vertices.push_back((j+1) / (float)rez);                     // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)rez);   // v.x
            vertices.push_back(0.0f);                                   // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)rez); // v.z
            vertices.push_back((i+1) / (float)rez);                     // u
            vertices.push_back((j+1) / (float)rez);                     // v
        }
    }

    // register VAO
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}
Heightmap::~Heightmap()
{
}
void Heightmap::Draw(Camera& camera)
{
    texture->Bind();
    texture->texUnit(tesselationShader, "heightMap");
    glm::mat4 model = glm::mat4(1.0f);
    tesselationShader->use();
    tesselationShader->setMat4("projection",camera.getProjection());
    tesselationShader->setMat4("view",      camera.getView());
    tesselationShader->setMat4("model",     model);
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);
    texture->Unbind();
}
