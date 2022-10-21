#version 410 core
uniform sampler2D heightMap;

in vec2 fragTexCoord;
in float height;

out float FragColor;

void main()
{
   FragColor = abs(texture(heightMap, fragTexCoord).r - height) * 255.0;
}
