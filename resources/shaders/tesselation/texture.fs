// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#version 410 core

uniform sampler2D heightMap;
in vec2 fragTexCoord;
in vec4 vertex;
out vec4 FragColor;

uniform vec3 camPos;

float getFogFactor(float d)
{
    const float FogMax = 2400.0;
    const float FogMin = 800.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main(void)
{
	const vec4 FogColor = vec4(0.0, 0.0, 0.0, 0.0);
    float d = distance(camPos, vertex.xyz);
    float alpha = getFogFactor(d);

    FragColor = mix(texture(heightMap, fragTexCoord)+vec4(0.2,0.2,0.2,0.1), FogColor, alpha);
}
