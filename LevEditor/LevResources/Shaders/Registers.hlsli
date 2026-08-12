#ifndef LEV_REGISTERS_HLSLI
#define LEV_REGISTERS_HLSLI

// Single source of truth for the binding slots shared between the engine and the shaders.
// Slots are shared by every stage. The comment on each line names the C++ side that binds it.

//<--- Constant buffers ---<<

#define CB_CAMERA			b0	// Renderer3D
#define CB_MODEL			b1	// Renderer3D
#define CB_LIGHTING			b2	// LightCollection
#define CB_LIGHT_SPACE		b3	// ShadowMapPass
#define CB_MATERIAL			b4	// Material::Bind
#define CB_LIGHT_INDEX		b4	// DeferredLightingPass
#define CB_SCREEN_TO_VIEW	b5	// Renderer
#define CB_SKYBOX			b6	// EnvironmentPrecomputePass, EnvironmentRenderPass
#define CB_DEBUG			b7	// DebugRenderPass, EnvironmentPrecomputePass (prefilter roughness)
#define CB_POST_PROCESSING	b8	// PostProcessingPass

// CB_LIGHT_INDEX aliases CB_MATERIAL: the deferred lighting pass has no material bound and
// the geometry passes have no light index. Never pull both into one shader -- FXC only lets
// the overlap slide while one of the two buffers is completely unreferenced, and turns it
// into X4509 the moment it is not. Include PBRCommon.hlsl, not MaterialPBR.hlsl, in the
// lighting pass to keep that true.

//<--- Textures ---<<

#define T_PBR_ALBEDO		t0	// MaterialPBR::TextureType
#define T_PBR_METALLIC		t1
#define T_PBR_ROUGHNESS		t2
#define T_PBR_NORMAL		t3
#define T_PBR_AO			t4
#define T_PBR_EMISSIVE		t5

#define T_GBUFFER_ALBEDO	t1	// DeferredLightingPass
#define T_GBUFFER_NORMAL	t2
#define T_GBUFFER_MRAO		t3
#define T_GBUFFER_DEPTH		t4

// The G-buffer set deliberately reuses t1..t4. Same rule as CB_LIGHT_INDEX above: a shader
// gets the material set or the G-buffer set, never both.

#define T_SHADOW_MAP		t9	// ShadowMapPass
#define T_IRRADIANCE		t10	// EnvironmentPass
#define T_PREFILTER			t11
#define T_BRDF_LUT			t12

//<--- Samplers ---<<

#define S_PBR_ALBEDO		s0
#define S_PBR_METALLIC		s1
#define S_PBR_ROUGHNESS		s2
#define S_PBR_NORMAL		s3
#define S_PBR_AO			s4
#define S_PBR_EMISSIVE		s5

#define S_SHADOW_MAP		s9
#define S_IRRADIANCE		s10
#define S_PREFILTER			s11
#define S_BRDF_LUT			s12

//<--- Particle compute ---<<
// The particle passes are a separate namespace: nothing above is bound while they run.

#define CB_PARTICLE_CAMERA	b0	// ParticlePass
#define CB_PARTICLE_HANDLER	b1
#define CB_PARTICLE_EMITTER	b2	// ParticleEmissionPass
#define CB_PARTICLE_RANDOM	b3
#define CB_PARTICLE_DEAD	b4
#define CB_PARTICLE_SORT	b0	// BitonicSort

#define T_PARTICLE_BUFFER	t0	// ParticleRenderPass
#define T_PARTICLE_TEXTURE	t1
#define T_PARTICLE_SORTED	t2
#define T_PARTICLE_NORMAL	t8	// ParticleSimulationPass
#define T_PARTICLE_DEPTH	t9

#define S_PARTICLE_TEXTURE	s1

#define U_PARTICLES			u0
#define U_DEAD_PARTICLES	u1
#define U_SORTED_PARTICLES	u2

#endif
