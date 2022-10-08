#version 330 core
layout (location = 0) in vec3 aPos;

flat out int id;

uniform int objectID;
uniform mat4 camMatrix;
uniform mat4 world;
uniform mat4 local;
uniform mat4 model;

void main()
{
   vec3 crntPos = vec3(world * local * model * vec4(aPos, 1.0));
   id = objectID;
   gl_Position = camMatrix * vec4(crntPos, 1.0);
}