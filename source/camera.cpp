#include"../headers/camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Camera::position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(position, position + orientation, up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)(width/height), nearPlane, farPlane);

    cameraMatrix = projection * view;
}

void Camera::Inputs(GLFWwindow* window)
{

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePressed){
        mousePressed = true;
        if(focus){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        
        focus = !focus;
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mousePressed){
        mousePressed = false;
    }
    
    if (locked){
        return;
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += speed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position += speed * -glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position += speed * -orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += speed * glm::normalize(glm::cross(orientation, up));
    }

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}

    if (focus && !mousePressed){
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

        float minLookUpAngle = 5.0f;

        // facing too high
        if(glm::angle(orientation, newOrientation) > glm::angle(orientation, up)-glm::radians(minLookUpAngle) && rotX < 0){
            orientation = glm::rotate(orientation, glm::angle(orientation, up)-glm::radians(minLookUpAngle), glm::normalize(glm::cross(orientation, up)));
            
        }
        // facing too low
        else if (glm::angle(orientation, newOrientation) > glm::radians(180-minLookUpAngle)-glm::angle(orientation, up) && rotX > 0){
            orientation = glm::rotate(orientation, -(glm::radians(180-minLookUpAngle)-glm::angle(orientation, up)), glm::normalize(glm::cross(orientation, up)));
        }
        else{
            orientation = newOrientation;
        }
        //std::cout << " " << glm::degrees(glm::angle(newOrientation, up)) << std::endl;

        orientation = glm::rotate(orientation, glm::radians(-rotY), up);

        glfwSetCursorPos(window, (width / 2), (height / 2));
    }

}
