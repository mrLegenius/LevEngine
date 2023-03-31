#define MAX_POINT_LIGHTS 10
#define CASCADE_COUNT 4
#define DEBUG_CASCADES 0

cbuffer CameraConstantBuffer : register(b0)
{
	row_major matrix cameraView;
	row_major matrix viewProjection;
	float3 cameraPosition;
};

cbuffer ModelConstantBuffer : register(b1)
{
	row_major matrix model;
};

cbuffer lightSpaceConstantBuffer : register(b3)
{
	row_major matrix lightViewProjection[CASCADE_COUNT];
	float4 distances;
	float shadowMapDimensions;
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
	float pad_;
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
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;
};

float GetCascadeIndex(float depth)
{
	float absDepth = abs(depth);
	for (float i = 0; i < CASCADE_COUNT; i++)
	{
		if (absDepth < distances[i])
			return i;
	}

	return CASCADE_COUNT - 1;
}

PS_IN VSMain(VS_IN input)
{
	PS_IN output;

	float4 pos = float4(input.pos, 1.0f);
	float4 fragPos = mul(pos, model);

	output.pos = mul(fragPos, viewProjection);
	output.normal = mul(transpose(model), input.normal);
	output.uv = input.uv;
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;
	
	return output;
}

Texture2D my_texture : register(t0);
SamplerState my_sampler : register(s0);

Texture2DArray shadowMapTexture : register(t1);
SamplerComparisonState shadowMapSampler : register(s1);

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv, float4 fragPosLightSpace, float depth);
float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float2 uv);
float CalcShadow(float4 lpos, float3 normal, float3 lightDir, float depth);

float4 PSMain(PS_IN input) : SV_Target
{
	float cascade = GetCascadeIndex(input.depth);

	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	// properties
	float3 norm = normalize(input.normal);
	float3 viewDir = normalize(cameraPosition - input.fragPos);

	float3 result = CalcDirLight(dirLight, norm, viewDir, input.uv, fragPosLightSpace, input.depth);

	for (int i = 0; i < pointLightsCount; i++)
		result += CalcPointLight(pointLights[i], norm, input.fragPos, viewDir, input.uv);

	return float4(result, 1.0f);
}

float CalcShadow(float4 lpos, float3 normal, float3 lightDir, float depth)
{
	float cascadeIndex = GetCascadeIndex(depth);

	//re-homogenize position after interpolation
	lpos.xyz /= lpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (lpos.x < -1.0f || lpos.x > 1.0f ||
		lpos.y < -1.0f || lpos.y > 1.0f ||
		lpos.z < 0.0f || lpos.z > 1.0f) return 0.0f;

	//transform clip space coords to texture space coords (-1:1 to 0:1)
	lpos.x = lpos.x * 0.5f + 0.5f;
	lpos.y = lpos.y * -0.5f + 0.5f;

	float ndotl = dot(normal, lightDir);

	//float shadowMapBias = max(0.005 * (1.0 - ndotl), 0.0005);
	float shadowMapBias = clamp(0.0005 / acos(saturate(ndotl)), 0, 0.005);
	//float shadowMapBias = max(0.005 * tan(acos(ndotl)), 0.0005);
	lpos.z -= shadowMapBias;

	float texelSize = 1.0f / shadowMapDimensions;

	//No filtering
	//float shadowFactor = shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.xy, cascadeIndex), lpos.z);

	//PCF filtering on a 4 x 4 texel neighborhood
	
	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.x + x * texelSize, lpos.y + y * texelSize, cascadeIndex), lpos.z);
		}
	}
	float shadowFactor = sum / 16.0;
	

	//if clip space z value greater than shadow map value then pixel is in shadow
	if (shadowFactor < lpos.z) return 0.0f;

	//otherwise calculate ilumination at fragment
	return ndotl * shadowFactor;
}

float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 uv, float4 fragPosLightSpace, float depth)
{
	float3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 5); //TODO: material.shininess

	float cascadeIndex = GetCascadeIndex(depth);
	float3 tex = my_texture.Sample(my_sampler, uv).rgb;
#if DEBUG_CASCADES 
	if (cascadeIndex == 0)
	{
		tex.r = 1;
		tex.g = 0;
		tex.b = 0;
	}
	else if (cascadeIndex == 1)
	{
		tex.r = 0;
		tex.g = 1;
		tex.b = 0;
	}
	else if (cascadeIndex == 2)
	{
		tex.r = 0;
		tex.g = 0;
		tex.b = 1;
	}
	else if (cascadeIndex == 3)
	{
		tex.r = 1;
		tex.g = 1;
		tex.b = 0;
	}
#endif
	// combine results
	float3 ambient = light.ambient * tex;
	float3 diffuse = light.diffuse * diff * my_texture.Sample(my_sampler, uv).rgb;
	float3 specular = light.specular * spec * my_texture.Sample(my_sampler, uv).rgb;

	float shadow = CalcShadow(fragPosLightSpace, normal, lightDir, depth);

	return (ambient + (shadow) * (diffuse + specular));
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