#include "../ShaderCommon.hlsl"
#include "../PBRCommon.hlsl"
#include "PlanetCommon.hlsli"

// The ground of a planet. One shader for both techniques: LEV_DEFERRED fills the G-buffer, without it
// the lighting is done here. PlanetPass compiles whichever the active technique needs, the same way
// the material shaders do.
//
// PBRCommon rather than MaterialPBR, because a planet binds no material: its properties come out of
// the biome table in CB_PLANET, and its textures are three arrays rather than six textures. That also
// keeps CB_MATERIAL and t0..t5 free, which is what lets the ground arrays sit at t0..t2.

struct PLANET_VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;

	// Climate, evaluated on the CPU per vertex: see PlanetChunkGeometry::Climate. A pass that does
	// not declare this stream simply does not get it -- which is how the shadow and depth passes draw
	// these same meshes with the ordinary vertex shader.
	float4 climate : CLIMATE;
};

struct PLANET_PS_IN
{
	float4 pos : SV_POSITION;
	float3x3 TBN : NORMAL;
	float3 fragPos : POSITION0;
	float depth : TEXCOORD1;

	//<--- Position in the planet's own space, which is where the ground textures are placed ---<<
	float3 planetPos : POSITION1;

	float4 climate : TEXCOORD2;
};

PLANET_PS_IN VSMain(PLANET_VS_IN input)
{
	PLANET_PS_IN output;

	float4 fragPos = mul(float4(input.pos, 1.0f), model);

	output.pos = mul(fragPos, viewProjection);
	output.fragPos = fragPos.xyz;
	output.depth = mul(fragPos, cameraView).z;

	// Back into the planet's own space, which is where the ground textures are placed. Textures placed
	// by world position would slide across the ground the moment the planet is moved or turned.
	output.planetPos = mul(fragPos, WorldToPlanet).xyz;

	output.climate = input.climate;

	float3 normal = normalize(mul(input.normal, (float3x3)transposedInvertedModel));
	float3 tangent = normalize(mul(input.tangent, (float3x3)model));
	float3 binormal = cross(normal, tangent);

	output.TBN = float3x3(tangent, binormal, normal);

	return output;
}

struct PlanetSurfaceProperties
{
	float3 Albedo;
	float3 Normal;
	float Roughness;
	float Metallic;
};

// Blends the biomes the point qualifies for. The loop runs over four slots because a height boundary
// crossing a climate boundary is four, and each slot costs three texture taps per array -- so slots
// whose weight rounds to nothing are skipped, which in practice leaves one or two.
PlanetSurfaceProperties PlanetShadeGround(float3 planetPos, float3 surfaceNormal, float4 climate,
                                          float3x3 TBN, float distanceToCamera)
{
	float elevation = climate.x;
	float temperature = climate.y;
	float humidity = climate.z;
	float slope = climate.w;

	PlanetBiomeBlendResult blend = PlanetClassify(temperature, humidity, elevation, slope);

	float3 triplanarWeights = PlanetTriplanarWeights(surfaceNormal, PlanetTriplanarSharpness);

	bool hasAlbedo = (PlanetTextureFlags & PLANET_HAS_ALBEDO) != 0;
	bool hasNormal = (PlanetTextureFlags & PLANET_HAS_NORMAL) != 0;
	bool hasRoughness = (PlanetTextureFlags & PLANET_HAS_ROUGHNESS) != 0;

	PlanetSurfaceProperties result;
	result.Albedo = 0.0f;
	result.Normal = surfaceNormal;
	result.Roughness = 0.0f;
	result.Metallic = 0.0f;

	float3 blendedNormal = 0.0f;

	// Unrolled, so the four layers read their weights and slices as constants -- and because a texture
	// sample inside a rolled loop makes the compiler assume the worst about the mip level it needs.
	// The weight test below still skips the samples at runtime; unrolling costs code, not work.
	[unroll]
	for (int slot = 0; slot < PLANET_MAX_LAYERS; ++slot)
	{
		float weight = blend.Weights[slot];

		//<--- A thousandth of a biome is not worth nine texture taps ---<<
		if (weight < 0.001f) continue;

		PlanetBiomeData biome = PlanetBiomes[blend.Indices[slot]];

		float scale = 1.0f / max(0.01f, biome.Surface.z);
		float slice = biome.Surface.w;

		float3 albedo = biome.Tint.rgb;

		if (hasAlbedo)
		{
			albedo *= PlanetSampleTriplanar(planetAlbedoArray, planetPos, triplanarWeights,
			                                scale, slice).rgb;
		}

		float roughness = biome.Surface.x;

		if (hasRoughness)
			roughness *= PlanetSampleTriplanar(planetRoughnessArray, planetPos, triplanarWeights, scale, slice).r;

		if (hasNormal)
		{
			float3 mapNormal = PlanetSampleTriplanarNormal(planetNormalArray, planetPos, triplanarWeights,
			                                               scale, slice, surfaceNormal, biome.Detail.x);

			//<--- Triplanar normals come out in planet space, so they go through the model, not the TBN ---<<
			blendedNormal += normalize(mul(mapNormal, (float3x3)transposedInvertedModel)) * weight;
		}

		result.Albedo += albedo * weight;
		result.Roughness += roughness * weight;
		result.Metallic += biome.Surface.y * weight;
	}

	if (hasNormal && dot(blendedNormal, blendedNormal) > 1e-6f)
		result.Normal = normalize(blendedNormal);

	// Detail below the size of a vertex. Two things at once: it breaks up the tiling of the ground
	// textures, and it puts a slope on ground the mesh says is flat. Faded out with distance because
	// past a few hundred units it is smaller than a pixel and only costs.
	float detailFade = 1.0f - smoothstep(PlanetDetailFadeStart, PlanetDetailFadeEnd, distanceToCamera);

	if (PlanetDetailStrength > 0.0f && detailFade > 0.01f)
	{
		float strength = PlanetDetailStrength * detailFade;

		float3 samplePoint = planetPos * PlanetDetailFrequency;

		// One evaluation, with the field's slope taken from it analytically. Bending the normal needs
		// the slope along the surface, which is the gradient with its component along the normal
		// removed -- the part pointing out of the surface only says the field is changing with height,
		// which is not a tilt.
		float3 gradient;
		float here = NoiseSimplexGradient(samplePoint, PlanetDetailSeed, gradient);

		float3 alongSurface = gradient - result.Normal * dot(gradient, result.Normal);
		result.Normal = normalize(result.Normal - alongSurface * strength);

		//<--- and a little of it into the albedo, which is what hides a repeating texture ---<<
		result.Albedo *= 1.0f + here * strength * 0.25f;
	}

	result.Roughness = saturate(result.Roughness);
	result.Metallic = saturate(result.Metallic);

	return result;
}

