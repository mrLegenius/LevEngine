// Renders the atmosphere into the six faces of a cubemap, which feeds the irradiance and prefilter
// chain. Shares the vertex and geometry stages with the other cubemap builders (CubemapRender.hlsl),
// and the sky model itself with the on-screen pass.
//
// This is a light probe and nothing else -- the sky on screen is marched per pixel by
// PlanetAtmospherePass. Step counts are therefore higher than the screen pass and the resolution
// lower: it runs rarely, so it can afford to be the accurate one.

#include "Common.hlsli"
#include "Atmosphere.hlsli"

// 24 and 8 while this was the sky people looked at, where a step in the gradient would have shown.
// It feeds the irradiance and prefilter chain now, which integrates over a whole hemisphere and
// cannot resolve a step even in principle. The product is what costs -- six faces of it measured
// 29ms of GPU per rebuild at 24 by 8 -- so dropping to 16 by 4 is a quarter of the work for a
// difference in the ambient nobody can point at.
static const int k_ViewSteps = 16;
static const int k_LightSteps = 4;

float4 PSMain(PS_IN input) : SV_Target
{
    float3 direction = normalize(input.uv);

    // No sun disks: the sky pass draws them sharp on top, and the suns light the scene directly
    // as well, so baking them in here would count them twice.
    //
    // Alpha carries how much of what lies beyond the air gets through. The skybox is deliberately not
    // composited in here: this cubemap is 128 pixels a face and would blur it away, and it feeds the
    // irradiance chain, where a nebula contributes no light worth speaking of.
    float transmittance;
    float3 sky = RenderSky(direction, k_ViewSteps, k_LightSteps, false, transmittance);

    return float4(sky, transmittance);
}
