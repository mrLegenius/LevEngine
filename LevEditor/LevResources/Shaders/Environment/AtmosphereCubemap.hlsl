// Renders the atmosphere into the six faces of a cubemap, which then feeds the irradiance and
// prefilter chain. Shares the vertex and geometry stages with the other cubemap builders
// (CubemapRender.hlsl), and the sky model itself with the on-screen pass.
//
// Step counts are higher than the screen pass: this runs at a low resolution and only when the
// sun has moved far enough to matter, so it can afford to be the accurate one.

#include "Common.hlsli"
#include "Atmosphere.hlsli"

static const int k_ViewSteps = 24;
static const int k_LightSteps = 8;

float4 PSMain(PS_IN input) : SV_Target
{
    float3 direction = normalize(input.uv);

    // No sun disks: the sky pass draws them sharp on top, and the suns light the scene directly
    // as well, so baking them in here would count them twice.
    return float4(RenderSky(direction, k_ViewSteps, k_LightSteps, false), 1.0f);
}
