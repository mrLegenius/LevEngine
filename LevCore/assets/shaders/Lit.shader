#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec4 u_Plane;
uniform float u_Tiling;

void main()
{
	gl_ClipDistance[0] = dot(vec4(position, 1.0), u_Plane);
	FragPos = vec3(u_Model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(u_Model))) * normal;
	TexCoords = texCoords * u_Tiling;

	gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
};

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	int useDiffuseMap;
	sampler2D diffuseMap;
	int useSpecularMap;
	sampler2D specularMap;
	int useEmissionMap;
	sampler2D emissionMap;
	int u_UseNormalMap;
	sampler2D normalMap;

	float shininess;
};
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

layout(location = 0) out vec4 color;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform SpotLight u_SpotLights[SPOT_LIGHTS_NUM];
uniform PointLight u_PointLights[POINT_LIGHTS_NUM];

uniform int u_PointLightsCount;
uniform int u_SpotLightsCount;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(u_Material.u_UseNormalMap * vec3(texture(u_Material.normalMap, TexCoords)) - (u_Material.u_UseNormalMap - 1) * Normal);
	//vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(u_ViewPos - FragPos);

	// phase 1: Directional lighting
	vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

	// phase 2: Point lights
	for (int i = 0; i < u_PointLightsCount; i++)
		result += CalcPointLight(u_PointLights[i], norm, FragPos, viewDir);
	// phase 3: Spot lights
	for (int i = 0; i < u_SpotLightsCount; i++)
		result += CalcSpotLight(u_SpotLights[i], norm, FragPos, viewDir);

	result += u_Material.useEmissionMap * vec3(texture(u_Material.emissionMap, TexCoords));

	color = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	// combine results
	vec3 ambient = light.ambient * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.ambient);
	vec3 diffuse = light.diffuse * diff * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.diffuse);
	vec3 specular = light.specular * spec * (u_Material.useSpecularMap * vec3(texture(u_Material.specularMap, TexCoords)) - (u_Material.useSpecularMap - 1) * u_Material.specular);

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
		light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.ambient);
	vec3 diffuse = light.diffuse * diff * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.diffuse);
	vec3 specular = light.specular * spec * (u_Material.useSpecularMap * vec3(texture(u_Material.specularMap, TexCoords)) - (u_Material.useSpecularMap - 1) * u_Material.specular);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.ambient);
	vec3 diffuse = light.diffuse * diff * (u_Material.useDiffuseMap * vec3(texture(u_Material.diffuseMap, TexCoords)) - (u_Material.useDiffuseMap - 1) * u_Material.diffuse);
	vec3 specular = light.specular * spec * (u_Material.useSpecularMap * vec3(texture(u_Material.specularMap, TexCoords)) - (u_Material.useSpecularMap - 1) * u_Material.specular);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}