#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in mat4 instanceMatrix;

out vec3 crntPos;
out vec3 norm;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
    mat4 transformMatrix = instanceMatrix * model;

    crntPos = vec3(transformMatrix * vec4(aPos, 1.0));
    
    mat3 normalMatrix = mat3(transformMatrix);
    normalMatrix = inverse(normalMatrix);
    normalMatrix = transpose(normalMatrix);
    norm = normalMatrix * aNormal;

    color = aColor;
    texCoord = mat2(1.0, 0.0, 0.0, -1.0) * aTex;

    gl_Position = camMatrix * vec4(crntPos, 1.0);
}