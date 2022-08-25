#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec4 ClipSpace;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_Tiling;

void main()
{
	FragPos = vec3(u_Model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(u_Model))) * normal;
	TexCoords = texCoords * u_Tiling;

	ClipSpace = u_Projection * u_View * vec4(FragPos, 1.0);
	gl_Position = ClipSpace;
};

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

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

in vec4 ClipSpace;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

layout(location = 0) out vec4 color;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform DirLight u_DirLight;
uniform SpotLight u_SpotLights[SPOT_LIGHTS_NUM];
uniform PointLight u_PointLights[POINT_LIGHTS_NUM];

uniform float u_WaveStrength;
uniform float u_MoveFactor;
uniform float u_Transparency;
uniform float u_Reflectivity;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D depthMap;
uniform sampler2D normalMap;

uniform int u_UseReflectionAndRefraction;
uniform int u_UseNormalMap;
uniform int u_UseDepthTesting;

uniform int u_PointLightsCount;
uniform int u_SpotLightsCount;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}
float map01(float value, float min1, float max1) {
	return map(value, min1, max1, 0.0, 1.0);
}
float clamp01(float value) {
	return clamp(value, 0, 1);
}
void main()
{
	
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(u_ViewPos - FragPos);

	vec2 ndc = (ClipSpace.xy / ClipSpace.w) / 2.0 + 0.5f;

	vec2 reflectionTextureCoords = vec2(ndc.x, -ndc.y);
	vec2 refractionTextureCoords = vec2(ndc.x, ndc.y);

	float near = 0.1;
	float far = 1000.0;
	float depth = texture(depthMap, refractionTextureCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	float waterDepth = floorDistance - waterDistance;

	vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoords.x + u_MoveFactor, TexCoords.y)).rg * 0.1;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + u_MoveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * u_WaveStrength;

	if(u_UseDepthTesting)
		totalDistortion *= clamp(waterDepth / 20.0f, 0.0, 1.0);

	reflectionTextureCoords += totalDistortion;
	reflectionTextureCoords.x = clamp(reflectionTextureCoords.x, 0.001, 0.999);
	reflectionTextureCoords.y = clamp(reflectionTextureCoords.y, -0.999, -0.001);
	vec4 reflectionColor = texture(reflectionTexture, reflectionTextureCoords);

	refractionTextureCoords += totalDistortion;
	refractionTextureCoords = clamp(refractionTextureCoords, 0.001, 0.999);
	vec4 refractionColor = texture(refractionTexture, refractionTextureCoords);

	vec4 normalMapColor = texture(normalMap, distortedTexCoords);

	if (u_UseNormalMap)
		norm = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0));

	float refractionFactor = dot(viewDir, norm) * 0.5 + 0.5;
	refractionFactor = pow(refractionFactor, u_Reflectivity);
	
	// phase 1: Directional lighting
	vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

	// phase 2: Point lights
	for (int i = 0; i < u_PointLightsCount; i++)
		result += CalcPointLight(u_PointLights[i], norm, FragPos, viewDir);
	// phase 3: Spot lights
	for (int i = 0; i < u_SpotLightsCount; i++)
		result += CalcSpotLight(u_SpotLights[i], norm, FragPos, viewDir);

	if(u_UseDepthTesting)
	{ 
		result *= clamp(waterDepth / 5.0, 0.0, 1.0);
		//refractionColor = mix(refractionColor, vec4(u_Material.ambient, 1.0), clamp01(map(waterDepth / u_Transparency, -1, 1, 0.0, 1.0)));
	}

	if(u_UseReflectionAndRefraction)
		color = mix(reflectionColor, refractionColor, refractionFactor);

	color += vec4(result, 0.0);

	if (u_UseDepthTesting)
		color.a = clamp(waterDepth / u_Transparency, 0.0, 1.0);
	else
		color.a = clamp(1.0 / u_Transparency, 0.0, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

	//blinn
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess * 2);

	// combine results
	vec3 ambient = light.ambient * u_Material.ambient;
	vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
	vec3 specular = light.specular * spec * u_Material.specular;

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
	vec3 ambient = light.ambient * u_Material.ambient;
	vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
	vec3 specular = light.specular * spec * u_Material.specular;

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
	vec3 ambient = light.ambient * u_Material.ambient;
	vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
	vec3 specular = light.specular * spec * u_Material.specular;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}