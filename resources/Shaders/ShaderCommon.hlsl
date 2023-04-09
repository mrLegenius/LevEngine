#define MAX_POINT_LIGHTS 100
#define CASCADE_COUNT 4
#define DEBUG_CASCADES 0

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

struct DirLight
{
	float3 direction;
	float3 color;
};

struct PointLight
{
	float4 positionViewSpace;
	float3 position;
	float3 color;

	float range;
	float smoothness;
	float intensity;
};

struct Material
{
	float3 emissive;
	// -- 16
	float3 ambient;
	// -- 16
	float3 diffuse;
	// -- 16
	float3 specular;
	// -- 16
	float shininess;
	bool hasEmissiveTexture;
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	// -- 16
	bool hasSpecularTexture;
	bool hasNormalTexture;
	// -- 8
};

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

cbuffer LightningConstantBuffer : register(b2)
{
	DirLight dirLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	float3 globalAmbient;
	int pointLightsCount;
};

cbuffer lightSpaceConstantBuffer : register(b3)
{
	row_major matrix lightViewProjection[CASCADE_COUNT];
	float4 distances;
	float shadowMapDimensions;
};

cbuffer MaterialConstantBuffer : register(b4)
{
	Material material;
};

cbuffer ScreenToViewParams : register(b5)
{
	float4x4 CameraInverseProjection;
	float2 ScreenDimensions;
}

Texture2DArray shadowMapTexture : register(t9);
SamplerComparisonState shadowMapSampler : register(s9);


float4 ClipToView(float4 clip)
{
	// View space position
	float4 view = mul(CameraInverseProjection, clip);
	// Perspective projection
	view = view / view.w;

	return view;
}

float4 ScreenToView(float4 screen)
{
	// Convert to normalized texture coordinates
	float2 texCoord = screen.xy / ScreenDimensions;

	// Convert to clip space
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip);
}

struct LightingResult
{
	float3 diffuse;
	float3 specular;
};

float3 CalcDiffuse(float3 color, float3 lightDir, float3 normal)
{
	float NdotL = max(0.0, dot(normal, lightDir));
	return color * NdotL;
}

float3 CalcSpecular(float3 color, float3 viewDir, float3 lightDir, float3 normal, float shininess)
{
	//Blinn-Phong
	float3 halfAngle = normalize(lightDir + viewDir);
	float spec = dot(normal, halfAngle);

	//Phong
	//float3 reflectDir = reflect(-lightDir, normal);
	//float spec = dot(viewDir, reflectDir);

	return color * pow(max(spec, 0.0), shininess);
}

float CalcAttenuation(float range, float smoothness, float distance)
{
	return 1.0f - smoothstep(range * smoothness, range, distance);
}

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

float CalcShadow(float4 lpos, float3 normal, float3 lightDir, float cascade)
{
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
	//float shadowFactor = shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.xy, cascade), lpos.z);

	//PCF filtering on a 4 x 4 texel neighborhood
	float sum = 0;
	for (float y = -1.5; y <= 1.5; y += 1.0)
	{
		for (float x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += shadowMapTexture.SampleCmpLevelZero(shadowMapSampler, float3(lpos.x + x * texelSize, lpos.y + y * texelSize, cascade), lpos.z);
		}
	}
	float shadowFactor = sum / 16.0;


	//if clip space z value greater than shadow map value then pixel is in shadow
	if (shadowFactor < lpos.z) return 0.0f;

	//otherwise calculate ilumination at fragment
	return ndotl * shadowFactor;
}


LightingResult CalcDirLight(DirLight light, float3 normal, float3 viewDir, float4 fragPosLightSpace, float cascade, float shininess)
{
	LightingResult result;

	float3 lightDir = normalize(-light.direction);

	float shadow = CalcShadow(fragPosLightSpace, normal, lightDir, cascade);
	result.diffuse = CalcDiffuse(light.color, lightDir, normal) * shadow;
	result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * shadow;

	return result;
}

LightingResult CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir, float shininess)
{
	LightingResult result;

	float3 lightDir = light.positionViewSpace - fragPos;
	float distance = length(lightDir);
	lightDir = lightDir / distance;

	float attenuation = CalcAttenuation(light.range, light.smoothness, distance);

	result.diffuse = CalcDiffuse(light.color, lightDir, normal) * attenuation * light.intensity;
	result.specular = CalcSpecular(light.color, viewDir, lightDir, normal, shininess) * attenuation * light.intensity;

	return result;
}