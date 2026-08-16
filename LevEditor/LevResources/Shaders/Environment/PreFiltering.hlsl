#include "Common.hlsli"

TextureCube environmentMap;
SamplerState Sampler;

float RadicalInverse_VdC(uint bits);
float2 Hammersley(uint i, uint N);
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);

cbuffer roughnessConstantBuffer : register(CB_DEBUG)
{
    float roughness;
};

float4 PSMain(PS_IN input) : SV_Target
{
    float3 N = normalize(input.uv);    
    float3 R = N;
    float3 V = R;

    // 1024 is the number for baking a probe once and never again. This runs whenever the sky is
    // rebuilt, which with an atmosphere is while the camera is flying, and at 128 pixels a face over
    // six faces and every mip it was the other half of the hitch. 128 importance samples is the
    // ordinary real time figure and holds up on an environment this smooth.
    const uint SAMPLE_COUNT = 128u;
    float totalWeight = 0.0;
    float3 prefilteredColor = 0.0f;  

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H  = ImportanceSampleGGX(Xi, N, roughness);
        float3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            float NdotH = max(dot(N, H), 0.0);
            //the GGX pdf is over the half vector, so this has to be H.V -- N.V is a constant
            //1.0 here (V == N == R) and silently collapses the mip selection below
            float HdotV = max(dot(H, V), 0.0);
            float D = DistributionGGX(N, H, roughness);
            float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            //clamp rather than tone map: running Reinhard here would flatten the environment
            //to LDR before integration and then get tone mapped a second time at the end of
            //the frame. The clamp still keeps a single bright texel from becoming a firefly.
            float3 sample = min(environmentMap.SampleLevel(Sampler, L, mipLevel).rgb, k_MaxRadiance);

            prefilteredColor += sample * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    
    return float4(prefilteredColor, 1.0);
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}  

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent   = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
