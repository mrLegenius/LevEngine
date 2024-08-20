#pragma once

namespace LevEngine
{
    enum class ShaderType;

    class SamplerState
	{
	public:
        enum class Filter
        {
            // The nearest texel to the sampled texel.
            Nearest,
            // Min - Linear average of the 4 closest texels.
            // Mag - Weighted average of the closest texels.
            // Mip - Linear interpolate between the 2 nearest mip map levels.
            Linear,                 
        };

        enum class WrapMode
        {
            Repeat,                 // Texture is repeated when texture coordinates are out of range.
            Mirror,                 // Texture is mirrored when texture coordinates are out of range.
            Clamp,                  // Texture coordinate is clamped to [0, 1] 
            Border,                 // Texture border color is used when texture coordinates are out of range.
        };

        enum class CompareMode
        {
            None,                   // Don't perform any comparison
            CompareRefToTexture,    // Compare the reference value (usually the currently bound depth buffer) to the value in the texture.
        };

        enum class CompareFunc
        {
            Never,                  // Never pass the comparison function.
            Less,                   // Pass if the source data is less than the destination data.
            Equal,                  // Pass if the source data is equal to the destination data.
            LessOrEqual,            // Pass if the source data is less than or equal to the destination data.
            Greater,                // Pass if the source data is greater than the destination data.
            NotEqual,               // Pass if the source data is not equal to the destination data.
            GreaterOrEqual,         // Pass if the source data is greater than or equal to the destination data.
            Always,                 // Always pass the comparison function.
        };

        virtual ~SamplerState() = default;

        static Ref<SamplerState> Create();

        // Bind this sampler state to the ID for a specific shader type.
        virtual void Bind(uint32_t slot, ShaderType shaderType) = 0;
        // Unbind the sampler state.
        virtual void Unbind(uint32_t slot, ShaderType shaderType) = 0;

        void SetFilter(Filter minFilter, Filter magFilter, Filter mipFilter);
        void GetFilter(Filter& minFilter, Filter& magFilter, Filter& mipFilter) const;

        void SetWrapMode(WrapMode u = WrapMode::Repeat, WrapMode v = WrapMode::Repeat, WrapMode w = WrapMode::Repeat);
        void GetWrapMode(WrapMode& u, WrapMode& v, WrapMode& w) const;

        void SetCompareMode(CompareMode compareMode);
        [[nodiscard]] CompareMode GetCompareMode() const;

        void SetCompareFunction(CompareFunc compareFunc);
        [[nodiscard]] CompareFunc GetCompareFunction() const;

        /**
		* Set the offset from the calculated mipmap level.  For example, if mipmap texture 1 should be sampled and
		* LOD bias is set to 2, then the texture will be sampled at mipmap level 3.
		*/
        void SetLODBias(float lodBias);
        [[nodiscard]] float GetLODBias() const;

        /**
         * Set the minimum LOD level that will be sampled.  The highest resolution mip map is level 0.
         */
        void SetMinLOD(float minLOD);
        [[nodiscard]] float GetMinLOD() const;

        /**
         * Set the maximum LOD level that will be sampled. The LOD level increases as the resolution of the mip-map decreases.
         */
        void SetMaxLOD(float maxLOD);
        [[nodiscard]] float GetMaxLOD() const;

        /**
         * Sets the border color to use if the wrap mode is set to Border.
         */
        void SetBorderColor(const Color& borderColor);
        [[nodiscard]] Color GetBorderColor() const;

        /**
         * Enable Anisotropic filtering (where supported).
         */
        void EnableAnisotropicFiltering(bool enabled);
        [[nodiscard]] bool IsAnisotropicFilteringEnabled() const;

        /**
         * When Anisotropic filtering is enabled, use this value to determine the maximum level
         * of anisotropic filtering to apply.  Valid values are in the range [1, 16].
         * May not be supported on all platforms.
         */
        void SetMaxAnisotropy(uint8_t maxAnisotropy);
        [[nodiscard]] uint8_t GetMaxAnisotropy() const;

    protected:
        Filter m_MinFilter = Filter::Linear;
        Filter m_MagFilter = Filter::Linear;
        Filter m_MipFilter = Filter::Linear;
        WrapMode  m_WrapModeU{}, m_WrapModeV{}, m_WrapModeW{};
        CompareMode m_CompareMode{};
        CompareFunc m_CompareFunc{};

        float m_LODBias = 0;
        float m_MinLOD = 0;
        float m_MaxLOD = std::numeric_limits<float>::max();

        Color m_BorderColor{};

        bool m_IsAnisotropicFilteringEnabled = false;
        uint8_t m_AnisotropicFiltering = 1;

        // Set to true if the sampler state needs to be recreated.
        bool m_IsDirty{};
	};
}
