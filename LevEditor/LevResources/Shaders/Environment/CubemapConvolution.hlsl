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

    // Two nested loops, so this is squared: 0.025 is 15800 samples for one texel, and six faces of
    // them is most of a 26ms hitch every time the probe is rebuilt. That number comes from baking a
    // probe once at load, which is not what this does any more -- an atmosphere rebuilds it as the
    // camera flies. Irradiance is a cosine convolution over a whole hemisphere, about the smoothest
    // function in the renderer, and the sky it integrates has no sun disk in it to alias against.
    float sampleDelta = 0.07f;
    float nrSamples = 0.0f;
    for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
    {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			//clamped, not tone mapped -- see PreFiltering.hlsl. Reinhard here would cost the
			//irradiance probe its entire HDR range.
			float3 sample = min(environmentMap.Sample(Sampler, sampleVec).rgb, k_MaxRadiance);
            irradiance += sample * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0f / float(nrSamples));
    
    return float4(irradiance, 1.0f);
}