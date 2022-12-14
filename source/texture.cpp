#include"../headers/texture.h"

// A texture constructor for a blank texture
Texture::Texture(int width, int height, int channels, GLuint slot)
{
	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
    unit = slot;

    widthImg = width;
    heightImg = height;
    numColCh = channels;
    bytes = NULL;

    Load();
}

Texture::Texture(const char* image, GLuint slot)
{
	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
    unit = slot;

	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);

	// Reads the image from a file and stores it in bytes
	bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    std::cout << widthImg << ", " << heightImg << std::endl;
    std::cout << image << ", " << numColCh << std::endl;

    Load();
    if(bytes){
        // Deletes the image data as it is already in the OpenGL Texture object
        stbi_image_free(bytes);
    }
}

// A texture constructor for a blank texture
void Texture::loadData(unsigned char* data)
{
    bytes = data;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    Load();
}

void Texture::Load(){
	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

    if (numColCh == 4)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 3)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 1)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else
        throw std::invalid_argument("Automatic texture type recognition failed.");

	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture(){
    glDeleteTextures(1, &ID);
    std::cout << "Deleted Texture" << std::endl;
}

void Texture::texUnit(std::shared_ptr<Shader>& shader, const char* uniform)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader->ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader->use();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

int Texture::getHeight(){
    return heightImg;
}
int Texture::getWidth(){
    return widthImg;
}
int Texture::getNumColCh(){
    return numColCh;
}