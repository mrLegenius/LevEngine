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

	// The biome blend, already done. Everything that is not a texture lookup is linear in the biome
	// weights, so it is worked out once per vertex and interpolated -- see PlanetBlendFlat. A planet
	// with no ground textures needs nothing else and never looks at a biome per pixel.
	float3 biomeAlbedo : BIOMEALBEDO;

	//<--- Roughness, metallic ---<<
	float2 biomeSurface : BIOMESURFACE;

	// Still carried, because the textured path has to redo the blend per pixel: a texture lookup is
	// not linear in position, so it cannot be interpolated the way a tint can.
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

	// The whole biome blend, for everything that is not a texture. Per vertex because it is linear in
	// the weights and the weights are smooth -- see PlanetBlendFlat.
	PlanetBlendFlat(input.climate.y, input.climate.z, input.climate.x, input.climate.w,
	                output.biomeAlbedo, output.biomeSurface.x, output.biomeSurface.y);

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

	//<--- How much of the sky reaches this point. See the detail block for where it comes from ---<<
	float Occlusion;
};

// The ground's material at one pixel.
//
// The biome blend arrives already done, from the vertex shader, and that covers every planet in this
// engine today because none of them have ground textures yet. Only when there are textures to sample
// does the blend have to be redone per pixel, and then it is behind a branch on a constant buffer
// value -- so every pixel in a draw takes the same side of it and the wave never splits.
//
// That branch is what got this pass from four and a half milliseconds to two and a half. What was
// costing was not working out the biome weights, which was the obvious suspect and the wrong one, but
// the four dynamically indexed reads out of the biome array that the blend did afterwards.
PlanetSurfaceProperties PlanetShadeGround(float3 planetPos, float3 surfaceNormal, float4 climate,
                                          float3 biomeAlbedo, float2 biomeSurface,
                                          float3x3 TBN, float distanceToCamera)
{
	PlanetSurfaceProperties result;
	result.Albedo = biomeAlbedo;
	result.Normal = surfaceNormal;
	result.Roughness = biomeSurface.x;
	result.Metallic = biomeSurface.y;
	result.Occlusion = 1.0f;

	[branch]
	if (PlanetTextureFlags != 0)
	{
		float4 weights0, weights1, weights2, weights3;
		PlanetComputeWeights(climate.y, climate.z, climate.x, climate.w,
		                     weights0, weights1, weights2, weights3);

		PlanetBiomeBlendResult blend = PlanetClassifyWeights(weights0, weights1, weights2, weights3);

		float3 triplanarWeights = PlanetTriplanarWeights(surfaceNormal, PlanetTriplanarSharpness);

		bool hasAlbedo = (PlanetTextureFlags & PLANET_HAS_ALBEDO) != 0;
		bool hasNormal = (PlanetTextureFlags & PLANET_HAS_NORMAL) != 0;
		bool hasRoughness = (PlanetTextureFlags & PLANET_HAS_ROUGHNESS) != 0;

		result.Albedo = 0.0f;
		result.Roughness = 0.0f;
		result.Metallic = 0.0f;

		float3 blendedNormal = 0.0f;

		// Unrolled, so the four layers read their weights and slices as constants -- and because a
		// texture sample inside a rolled loop makes the compiler assume the worst about the mip level
		// it needs. The weight test below still skips the samples at runtime; unrolling costs code,
		// not work.
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
	}

	// Detail below the size of a vertex. Two things at once: it breaks up the tiling of the ground
	// textures, and it puts a slope on ground the mesh says is flat. Faded out with distance because
	// past a few hundred units it is smaller than a pixel and only costs.
	float detailFade = 1.0f - smoothstep(PlanetDetailFadeStart, PlanetDetailFadeEnd, distanceToCamera);

	if (PlanetDetailStrength > 0.0f && detailFade > 0.01f)
	{
		float strength = PlanetDetailStrength * detailFade;

		// Frequency is in cycles per planet radius, so features come out Radius/Frequency world units
		// across and a planet keeps its look at any size -- the same convention the shape's noise uses.
		float3 samplePoint = planetPos * (PlanetDetailFrequency / max(PlanetSurfaceRadius, 1e-4f));

		// Each evaluation brings the field's slope with it analytically. Bending the normal needs the
		// slope along the surface, which is the gradient with its component along the normal removed --
		// the part pointing out of the surface only says the field is changing with height, which is
		// not a tilt.
		float3 gradient;
		float here = NoiseSimplexGradient(samplePoint, PlanetDetailSeed, gradient);

		// A second octave, because one is what made the ground look moulded. A single frequency of
		// simplex is a field of smooth round blobs of one size, and ground lit through it reads as
		// something soft and poured rather than something eroded -- real ground has detail at every
		// size at once. 4.3 rather than 4 keeps the two from ever lining up into a visible pattern,
		// and the amplitude is chosen so its contribution to the slope roughly matches the first: it
		// is the slope, not the height, that the light actually shows.
		float3 fineGradient;
		float fine = NoiseSimplexGradient(samplePoint * 4.3f, PlanetDetailSeed + 101u, fineGradient);

		here += fine * 0.3f;
		gradient += fineGradient * (4.3f * 0.3f);

		float3 alongSurface = gradient - result.Normal * dot(gradient, result.Normal);
		result.Normal = normalize(result.Normal - alongSurface * strength);

		//<--- and a little of it into the albedo, which is what hides a repeating texture ---<<
		result.Albedo *= 1.0f + here * strength * 0.25f;

		// With no roughness texture assigned, a biome is a single gloss value across the whole of
		// itself, so the specular is one unbroken sheet sliding over the ground as the camera moves --
		// which is what moulded plastic does. Ground varies grain to grain. This is the noise that
		// already bent the normal, so it costs nothing more and agrees with the bumps.
		result.Roughness += here * strength * 0.35f;

		// Occlusion, and this is the one that matters most. The ambient term was being handed a flat
		// 1.0 -- see PSMain, where it was a named constant -- so every crevice in the ground received
		// exactly as much sky as every ridge. Nothing reads as solid under that: it is the darkening
		// in the folds that tells the eye a surface has depth rather than a pattern painted on it, and
		// without any the ground can only look like something moulded in one piece.
		//
		// There is no SSAO pass in this renderer to ask, but the ground does not need one: the noise
		// that makes the folds already knows where they are. Low field means a hollow, and a hollow
		// sees less of the sky. Costs one multiply-add on a value already in a register.
		result.Occlusion = saturate(1.0f + min(here, 0.0f) * strength * 0.9f);
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
	                                                   input.climate, input.biomeAlbedo, input.biomeSurface,
	                                                   input.TBN, distanceToCamera);

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 ambient = CalcAmbient(surface.Normal, viewDir, surface.Albedo, surface.Metallic,
	                             surface.Roughness, surface.Occlusion);
	float3 lit = CalcDirLights(surface.Normal, viewDir, fragPosLightSpace, cascade, surface.Albedo,
	                           surface.Metallic, surface.Roughness);

	result.LightAccumulation = float4(ambient + lit, 1.0f);
	result.Albedo = float4(surface.Albedo, 1.0f);
	result.MetallicRoughnessAO = float4(surface.Metallic, surface.Roughness, surface.Occlusion, 0.0f);
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
	                                                   input.climate, input.biomeAlbedo, input.biomeSurface,
	                                                   input.TBN, distanceToCamera);

	float cascade = GetCascadeIndex(input.depth);
	float4 fragPosLightSpace = mul(float4(input.fragPos, 1.0f), lightViewProjection[cascade]);

	float3 total = CalcAmbient(surface.Normal, viewDir, surface.Albedo, surface.Metallic,
	                           surface.Roughness, surface.Occlusion);

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
