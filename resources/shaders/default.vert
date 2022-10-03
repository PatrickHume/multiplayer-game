#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 norm;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 world;
uniform mat4 local;
uniform mat4 model;

void main()
{
    // need to add world to this too
    crntPos = vec3(world * local * model * vec4(aPos, 1.0));
    norm = aNormal;
    color = aColor;
    texCoord = mat2(1.0, 0.0, 0.0, -1.0) * aTex;

    gl_Position = camMatrix * vec4(crntPos, 1.0);
}