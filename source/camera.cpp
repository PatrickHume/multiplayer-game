#include"../headers/camera.h"

Camera::Camera()
{
}
// Sets the position of the camera.
void Camera::setPosition(glm::vec3 position){
    Camera::position = position;
}
// Sends the camera's transform matrix to the shader's "cameraMatrix" uniform.
void Camera::sendMatrix(Shader& shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}
// Sends the camera's position to the shader's "cameraMatrix" uniform.
void Camera::sendPosition(Shader& shader, const char* uniform)
{
    glUniform3f(glGetUniformLocation(shader.ID, uniform), position.x, position.y, position.z);
}
// Updates the camera matrix.
void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    // The matrix representing the orientation of the camera.
    glm::mat4 view = glm::mat4(1.0f);
    // The matrix representing the perspective of the camera.
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::lookAt(position, position + orientation, up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)(Screen::windowAspect), nearPlane, farPlane);
    
    cameraMatrix = projection * view;
}
// Processes GLFW window inputs for movement and orientation controls.
void Camera::Inputs(GLFWwindow* window)
{
    // If the screen is clicked, focus the camera.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouseHeld){
        mouseHeld = true;
        // Toggle the focus.
        focus = !focus;
        if(!focus){
            // Hide the mouse cursor.
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }else{
            // Show the mouse cursor.
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        glfwSetCursorPos(window, (Screen::windowWidth / 2), (Screen::windowHeight / 2));
    }
    // Record left mouse button released.
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mouseHeld){
        mouseHeld = false;
    }
    // If locked, prevent movement controls being processed.
    if (locked){
        return;
    }

    // Keypress movement controls:
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {// Move forwards.
        position += speed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {// Move left.
        position += speed * -glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {// Move backwards.
        position += speed * -orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {// Move right.
        position += speed * glm::normalize(glm::cross(orientation, up));
    }
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{// Move upwards.
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{// Move downwards.
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{// Set speed high.
		speed = highSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{// Set speed low.
		speed = regularSpeed;
	}

    // Mouse lookaround controls:
    if (focus && !mouseHeld){
        // Store the position of the mouse onscreen.
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        // The vertical distance between the mouse and the center of the screen. (To rotate the camera around X axis.)
        float rotX = sensitivity * (float)(mouseY - (Screen::windowHeight / 2)) / Screen::windowHeight;
        // The horizontal distance between the mouse and the center of the screen. (To rotate the camera around the Y axis.)
        float rotY = sensitivity * (float)(mouseX - (Screen::windowWidth / 2)) / Screen::windowWidth;

        // Looking up/down:
        // Get the resultant direction vector by rotating the camera about the X axis.
        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));
        // If the resultant angle is facing too high...
        if(glm::angle(orientation, newOrientation) > glm::angle(orientation, up)-glm::radians(minLookUpAngle) && rotX < 0)
        {// Adjust the new orientation to face as high as allowed by minLookUpAngle.
            newOrientation = glm::rotate(orientation, glm::angle(orientation, up)-glm::radians(minLookUpAngle), glm::normalize(glm::cross(orientation, up)));  
        }
        // If the resultant angle is facing too low...
        else if (glm::angle(orientation, newOrientation) > glm::radians(180-minLookUpAngle)-glm::angle(orientation, up) && rotX > 0)
        {// Adjust the new orientation to face as high as low by minLookUpAngle.
            newOrientation = glm::rotate(orientation, -(glm::radians(180-minLookUpAngle)-glm::angle(orientation, up)), glm::normalize(glm::cross(orientation, up)));
        }
        // Update the cameras orientation.
        orientation = newOrientation;

        // Looking left/right:
        // Get the resultant direction vector by rotating the camera about the Y axis.
        orientation = normalize(glm::rotate(orientation, glm::radians(-rotY), up));

        // Reset the cursor position to the middle of the screen. 
        // (To prevent it leaving the window over time.)
        glfwSetCursorPos(window, (Screen::windowWidth / 2), (Screen::windowHeight / 2));
    }
}
// Returns the position vector "dist" units infront of the camera.
glm::vec3 Camera::getPositionInFront(float dist){
    return position + orientation*dist;
}
// Returns the direction vector representing the orientation of the camera.
glm::vec3 Camera::getOrientation(){
    return orientation;
}
