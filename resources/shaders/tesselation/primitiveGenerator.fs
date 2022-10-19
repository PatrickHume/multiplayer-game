// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
// fragment shader
#version 410 core

layout (vertices=4) out;

// varying input from vertex shader
in vec2 TexCoord[];
// varying output to evaluation shader
out vec2 TextureCoord[];

in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[gl_MaxPatchVertices];

gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
