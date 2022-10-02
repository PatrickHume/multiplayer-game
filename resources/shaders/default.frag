#version 330 core
out vec4 FragColor;

in vec3 crntPos;
in vec3 norm;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

//uniform bool   hasBaseColorTex;

uniform bool   doubleSided;
uniform vec4   baseColorFactor;
uniform float  metallicFactor;
uniform float  roughnessFactor;
uniform int    baseColorTexture;
uniform int    metallicRoughnessTexture;

/*
vec4 pointLight()
{
   vec3 lightVec = lightPos - crntPos;
   float dist = length(lightVec);
   float a = 0.05f;
   float b = 0.04f; 
   float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

   vec3 normal = normalize(norm);
   vec3 lightDirection = normalize(lightVec);

   float diffuse = max(dot(normal,lightDirection), 0);
   float ambient = 0.2f;
   float specularLight = 0.5f;

   vec3 viewDirection = normalize(camPos - crntPos);
   vec3 reflectionDirection = reflect(-lightDirection, normal);
   float specAmount = pow(max(dot(viewDirection, reflectionDirection),0.0f), 16.0f);
   float specular = specAmount * specularLight;

   return (texture(diffuse, texCoord) * (diffuse * inten + ambient) + texture(specular, texCoord).r * specular * inten) * lightColor;
}
*/
/*
vec4 direcLight()
{
}
*/
/*
vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(norm);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse, texCoord) * (diffuse * inten + ambient) + texture(specular, texCoord).r * specular * inten) * lightColor;
}*/

void main()
{

   //doubleSided;
   //baseColorFactor;
   //metallicFactor;
   //roughnessFactor;

   // ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(norm);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

   //if (1)
   //{
      FragColor = (texture(diffuseTex, texCoord) * (diffuse + ambient) + texture(specularTex, texCoord).r * specular) * lightColor;
   //}
   //else
   //{
   //   FragColor = (baseColorFactor * (diffuse + ambient) + texture(specularTex, texCoord).r * specular) * lightColor
   //}
}