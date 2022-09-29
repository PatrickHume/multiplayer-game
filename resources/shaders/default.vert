#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

out vec3 color;

out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;

out vec3 norm;
out vec3 crntPos;

void main()
{
    crntPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = camMatrix * vec4(crntPos, 1.0);
    color = aColor;
    texCoord = aTex;
    norm = aNormal;
}