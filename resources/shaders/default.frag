#version 330 core
out vec4 FragColor;

in vec3 color;

in vec2 texCoord;

uniform sampler2D tex0;
uniform vec4 lightColor;

void main()
{
   FragColor = texture(tex0, texCoord) * lightColor;
}