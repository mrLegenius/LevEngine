// Atmospheric fog. Fullscreen pass over the linear HDR target, after the scene is lit and
// before tone mapping, so the fog goes through the same exposure curve as everything else.
// Density falls off exponentially with world height, and is integrated analytically along the
// view ray, which is what makes the fog thin out as the camera climbs instead of being a flat
// distance blend.

#include "Quad.hlsl"
#include "Registers.hlsli"

cbuffer FogParams : register(CB_FOG)
{
    row_major matrix InverseViewProjection;

    float3 CameraPosition;
    float FogDensity;

    float3 FogColor;
    float FogHeightFalloff;

    float FogHeight;
    float FogStartDistance;
    float FogMaxOpacity;
    float FogAffectsSkybox;

    float3 SunDirection;
    float SunScatteringIntensity;

    float3 SunColor;
    float SunScatteringExponent;
};

Texture2D depthTexture : register(T_FOG_DEPTH);

// Optical depth of an exponential height fog along a ray segment.
// density(h) = FogDensity * exp(-FogHeightFalloff * (h - FogHeight)), integrated over t in [0, length].
float CalcOpticalDepth(float3 start, float3 direction, float rayLength)
{
    float densityAtStart = FogDensity * exp(-FogHeightFalloff * (start.y - FogHeight));
    float falloffAlongRay = FogHeightFalloff * direction.y;

    // A ray that is (nearly) horizontal stays at one height, and the integral degenerates to
    // density * length. Taking the general form there would divide by ~0.
    if (abs(falloffAlongRay) < 0.0001f)
        return densityAtStart * rayLength;

    return densityAtStart * (1.0f - exp(-falloffAlongRay * rayLength)) / falloffAlongRay;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float depth = depthTexture.Load(int3(input.pos.xy, 0)).r;

    // Nothing was drawn here, so the pixel is the skybox sitting at the far plane.
    bool isSky = depth >= 0.9999f;
    if (isSky && FogAffectsSkybox < 0.5f)
        return 0.0f;

    float4 clip = float4(input.uv.x * 2.0f - 1.0f, (1.0f - input.uv.y) * 2.0f - 1.0f, depth, 1.0f);
    float4 world = mul(clip, InverseViewProjection);
    world /= world.w;

    float3 ray = world.xyz - CameraPosition;
    float rayLength = length(ray);
    float3 direction = ray / max(rayLength, 0.0001f);

    // Fog only accumulates past the start distance, so geometry close to the camera stays clear.
    float fogged = max(rayLength - FogStartDistance, 0.0f);
    if (fogged <= 0.0f)
        return 0.0f;

    float3 start = CameraPosition + direction * min(FogStartDistance, rayLength);

    float opticalDepth = CalcOpticalDepth(start, direction, fogged);
    float fogFactor = min(1.0f - exp(-opticalDepth), FogMaxOpacity);

    // Inscattering: looking towards the sun tints the fog with the light colour.
    float sunAmount = saturate(dot(direction, -normalize(SunDirection)));
    float3 color = lerp(FogColor,
                        SunColor,
                        saturate(pow(sunAmount, max(SunScatteringExponent, 1.0f)) * SunScatteringIntensity));

    return float4(color, fogFactor);
}
