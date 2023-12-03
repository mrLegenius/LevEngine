#define MAX_LIGHTS 100

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
    float3 color;

    float range;
    float angle;
    float smoothness;
    float intensity;

    int type;
};

cbuffer LightningConstantBuffer : register(b2)
{
    DirLight dirLight;
    Light lights[MAX_LIGHTS];
    int lightsCount;
};

float CalcSpotCone(Light light, float3 lightDir)
{
    // If the cosine angle of the light's direction 
    // vector and the vector from the light source to the point being 
    // shaded is less than minCos, then the spotlight contribution will be 0.
    float minCos = cos(radians(light.angle));
    // If the cosine angle of the light's direction vector
    // and the vector from the light source to the point being shaded
    // is greater than maxCos, then the spotlight contribution will be 1.
    float maxCos = lerp(minCos, 1, light.smoothness);
    float cosAngle = dot(light.directionViewSpace.xyz, -lightDir);
    // Blend between the maxixmum and minimum cosine angles.
    return smoothstep(minCos, maxCos, cosAngle);
}

float CalcAttenuation(float range, float smoothness, float distance)
{
	return 1.0f - smoothstep(range * smoothness, range, distance);
}



