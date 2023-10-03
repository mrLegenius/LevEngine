#define MAX_POINT_LIGHTS 200

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

cbuffer LightningConstantBuffer : register(b2)
{
    DirLight dirLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    float3 globalAmbient;
    int pointLightsCount;
};



