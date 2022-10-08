#version 330 core
flat in int id;
out ivec4 FragColor;

void main()
{
   FragColor = ivec4(id,255,255,1);
}