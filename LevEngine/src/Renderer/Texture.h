#pragma once
#include "ClearFlags.h"
#include "CPUAccess.h"
#include "SamplerState.h"
#include "ShaderType.h"
#include "DataTypes/Utility.h"
#include "Math/Vector4.h"

namespace LevEngine
{
	class Texture
	{
	public:
		enum class Dimension
		{
			Texture1D,
			Texture1DArray,
			Texture2D,
			Texture2DArray,
			Texture3D,
			TextureCube,
		};
		// The number of components used to create the texture.
		enum class Components
		{
			R,              // One red component.
			RG,             // Red, and green components.
			RGB,            // Red, green, and blue components.
			RGBA,           // Red, green, blue, and alpha components.
			Depth,          // Depth component.
			DepthStencil    // Depth and stencil in the same texture.
		};

		// The type of components in the texture.
		enum class Type
		{
			Typeless,           // Typeless formats.
			SRGB,
			UnsignedNormalized, // Unsigned normalized (8, 10, or 16-bit unsigned integer values mapped to the range [0..1])
			SignedNormalized,   // Signed normalized (8, or 16-bit signed integer values mapped to the range [-1..1])
			Float,              // Floating point format (16, or 32-bit).
			UnsignedInteger,    // Unsigned integer format (8, 16, or 32-bit unsigned integer formats).
			SignedInteger,      // Signed integer format (8, 16, or 32-bit signed integer formats).
		};

		struct TextureFormat
		{
			Components Components;
			Type Type;

			// For multi-sample textures, we can specify how many samples we want 
			// to use for this texture. Valid values are usually in the range [1 .. 16]
			// depending on hardware support.
			// A value of 1 will effectively disable multisampling in the texture.
			uint8_t NumSamples;

			// Components should commonly be 8, 16, or 32-bits but some texture formats
			// support 1, 10, 11, 12, or 24-bits per component.
			uint8_t RedBits;
			uint8_t GreenBits;
			uint8_t BlueBits;
			uint8_t AlphaBits;
			uint8_t DepthBits;
			uint8_t StencilBits;

			// By default create a 4-component unsigned normalized texture with 8-bits per component and no multisampling.
			explicit TextureFormat(const Texture::Components components = Components::RGBA,
			                       const Texture::Type type = Type::UnsignedNormalized,
			                       const uint8_t numSamples = 1,
			                       const uint8_t redBits = 8,
			                       const uint8_t greenBits = 8,
			                       const uint8_t blueBits = 8,
			                       const uint8_t alphaBits = 8,
			                       const uint8_t depthBits = 0,
			                       const uint8_t stencilBits = 0)
				: Components(components)
				, Type(type)
				, NumSamples(numSamples)
				, RedBits(redBits)
				, GreenBits(greenBits)
				, BlueBits(blueBits)
				, AlphaBits(alphaBits)
				, DepthBits(depthBits)
				, StencilBits(stencilBits)
			{ } 
		};

		// For cube maps, we may need to access a particular face of the cube map.
		enum class CubeFace
		{
			Right,  // +X
			Left,   // -X
			Top,    // +Y
			Bottom, // -Y
			Front,  // +Z
			Back,   // -Z
		};

		virtual ~Texture() = default;

		static Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format, void* data, CPUAccess cpuAccess = CPUAccess::None, bool uav = false);
		static Ref<Texture> CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format, CPUAccess cpuAccess = CPUAccess::None, bool uav = false);
		static Ref<Texture> CreateTextureCube(const String paths[6]);
		static Ref<Texture> Create(const String& path);
		static Ref<Texture> Create(const String& path, bool isLinear);

		[[nodiscard]] virtual void* GetId() = 0;

		virtual void GenerateMipMaps() = 0;

		[[nodiscard]] const String& GetPath() const { return m_Path; }

		[[nodiscard]] virtual Ref<Texture> GetSlice(unsigned int slice) const = 0;

		[[nodiscard]] virtual uint16_t GetWidth() const = 0;
		[[nodiscard]] virtual uint16_t GetHeight() const = 0;
		[[nodiscard]] virtual uint16_t GetDepth() const = 0;

		[[nodiscard]] virtual bool IsTransparent() const = 0;

		void AttachSampler(const Ref<SamplerState>& sampler) { m_SamplerState = sampler; }
		[[nodiscard]] const Ref<SamplerState>& GetSamplerState() const { return m_SamplerState; }

		virtual void Resize(uint16_t width, uint16_t height = 0, uint16_t depth = 0) = 0;

		virtual void CopyFrom(Ref<Texture> sourceTexture) = 0;
		virtual Ref<Texture> Clone() = 0;

		virtual uint8_t GetBPP() const = 0;

		virtual void Clear(ClearFlags clearFlags = ClearFlags::All, const Vector4& color = Vector4::Zero, float depth = 1.0f, uint8_t stencil = 0) = 0;

		virtual void Bind(uint32_t slot, ShaderType type) const = 0;
		virtual void Unbind(uint32_t slot, ShaderType type) const = 0;

		[[nodiscard]] virtual bool IsLoaded() const = 0;

	protected:
		Texture() = default;
		explicit Texture(String path) : m_Path(Move(path)) { }

		String m_Path{};

		Ref<SamplerState> m_SamplerState;
	};
}
