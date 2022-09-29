#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

in vec3 norm;
in vec3 crntPos;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
   vec3 normal = normalize(norm);
   vec3 lightDirection = normalize(lightPos - crntPos);

   float diffuse = max(dot(normal,lightDirection), 0);
   float ambient = 0.2f;
   float specularLight = 0.5f;

   vec3 viewDirection = normalize(camPos - crntPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection),0.0f), 16.0f);
   float specular = specAmount * specularLight;

   FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient) + texture(tex1, texCoord).r * lightColor * specular;
}