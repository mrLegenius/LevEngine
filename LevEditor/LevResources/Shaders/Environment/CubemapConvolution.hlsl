#include "Common.hlsli"

TextureCube environmentMap;
SamplerState Sampler;

float4 PSMain(PS_IN input) : SV_Target
{
    float3 normal = normalize(input.uv);
    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			float3 sample = environmentMap.Sample(Sampler, sampleVec).rgb;
			sample = sample / (sample + float3(1.0f, 1.0f, 1.0f));
            irradiance += sample * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    
    return float4(irradiance, 1.0f);
}