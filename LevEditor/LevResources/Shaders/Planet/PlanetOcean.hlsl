#include "../ShaderCommon.hlsl"
#include "../PBRCommon.hlsl"
#include "PlanetCommon.hlsli"

// The sea, drawn from the terrain's own chunk meshes with their vertices pulled out to sea level.
//
// It has no geometry of its own on purpose. A separate sphere would need its own level of detail
// scheme to stop the horizon going polygonal, and its own quadtree to avoid spending vertices on the
// half of the planet that is dry. Reprojecting the terrain chunks costs one line in the vertex shader
// and inherits both: wherever the ground is finely tessellated, so is the water over it.
//
// It also means the water knows how deep it is. The terrain's elevation is already in the vertex
// stream, so the depth at a pixel is just minus that -- no depth buffer read, no second pass, and an
// exact shoreline, since the surface is clipped where the elevation crosses zero.

struct PLANET_OCEAN_VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
	float4 climate : CLIMATE;
};

struct PLANET_OCEAN_PS_IN
{
	float4 pos : SV_POSITION;
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;

	//<--- Sea level position in planet space, which is where the waves are placed ---<<
	float3 planetPos : POSITION1;

	//<--- Outward direction, which for a sphere of water is the surface normal ---<<
	float3 up : NORMAL;

	//<--- How deep the water is here, in world units ---<<
	float waterDepth : TEXCOORD2;
};

PLANET_OCEAN_PS_IN VSMain(PLANET_OCEAN_VS_IN input)
{
	PLANET_OCEAN_PS_IN output;

	//<--- Out to the chunk's own place in the world, into planet space, out to sea level, and back ---<<
	float4 terrainWorld = mul(float4(input.pos, 1.0f), model);
	float3 direction = normalize(mul(terrainWorld, WorldToPlanet).xyz);

	float3 seaPosition = direction * PlanetSurfaceRadius;

	float4 fragPos = mul(float4(seaPosition, 1.0f), PlanetToWorld);

	output.pos = mul(fragPos, viewProjection);
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;
	output.planetPos = seaPosition;
	output.up = normalize(mul(direction, (float3x3)transposedInvertedModel));

	//<--- Elevation is signed and measured from sea level, so the depth is its negative ---<<
	output.waterDepth = -input.climate.x;

	return output;
}

[earlydepthstencil]
float4 PSMain(PLANET_OCEAN_PS_IN input) : SV_Target
{
	// Where the terrain rises above sea level there is no sea. Clipping on the interpolated elevation
	// puts the shoreline exactly where the ground crosses zero, to the pixel, without the water and
	// the beach needing to agree on anything else.
	clip(input.waterDepth);

	float3 up = normalize(input.up);
	float3 viewDir = normalize(cameraPosition - input.fragPos);

	// Waves as a moving noise field, bending the normal rather than displacing the vertices: a
	// displacement fine enough to see would need a tessellation the ocean does not have, and at the
	// scale of a planet what reads as water is the way it catches the light, not its silhouette.
	float3 normal = up;

	if (PlanetWaveStrength > 0.0f)
	{
		float3 samplePoint = input.planetPos * PlanetWaveScale;

		//<--- Drifting the sample point through the third dimension of the field animates it ---<<
		samplePoint += up * (PlanetTime * PlanetWaveSpeed);

		//<--- One evaluation and its analytic slope, rather than three samples differenced ---<<
		float3 gradient;
		NoiseSimplexGradient(samplePoint, 0x51ED2701u, gradient);

		//<--- Waves flatten out over deep water, where there is no bottom to trip them up ---<<
		float shoreFactor = 1.0f - saturate(input.waterDepth / max(1.0f, PlanetOceanDepthFalloff));
		float strength = PlanetWaveStrength * (0.6f + 0.4f * shoreFactor);

		// Faded out where the water is seen edge on. A wave narrower than the pixel looking at it is
		// not detail, it is noise: every pixel lands on a different part of the field and the whole
		// band boils. This is the same reason a normal map needs mips, and the horizon of a planet is
		// all grazing angles.
		strength *= smoothstep(0.0f, 0.25f, abs(dot(up, viewDir)));

		float3 alongSurface = gradient - up * dot(gradient, up);
		normal = normalize(up - alongSurface * strength);
	}

	//<--- Shallow water shows the bottom through it; deep water shows only itself ---<<
	float depthFactor = saturate(input.waterDepth / max(1.0f, PlanetOceanDepthFalloff));
	float3 albedo = lerp(PlanetOceanShallowColor.rgb, PlanetOceanDeepColor.rgb, depthFactor);

	const float roughness = PlanetOceanRoughness;
	const float metallic = 0.0f;
	const float ao = 1.0f;

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 total = CalcAmbient(normal, viewDir, albedo, metallic, roughness, ao);
	total += CalcDirLights(normal, viewDir, fragPosLightSpace, cascade, albedo, metallic, roughness);

	for (int i = 0; i < lightsCount; i++)
	{
		Light light = lights[i];

		if (light.type == POINT_LIGHT)
			total += CalcPointLight(light, normal, input.fragPos, viewDir, albedo, metallic, roughness);
		else if (light.type == SPOT_LIGHT)
			total += CalcSpotLight(light, normal, input.fragPos, viewDir, albedo, metallic, roughness);
	}

	// Water seen from above is nearly clear and seen edge on is a mirror, which is Fresnel and is most
	// of what makes water look like water. Deep water is also less transparent than shallow, because
	// there is more of it to look through.
	float fresnel = pow(1.0f - saturate(dot(normal, viewDir)), 5.0f);

	float alpha = PlanetOceanOpacity * lerp(0.45f, 1.0f, depthFactor);
	alpha = saturate(alpha + fresnel * PlanetOceanFresnel);

	return float4(total, alpha);
}
