#version 410 core
in float HeightScaled;
layout (location = 0) out float FragColor;

void main()
{
   FragColor = HeightScaled;
}