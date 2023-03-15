#define MAX_POINT_LIGHTS 10

cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix viewProjection;
	float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(b1)
{
	row_major matrix model;
};

struct DirLight
{
	float3 direction;
	float3 ambient;
	float3 diffuse;
	float3 specular;
};

struct PointLight 
{
	float3 position;

	float constant;
	float linea;
	float quadratic;
	float _;

	float3 ambient;
	float3 diffuse;
	float3 specular;
};

cbuffer LightningConstantBuffer : register(b2)
{
	DirLight dirLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	int pad_;
	int pointLightsCount;
};

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 fragPos : POSITION;
};

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0f);
	float4 fragPos = mul(pos, model);
	output.pos = mul(fragPos, viewProjection);
	output.normal = mul(transpose(model), input.normal);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;

	return output;
}

Texture2D my_texture;
SamplerState my_sampler;

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv);
float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float2 uv);

float4 PSMain(PS_IN input) : SV_Target
{
	// properties
	float3 norm = normalize(input.normal);
	float3 viewDir = normalize(cameraPosition - input.fragPos);

	float3 result = CalcDirLight(dirLight, norm, viewDir, input.uv);

	for (int i = 0; i < pointLightsCount; i++)
		result += CalcPointLight(pointLights[i], norm, input.fragPos, viewDir, input.uv);

	return float4(result, 1.0f);
}

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv)
{
	float3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5); //TODO: material.shininess
	// combine results
	float3 ambient = light.ambient * my_texture.Sample(my_sampler, uv).rgb;
	float3 diffuse = light.diffuse * diff * my_texture.Sample(my_sampler, uv).rgb;
	float3 specular = light.specular * spec * my_texture.Sample(my_sampler, uv).rgb;

	return (ambient + diffuse + specular);
}

float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float2 uv)
{
	float3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1); //TODO: material.shininess
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linea * distance +
		light.quadratic * (distance * distance));
	// combine results
	float3 ambient = light.ambient * my_texture.Sample(my_sampler, uv).rgb;
	float3 diffuse = light.diffuse * diff * my_texture.Sample(my_sampler, uv).rgb;
	float3 specular = light.specular * spec* my_texture.Sample(my_sampler, uv).rgb;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}