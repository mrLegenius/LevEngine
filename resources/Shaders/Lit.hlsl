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

cbuffer ModelConstantBuffer : register(b2)
{
	DirLight dirLight;
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
	matrix mvp = mul(model, viewProjection);
	output.pos = mul(pos, mvp);
	output.normal = mul(transpose(model), input.normal);
	output.uv = input.uv;
	float4 fragPos = mul(float4(input.pos, 1.0f), model);
	output.fragPos = fragPos.xyz;

	return output;
}

Texture2D my_texture;
SamplerState my_sampler;

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv);

float4 PSMain(PS_IN input) : SV_Target
{
	// properties
	float3 norm = normalize(input.normal);
	float3 viewDir = normalize(cameraPosition - input.fragPos);

	float3 result = CalcDirLight(dirLight, norm, viewDir, input.uv);

	return float4(result, 1.0f);
}

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv)
{
	float3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5); //material.shininess
	// combine results
	float3 ambient = light.ambient * my_texture.Sample(my_sampler, uv).rgb;
	float3 diffuse = light.diffuse * diff * my_texture.Sample(my_sampler, uv).rgb;
	float3 specular = light.specular * spec * my_texture.Sample(my_sampler, uv).rgb;

	return (ambient + diffuse + specular);
}