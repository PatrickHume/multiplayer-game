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

//uniform mat4 worldRotation;
//uniform mat4 localRotation;
//uniform mat4 modelRotation;

void main()
{
    mat4 transformMatrix = world * local * model;

    crntPos = vec3(transformMatrix * vec4(aPos, 1.0));
    
    mat3 normalMatrix = mat3(transformMatrix);
    normalMatrix = inverse(normalMatrix);
    normalMatrix = transpose(normalMatrix);
    norm = normalMatrix * aNormal;

    color = aColor;
    texCoord = mat2(1.0, 0.0, 0.0, -1.0) * aTex;

    gl_Position = camMatrix * vec4(crntPos, 1.0);
}