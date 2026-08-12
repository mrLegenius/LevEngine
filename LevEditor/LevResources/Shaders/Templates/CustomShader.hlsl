// A custom LevEngine shader. Create a material from it (Create -> Shader Material in the asset
// browser) and everything declared below turns into a row in that material's inspector.
//
// What becomes a material property:
//   * every field of MaterialConstantBuffer -- float, float2, float3, float4, int and bool
//   * every Texture2D bound to registers t0..t8
// The engine reads them out of the compiled shader, so a renamed field is a renamed row, and a
// material keeps the values of every property that still exists.
//
// The engine compiles this file once per permutation it needs, defining:
//   LEV_DEFERRED     the deferred technique's G-buffer pass -- PSMain fills PS_OUT
//   WITH_INSTANCING  instanced draw -- use GET_MODEL(input), the model buffer is not bound
//   WITH_ANIMATIONS  skinned mesh -- CalculateVertex does the skinning
// Every one of them has to compile: a permutation that does not falls back to the engine's own
// shader for those draws. Both stages are required, VSMain and PSMain.
//
// Shadows are not drawn with this shader: casters go through the engine's shadow pass.

#include "ShaderCommon.hlsl"
#include "PBRCommon.hlsl"

struct CustomMaterial
{
    float3 tint;

    float2 tiling;
    float2 offset;

    float metallic;
    float roughness;
};

cbuffer MaterialConstantBuffer : register(CB_MATERIAL)
{
    CustomMaterial material;
};

Texture2D albedoMap : register(T_PBR_ALBEDO);
SamplerState albedoMapSampler : register(S_PBR_ALBEDO);

Texture2D normalMap : register(T_PBR_NORMAL);
SamplerState normalMapSampler : register(S_PBR_NORMAL);

PS_IN VSMain(VS_IN input)
{
    PS_IN output;

    VertexCalculationResult result = CalculateVertex(input);

    float4 fragPos = mul(result.pos, GET_MODEL(input));
    float3 binormal = cross(result.normal, result.tangent);

    output.pos = mul(fragPos, viewProjection);
    output.uv = input.uv;
    output.fragPos = fragPos.xyz;
    output.depth = mul(fragPos, cameraView).z;

    output.TBN = float3x3(normalize(result.tangent),
                          normalize(binormal),
                          normalize(result.normal));

    return output;
}

// What the pixel shader works out, in one place, so both outputs below stay in sync.
struct Surface
{
    float3 Albedo;
    float3 Normal;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
    float Alpha;
};

Surface CalculateSurface(PS_IN input)
{
    Surface surface;

    float2 uv = ApplyTextureProperties(input.uv, material.tiling, material.offset);
    float4 albedo = albedoMap.Sample(albedoMapSampler, uv);

    surface.Albedo = albedo.rgb * material.tint;
    surface.Alpha = albedo.a;
    surface.Normal = CalculateNormal(normalMap, normalMapSampler, uv, input.TBN);
    surface.Metallic = material.metallic;
    surface.Roughness = material.roughness;
    surface.AmbientOcclusion = 1.0f;

    return surface;
}

#ifdef LEV_DEFERRED

// The G-buffer layout of Renderer::Init. Deferred lighting reads these four targets, so the
// only lighting done here is the ambient and directional part, exactly like DeferredOpaquePBR.
struct PS_OUT
{
    float4 LightAccumulation : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 Normal : SV_Target2;
    float4 MetallicRoughnessAO : SV_Target3;
};

[earlydepthstencil]
PS_OUT PSMain(PS_IN input)
{
    Surface surface = CalculateSurface(input);

    float cascade = GetCascadeIndex(input.depth);
    float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);
    float3 viewDir = normalize(cameraPosition - input.fragPos);

    float3 ambient = CalcAmbient(surface.Normal, viewDir, surface.Albedo,
                                 surface.Metallic, surface.Roughness, surface.AmbientOcclusion);
    float3 lit = CalcDirLight(dirLight, surface.Normal, viewDir, fragPosLightSpace, cascade,
                              surface.Albedo, surface.Metallic, surface.Roughness);

    PS_OUT result;

    result.LightAccumulation = float4(ambient + lit, 1.0f);
    result.Albedo = float4(surface.Albedo, 1.0f);
    result.Normal = float4(surface.Normal, 0.0f);
    result.MetallicRoughnessAO = float4(surface.Metallic, surface.Roughness, surface.AmbientOcclusion, 0.0f);

    return result;
}

#else

// Forward output: linear HDR into the scene target. Tone mapping and gamma belong to
// PostProcessingPass, doing either here would apply the curve twice.
float4 PSMain(PS_IN input) : SV_Target
{
    Surface surface = CalculateSurface(input);

    float cascade = GetCascadeIndex(input.depth);
    float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);
    float3 viewDir = normalize(cameraPosition - input.fragPos);

    float3 color = CalcAmbient(surface.Normal, viewDir, surface.Albedo,
                               surface.Metallic, surface.Roughness, surface.AmbientOcclusion);

    color += CalcDirLight(dirLight, surface.Normal, viewDir, fragPosLightSpace, cascade,
                          surface.Albedo, surface.Metallic, surface.Roughness);

    for (int i = 0; i < lightsCount; i++)
    {
        Light light = lights[i];

        if (light.type == POINT_LIGHT)
            color += CalcPointLight(light, surface.Normal, input.fragPos, viewDir,
                                    surface.Albedo, surface.Metallic, surface.Roughness);
        else if (light.type == SPOT_LIGHT)
            color += CalcSpotLight(light, surface.Normal, input.fragPos, viewDir,
                                   surface.Albedo, surface.Metallic, surface.Roughness);
    }

    return float4(color, surface.Alpha);
}

#endif
