#version 330 core
out vec4 FragColor;

uniform vec4 lightColor;
uniform vec3 lightPos;

in vec3 normal;
in vec3 crntPos;

void main()
{
   FragColor = lightColor;
}