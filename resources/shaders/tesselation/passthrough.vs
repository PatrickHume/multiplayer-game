// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
// vertex shader
#version 410 core
// vertex position
layout (location = 0) in vec3 aPos;
// texture coordinate
layout (location = 1) in vec2 aTex;
// send the tex coordinates to the tcs
out vec2 TexCoord;
void main()
{
    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = vec4(aPos, 1.0);
    // pass texture coordinate though
    TexCoord = aTex;
}
