// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#version 410 core

uniform sampler2D heightMap;
in vec2 fragTexCoord;
out vec4 FragColor;

void main()
{
	FragColor = texture(heightMap, fragTexCoord);
}
