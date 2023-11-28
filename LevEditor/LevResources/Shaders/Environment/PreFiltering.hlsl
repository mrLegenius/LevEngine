//<--- Vertex Shader ---<<

cbuffer cubeMapConstantBuffer : register(b6)
{
    row_major matrix cubeMapViewProjections[6];
};

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 uv : UV;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    output.pos = float4(input.pos, 1.0f);
    output.uv = input.pos;

    return output;
}

//<--- Geometry Shader ---<<

struct GS_OUT
{
    float4 pos : SV_Position;
    uint slice : SV_RenderTargetArrayIndex;
    float3 uv : UV;
};

[instance(6)]
[maxvertexcount(3)]
void GSMain(triangle VS_OUT p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        GS_OUT gs;

        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), cubeMapViewProjections[id]);
        gs.slice = id;
        gs.uv = p[i].uv;
        stream.Append(gs);
    }
}

//<--- Pixel Shader ---<<

TextureCube environmentMap;
SamplerState Sampler;

static const float PI = 3.14159265359f;

float RadicalInverse_VdC(uint bits);
float2 Hammersley(uint i, uint N);
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);

cbuffer roughnessConstantBuffer : register(b7)
{
    float roughness;
};

float4 PSMain(GS_OUT input) : SV_Target
{
    float3 N = normalize(input.uv);    
    float3 R = N;
    float3 V = R;

    const uint SAMPLE_COUNT = 1024u;
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
            float HdotV = max(dot(N, V), 0.0);
            float D = DistributionGGX(N, H, roughness);
            float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            float3 sample = environmentMap.SampleLevel(Sampler, L, mipLevel).rgb;
            sample = sample / (sample + float3(1.0f, 1.0f, 1.0f));

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
