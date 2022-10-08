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
// Stores camera position and orientation - also handles camera input controls
class Camera
{
public:
    Camera();
    // Is toggled when the screen is clicked on.
    bool focus = false;
    // Set to true when the mouse is held down, should be false at
    // the instant of a new mouse click.
    bool mouseHeld = false;
    // A bool used to prevent the mouse from moving the camera during focus
    // e.g. when interacting with the terminal.
    bool locked = false;
    void setPosition(glm::vec3 position);
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void sendMatrix(Shader& shader, const char* uniform);
    void sendPosition(Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window);
    glm::vec3 getPositionInFront(float dist = 5);
private:
    // The position of the camera.
	glm::vec3 position;
    // The direction vector representing the camera's orientation.
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    // A vector pointing up, the cross product of this and the orientation
    // gives an axis for looking up and down.
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    // The camera matrix, sent to the vertex shaders.
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    // Controls the speed at which the camera travels through space.
    float speed = 0.01f;
    // The speed at which the camera moves when left shift is held
    const float highSpeed = 0.4f;
    // The regular speed of the camera
    const float regularSpeed = 0.1f;
    // The sensitivity of the camera orientation controls.
    float sensitivity = 100.0f;
    // Set the maximum/minimum angle the camera can look. 
    // (Prevents the camera from frontflips.)
    const float minLookUpAngle = 5.0f;
};

#endif