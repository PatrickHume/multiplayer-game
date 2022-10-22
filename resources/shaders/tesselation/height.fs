#version 410 core

uniform sampler2D heightMap;

in vec2 fragTexCoord;

in float HeightScaled;

layout (location = 0) out float FragColor;

void main()
{
   FragColor = abs((texture(heightMap, fragTexCoord).r*255.0) - HeightScaled);
}