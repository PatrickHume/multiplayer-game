#include"../headers/heightmap.h"

// Based heavily on https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
Heightmap::Heightmap(const char *file)
{
    fileStr        = std::string(file);
    fileDirectory  = fileStr.substr(0,fileStr.find_last_of('/')+1);

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

    tessLevelsTexture = std::make_shared<Texture>(rez, rez, 1, tessLevelsTexUnit);
    heightmapTexture = std::make_shared<Texture>(file, heightmapTexUnit);
    width = heightmapTexture->getWidth();
    height = heightmapTexture->getHeight();

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

    glGenRenderbuffers( 1, &renderbufId );
    glGenRenderbuffers( 1, &depthbufId );
    glGenFramebuffers ( 1, &framebufId   );

    glBindRenderbuffer(GL_RENDERBUFFER, renderbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, depthbufId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    // Set up the framebuffer to hold the renderbuffers.
    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, renderbufId);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, depthbufId  );

    // Check frame buffer for errors.
    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);  
    if(stat != GL_FRAMEBUFFER_COMPLETE) { 
        throw std::runtime_error("Heightmap framebuffer is not framebuffer complete.");
    }
    // Set up color attachment to draw buffer 1.
    const GLenum att[] = {GL_COLOR_ATTACHMENT2};
    glDrawBuffers(1, att);
    // Bind the frame buffer back to 0 for regular drawing to the screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create shader that will write to the framebuffer.
    setupShader = std::make_shared<Shader>(
        "resources/shaders/tesselation/passthrough.vs", 
        "resources/shaders/tesselation/height.fs", 
        nullptr,
        "resources/shaders/tesselation/controlSetup.tcs", 
        "resources/shaders/tesselation/evaluationSetup.tes"); 

    tessLevels = loadTessLevelImage();
    tessLevelsTexture->loadData(&tessLevels[0]);
}

