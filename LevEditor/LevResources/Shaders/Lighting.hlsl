#ifndef LEV_LIGHTING_HLSL
#define LEV_LIGHTING_HLSL

#include "Registers.hlsli"

#define MAX_LIGHTS 100

// Matches RenderSettings::MaxDirectionalLights. A planet can have more than one sun; only the
// first of them casts shadows, since there is a single cascade shadow map.
#define MAX_DIR_LIGHTS 4

#define POINT_LIGHT 0
#define SPOT_LIGHT 1

struct DirLight
{
    float3 direction;
    float3 color;
};

struct Light
{
    float4 positionViewSpace;
    float4 directionViewSpace;

    float3 position;
    float3 direction;
    float3 color;

    float range;
    float angle;
    float smoothness;
    float intensity;

    int type;
};

cbuffer LightningConstantBuffer : register(CB_LIGHTING)
{
    DirLight dirLights[MAX_DIR_LIGHTS];
    Light lights[MAX_LIGHTS];
    int dirLightsCount;
    int lightsCount;
};

float CalcSpotConeInViewSpace(Light light, float3 lightDir)
{
    // If the cosine angle of the light's direction 
    // vector and the vector from the light source to the point being 
    // shaded is less than minCos, then the spotlight contribution will be 0.
    float minCos = cos(radians(light.angle));
    // If the cosine angle of the light's direction vector
    // and the vector from the light source to the point being shaded
    // is greater than maxCos, then the spotlight contribution will be 1.
    float maxCos = lerp(minCos, 1, 0.5);
    float cosAngle = dot(light.directionViewSpace.xyz, -lightDir);
    // Blend between the maxixmum and minimum cosine angles.
    return smoothstep(minCos, maxCos, cosAngle);
}

float CalcSpotCone(Light light, float3 lightDir)
{
    // If the cosine angle of the light's direction 
    // vector and the vector from the light source to the point being 
    // shaded is less than minCos, then the spotlight contribution will be 0.
    float minCos = cos(radians(light.angle));
    // If the cosine angle of the light's direction vector
    // and the vector from the light source to the point being shaded
    // is greater than maxCos, then the spotlight contribution will be 1.
    float maxCos = lerp(minCos, 1, 0.5);
    float cosAngle = dot(light.direction, -lightDir);
    // Blend between the maxixmum and minimum cosine angles.
    return smoothstep(minCos, maxCos, cosAngle);
}

float CalcAttenuation(float range, float smoothness, float distance)
{
	return 1.0f - smoothstep(range * smoothness, range, distance);
}

#endif

