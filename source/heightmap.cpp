#include"../headers/heightmap.h"

// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
Heightmap::Heightmap(const char *filename)
{
    minTessLevels.reserve(rez);

    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;

    // ---------------------------------- Loading Heightmap ----------------------------------
    tesselationShader = std::make_shared<Shader>(
        "resources/shaders/tesselation/passthrough.vs", 
        "resources/shaders/tesselation/texture.fs", 
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

    // ---------------------------------- Generating Buffers ----------------------------------
    // Create the framebuffers.
    glGenRenderbuffers( 1, &renderbufId );
    glGenRenderbuffers( 1, &depthbufId );
    glGenFramebuffers ( 1, &framebufId   );
    // Set the width and height to be that of the image, use only one color channel.
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height);
    // Do the same for the depth buffer - Despite it not being used, I have not been able to
    // successfully compile the framebuffer without having attached a depth buffer.
    glBindRenderbuffer(GL_RENDERBUFFER, depthbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    // Set up the framebuffer to hold both renderbuffers.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, renderbufId);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, depthbufId);

    // Check frame buffer for errors.
    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
    if(stat != GL_FRAMEBUFFER_COMPLETE) { 
        throw std::runtime_error("Heightmap framebuffer is not framebuffer complete.");
        }
    // Set up color attachment to draw buffer 0.
    const GLenum att[] = {GL_COLOR_ATTACHMENT2};
    glDrawBuffers(1, att);

    // Create shader that will write to the framebuffer.
    setupShader = std::make_shared<Shader>(
        "resources/shaders/tesselation/passthrough.vs", 
        "resources/shaders/tesselation/height.fs", 
        nullptr,
        "resources/shaders/tesselation/controlSetup.tcs", 
        "resources/shaders/tesselation/evaluationSetup.tes"); 

    // Resize the viewport to fit the image dimensions.
    glViewport(0,0,width,height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Activate the shader.
    setupShader->use();
    // Set the projection matrix to be orthogonal, this will make the resultant image flat.
    glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -2.0f, 2.0f);
    setupShader->setMat4("projection",ortho);
    // Bind the heightmap texture.
    texture->Bind();
    texture->texUnit(setupShader, "heightMap");
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);
    texture->Unbind();
    // Wait for the draw to finish before reading the framebuffer.
    glFinish();

    // Create a buffer to store frame buffer pixel values.
    GLfloat* floatBuffer = new GLfloat[width * height];
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    // Set the data to be 1 byte aligned.
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    // Read the red value of each pixel, as a float, into floatBuffer.
    glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, floatBuffer);

    // Convert the floats into bytes for writing to png.
    unsigned char* charBuffer = new unsigned char[width * height * 4];
    for(int i = 0; i < width * height; i++){
        // Flip the data upside down to match the original image orientation.
        charBuffer[i] = (unsigned char)(floatBuffer[(width * height) - 1 - i]);
    }
    stbi_write_png("image.png", width, height, 1, charBuffer, width);

    //exit(0);

    //delete[] buffer;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    Screen::resizeViewport();

    //exit(0);
    
}
Heightmap::~Heightmap()
{
}
void Heightmap::Draw(Camera& camera)
{
    //glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 ortho = glm::ortho((float)(-width/2), (float)(width/2), (float)(-height/2), (float)(height/2), 0.0f, 10000.0f);

    texture->Bind();
    texture->texUnit(tesselationShader, "heightMap");

    //tessLevelsTexture->Bind();
    //tessLevelsTexture->texUnit(tesselationShader, "tessLevels");

    glm::mat4 model = glm::mat4(1.0f);
    tesselationShader->use();
    tesselationShader->setMat4("projection",camera.getProjection());
    tesselationShader->setMat4("view",      camera.getView());
    tesselationShader->setMat4("model",     model);
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);

    texture->Unbind();
    //tessLevelsTexture->Unbind();
}