std::array<unsigned char, Heightmap::rez*Heightmap::rez> Heightmap::loadTessLevelImage(){
    std::array<float, rez*rez>  tessLevelError;
    std::array<bool, rez*rez>   patchIsLocked;
    std::array<unsigned char, rez*rez> tessLevelBuffer;
    std::array<unsigned char, rez*rez> errorData;
    std::array<unsigned char, rez*rez> patchCharBuffer;

    std::string fileStrNoExt    = fileStr.substr(0,fileStr.find_last_of('.'));
    std::string outputFile      = fileStrNoExt+"TessLevels.png";

    int widthImg;
    int heightImg;
    int numColCh;

    std::cout << "Searching for file: " << outputFile << std::endl;

    stbi_set_flip_vertically_on_load(false);
    unsigned char* bytes = stbi_load(outputFile.c_str(), &widthImg, &heightImg, &numColCh, 0);
    if(bytes){
        std::cout << "File found." << std::endl;
        if(numColCh != 1){
            std::cout << "Number of channels in heightmap tessLevels image is not equal to 1." << std::endl;
        }
        else if(widthImg != rez){
            std::cout << "Height of heightmap tessLevels image is not equal to rez." << std::endl;
        }
        else if(heightImg != rez){
            std::cout << "Width of heightmap tessLevels image is not equal to rez." << std::endl;
        }else{
            for(int i = 0; i < tessLevelBuffer.size(); i++){
                tessLevelBuffer[i] = (unsigned char)bytes[i];
            }            
            // Deletes the image data as it is already in the OpenGL Texture object
            stbi_image_free(bytes);
            return tessLevelBuffer;
        }
    }
    std::cout << "Creating tessLevels image." << std::endl;

    glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -2.0f, 2.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufId);
    glViewport(0,0,width,height);
    glDisable(GL_DEPTH_TEST);

    setupShader->use();
    setupShader->setMat4("projection",ortho);

    heightmapTexture->Bind();
    heightmapTexture->texUnit(setupShader, "heightMap");

    glBindVertexArray(terrainVAO);

    floatBuffer.resize(width*height);
    screenCharBuffer.resize(width*height);

    tessLevelBuffer.fill((unsigned char)1);
    patchIsLocked.fill(false);

    for(int currentTessLevel = 1; currentTessLevel <= 32; currentTessLevel++){
        std::cout << "Evaluating Tessellation Level " << currentTessLevel << std::endl;

        tessLevelsTexture->loadData(&tessLevelBuffer[0]);

        tessLevelsTexture->Bind();
        tessLevelsTexture->texUnit(setupShader, "tessLevels");

        glFinish();

        glClearColor(1.0,1.0,1.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_PATCHES, 0, 4*rez*rez);
        heightmapTexture->Unbind();
        glFinish();

        glReadBuffer(GL_COLOR_ATTACHMENT2);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, &floatBuffer[0]);

        tessLevelError.fill(0.0);

        for(int i = 0; i < width * height; i++){
            int imageY = i/width;
            int imageX = i-(imageY*width);

            int tessY = (imageY*rez) / height;
            int tessX = (imageX*rez) / width;

            int pos = (tessY*rez) + tessX;
        
            tessLevelError[pos] += floatBuffer[i];    
        }

        float highestError = 255.0f * (int)(width/rez)*(int)(height/rez);

        for(int i = 0; i < rez*rez; i++){
            float error = std::fmin((tessLevelError[i]/highestError)*255.0,255.0);
            errorData[i] = (unsigned char)error;

            const float tolerance = 4.0;
            if(!patchIsLocked[i]){
                if(error > tolerance){
                    tessLevelBuffer[i] = (unsigned char)(currentTessLevel + 1);
                }else{
                    tessLevelBuffer[i] = (unsigned char)(currentTessLevel);
                    patchIsLocked[i] = true;
                }
            }
        }

        /*
        std::string file;
        for(int i = 0; i < patchCharBuffer.size(); i++){
            patchCharBuffer[i] = errorData[patchCharBuffer.size() -i -1];
        }
        file = "images/patchErrors/pe"+std::to_string(currentTessLevel)+".png";
        stbi_write_png(file.c_str(), rez, rez, 1, &patchCharBuffer[0], rez);

        for(int i = 0; i < floatBuffer.size(); i++){
            screenCharBuffer[i] = (unsigned char)(floatBuffer[floatBuffer.size() - 1 - i]);
        }
        file = "images/pixelErrors/px"+std::to_string(currentTessLevel)+".png";
        stbi_write_png(file.c_str(), width, height, 1, &screenCharBuffer[0], width);

        patchCharBuffer.fill((unsigned char)0);
        for(int i = 0; i < patchCharBuffer.size(); i++){
            if(patchIsLocked[patchCharBuffer.size() -1 -i])
                patchCharBuffer[i] = (unsigned char)255;
        }
        file = "images/isLocked/lk"+std::to_string(currentTessLevel)+".png";
        stbi_write_png(file.c_str(), rez, rez, 1, &patchCharBuffer[0], rez);

        for(int i = 0; i < patchCharBuffer.size(); i++){
            patchCharBuffer[i] = tessLevelBuffer[patchCharBuffer.size() -i -1];
        }
        file = "images/tessLevels/tl"+std::to_string(currentTessLevel)+".png";
        stbi_write_png(file.c_str(), rez, rez, 1, &patchCharBuffer[0], rez);
        */
    }

    heightmapTexture->Unbind();
    tessLevelsTexture->Unbind();

    stbi_write_png(outputFile.c_str(), rez, rez, 1, &tessLevelBuffer[0], rez);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    Screen::resizeViewport();

    return tessLevelBuffer;
}

Heightmap::~Heightmap()
{
}
void Heightmap::Draw(Camera& camera)
{
    //glm::mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 ortho = glm::ortho((float)(-width/2), (float)(width/2), (float)(-height/2), (float)(height/2), 0.0f, 10000.0f);
    camera.sendPosition(tesselationShader, "camPos");

    heightmapTexture->Bind();
    heightmapTexture->texUnit(tesselationShader, "heightMap");

    tessLevelsTexture->Bind();
    tessLevelsTexture->texUnit(tesselationShader, "tessLevels");

    glm::mat4 model = glm::mat4(1.0f);
    tesselationShader->use();
    tesselationShader->setMat4("projection",camera.getProjection());
    tesselationShader->setMat4("view",      camera.getView());
    tesselationShader->setMat4("model",     model);
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, 4*rez*rez);

    heightmapTexture->Unbind();
    tessLevelsTexture->Unbind();
}