#ifdef LEV_DEFERRED

struct PS_OUT
{
	float4 LightAccumulation : SV_Target0;
	float4 Albedo : SV_Target1;
	float4 Normal : SV_Target2;
	float4 MetallicRoughnessAO : SV_Target3;
};

[earlydepthstencil]
PS_OUT PSMain(PLANET_PS_IN input)
{
	PS_OUT result;

	float3 viewDir = normalize(cameraPosition - input.fragPos);
	float distanceToCamera = length(cameraPosition - input.fragPos);

	PlanetSurfaceProperties surface = PlanetShadeGround(input.planetPos, normalize(input.TBN[2]),
	                                                   input.climate, input.TBN, distanceToCamera);

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	const float ao = 1.0f;

	float3 ambient = CalcAmbient(surface.Normal, viewDir, surface.Albedo, surface.Metallic,
	                             surface.Roughness, ao);
	float3 lit = CalcDirLights(surface.Normal, viewDir, fragPosLightSpace, cascade, surface.Albedo,
	                           surface.Metallic, surface.Roughness);

	result.LightAccumulation = float4(ambient + lit, 1.0f);
	result.Albedo = float4(surface.Albedo, 1.0f);
	result.MetallicRoughnessAO = float4(surface.Metallic, surface.Roughness, ao, 0.0f);
	result.Normal = float4(surface.Normal, 0.0f);

	return result;
}

#else

[earlydepthstencil]
float4 PSMain(PLANET_PS_IN input) : SV_Target
{
	float3 viewDir = normalize(cameraPosition - input.fragPos);
	float distanceToCamera = length(cameraPosition - input.fragPos);

	PlanetSurfaceProperties surface = PlanetShadeGround(input.planetPos, normalize(input.TBN[2]),
	                                                   input.climate, input.TBN, distanceToCamera);

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	const float ao = 1.0f;

	float3 total = CalcAmbient(surface.Normal, viewDir, surface.Albedo, surface.Metallic,
	                           surface.Roughness, ao);

	total += CalcDirLights(surface.Normal, viewDir, fragPosLightSpace, cascade, surface.Albedo,
	                       surface.Metallic, surface.Roughness);

	for (int i = 0; i < lightsCount; i++)
	{
		Light light = lights[i];

		if (light.type == POINT_LIGHT)
			total += CalcPointLight(light, surface.Normal, input.fragPos, viewDir, surface.Albedo,
			                        surface.Metallic, surface.Roughness);
		else if (light.type == SPOT_LIGHT)
			total += CalcSpotLight(light, surface.Normal, input.fragPos, viewDir, surface.Albedo,
			                       surface.Metallic, surface.Roughness);
	}

	//<--- Linear HDR out; tone mapping and gamma belong to PostProcessingPass ---<<
	return float4(total, 1.0f);
}

#endif
