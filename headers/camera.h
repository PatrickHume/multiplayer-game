#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include"shader.h"
#include"screen.h"

enum class DrawType {
    REGULAR,
    OUTLINE,
    PREPARE_OUTLINE,
    OBJECT_ID
};

class Camera
{
public:
	glm::vec3 position;
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    bool focus = false;
    bool mousePressed = false;

    bool locked = false;

    float speed = 0.01f;
    float sensitivity = 100.0f;

    Camera();

    void setPosition(glm::vec3 position);

    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Matrix(Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window);
    glm::vec3 getPositionInFront(float dist = 5);
private:
};

#endif