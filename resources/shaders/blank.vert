#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 transform;
uniform mat4 model;

void main()
{
   mat4 transformMatrix = transform * model;
   vec3 crntPos = vec3(transformMatrix * vec4(aPos, 1.0));
   gl_Position = camMatrix * vec4(crntPos, 1.0);
}