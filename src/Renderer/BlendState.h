#pragma once

namespace LevEngine
{
    enum class BlendOperation // could also be called BlendOperand
    {
        Add,                // Add source and destination ( s + d ).
        Subtract,           // Subtract source from destination ( d - s )
        ReverseSubtract,    // Subtract destination from source ( s - s )
        Min,                // The minimum of source and destination ( min( s, d ) )
        Max                 // The maximum of source and destination ( max( s, d ) )
    };

    enum class BlendFactor
    {
        Zero,               // Multiply the pixel operand by 0 before applying the blend operation.
        One,                // Multiply the pixel operand by 1 before applying the blend operation.
        SrcColor,           // Multiply the pixel operand by the source color (Cs) before applying the blend operation.
        OneMinusSrcColor,   // Multiply the pixel operand by ( 1 - Cs ) before applying the blend operation.
        DstColor,           // Multiply the pixel operand by the destination color (Cd) before applying the blend operation.
        OneMinusDstColor,   // Multiply the pixel operand by ( 1 - Cd ) before applying the blend operation.
        SrcAlpha,           // Multiply the pixel operand by the source alpha ( As ) before applying the blend operation.
        OneMinusSrcAlpha,   // Multiply the pixel operand by ( 1 - As ) before applying the blend operation.
        DstAlpha,           // Multiply the pixel operand by the destination alpha ( Ad ) before applying the blend operation.
        OneMinusDstAlpha,   // Multiply the pixel operand by ( 1 - Ad ) before applying the blend operation.
        SrcAlphaSat,        // Multiply the pixel operand by min( As, 1 - Ad ) before applying the blend operation.
        ConstBlendFactor,   // Multiply the pixel operand by a constant blend factor before applying the blend operation. See @BlendState::SetConstBlendFactor.
        OneMinusBlendFactor,// Multiply the pixel operand by ( 1 - blend factor ) before applying the blend operation. See @BlendState::SetBlendFactor.
        Src1Color,          // Multiply the pixel operand by the color of the 2nd output ( Cs1 ) from the pixel shader before applying the blend operation. @see https://msdn.microsoft.com/en-us/library/windows/desktop/bb205120(v=vs.85).aspx#Blending @see https://www.opengl.org/sdk/docs/man/html/glBlendFunc.xhtml
        OneMinusSrc1Color,  // Multiply the pixel operand by ( 1 - Cs1 ) before applying the blend operation.
        Src1Alpha,          // Multiply the pixel operand by the alpha value of the 2nd output ( As1 ) from the pixel shader before applying the blend operation.
        OneMinusSrc1Alpha   // Multiply the pixel operand by ( 1 - As1 ) before applying the blend operation.
    };

    enum class LogicOperator
    {
        None,               // No logical operator is performed.
        Clear,              // Clears the render target (d = 0) at the current pixel.
        Set,                // Set the destination render target to 1 (d = 1) at the current pixel.
        Copy,               // Copy the source to the destination (d = s) at the current pixel.
        CopyInverted,       // Copy the inverted source to the destination (d = ~s) at the current pixel.
        Invert,             // Invert the color in the render target (d = ~d) at the current pixel.
        And,                // Logical And the source and destination values (d = s & d) at the current pixel.
        Nand,               // Logical !And the source and destination values (d = ~(s & d)) at the current pixel.
        Or,                 // Logical Or the source and destination values (d = s | d) at the current pixel.
        Nor,                // Logical !Or the source and destination values (d = ~( s | d) at the current pixel.
        Xor,                // Logical Xor the source and destination values (d = s ^ d) at the current pixel.
        Equiv,              // Logical !Xor at the source and destination values (d = ~(s ^ d) at the current pixel.
        AndReverse,         // Logical And the source and inverted destination values (d = s & ~d) at the current pixel.
        AndInverted,        // Logical And the inverted source  and destination values (d = ~s & d) at the current pixel.
        OrReverse,          // Logical Or the source and inverted destination values (d = s | ~d) at the current pixel.
        OrInverted          // Logical Or the inverted source and destination values (d = ~s | d) at the current pixel.
    };

    enum class BlendWrite
    {
        None = 0,
        Red = 1,
        Green = 2,
        Blue = 4,
        Alpha = 8,
        All = Red | Green | Blue | Alpha,
    };

    struct BlendMode
    {
        /**
         * Set to true to enable blending.
         * Default: false
         */
        bool BlendEnabled;

        /**
         * Set to true to enable the logical operator.
         * Default: false
         */
        bool LogicOpEnabled;

        /**
         * The blend factor to apply to the source color (S).
         * Default: BlendFactor::One
         */
        BlendFactor SrcFactor;
        /**
         * The blend factor to apply the the destination color (D).
         * Default: BlendFactor::Zero
         */
        BlendFactor DstFactor;

        /**
         * The blend operation to perform on the color components.
         * Default: BlendOperation::Add
         */
        BlendOperation BlendOp;

        /**
         * The blend factor to apply to the source alpha (As)
         * Default: BlendFactor::One
         */
        BlendFactor SrcAlphaFactor;

        /**
         * The blend factor to apply to the destination alpha (Ad)
         * Default: BlendFactor::Zero
         */
        BlendFactor DstAlphaFactor;

        /**
         * The blend operation to perform on the alpha components.
         * Default: BlendOperation::Add
         */
        BlendOperation AlphaOp;

        /**
         * The logical operator to perform at the current pixel.
         * Default: LogicOperator::None.
         */
        LogicOperator LogicOp;

        /**
         * Enable writing to the channels of the back buffer.
         * Default: true
         */
        BlendWrite Write;

    	explicit BlendMode(bool enabled = false,
            bool logicOpEnabled = false,
            BlendFactor srcFactor = BlendFactor::One,
            BlendFactor dstFactor = BlendFactor::Zero,
            BlendOperation blendOp = BlendOperation::Add,
            BlendFactor srcAlphaFactor = BlendFactor::One,
            BlendFactor dstAlphaFactor = BlendFactor::Zero,
            BlendOperation alphaOp = BlendOperation::Add,
            LogicOperator logicOp = LogicOperator::None,
            BlendWrite blendWrite = BlendWrite::All)
            : BlendEnabled(enabled)
            , LogicOpEnabled(logicOpEnabled)
            , SrcFactor(srcFactor)
            , DstFactor(dstFactor)
            , BlendOp(blendOp)
            , SrcAlphaFactor(srcAlphaFactor)
            , DstAlphaFactor(dstAlphaFactor)
            , AlphaOp(alphaOp)
            , LogicOp(logicOp)
            , Write(blendWrite)
        {}

        static BlendMode AlphaBlending;
        static BlendMode Additive;
    };

    class BlendState
    {
    public:
        BlendState()
        {
            m_BlendModes.resize(8, BlendMode());
        }

        virtual ~BlendState() = default;

        static Ref<BlendState> Create();

        void SetBlendMode(const BlendMode& blendMode)
        {
            m_BlendModes[0] = blendMode;
            m_Dirty = true;
        }
        // Use this method to set all blend modes at once. Only the first 8 are considered.
        void SetBlendModes(const std::vector<BlendMode>& blendModes)
        {
            m_BlendModes = blendModes;
            m_Dirty = true;
        }
        const std::vector<BlendMode>& GetBlendModes() const { return m_BlendModes; }

        void SetAlphaCoverage(const bool enabled)
        {
            if (m_AlphaToCoverageEnabled != enabled)
            {
                m_AlphaToCoverageEnabled = enabled;
                m_Dirty = true;
            }
        }

        bool GetAlphaCoverage() const
        {
            return m_AlphaToCoverageEnabled;
        }

        void SetIndependentBlend(const bool enabled)
        {
            if (m_AlphaToCoverageEnabled != enabled)
            {
                m_AlphaToCoverageEnabled = enabled;
                m_Dirty = true;
            }
        }

        bool GetIndependentBlend() const
        {
            return m_IndependentBlendEnabled;
        }

        void SetConstBlendFactor(const Vector4& constantBlendFactor)
        {
            m_ConstBlendFactor = constantBlendFactor;
            // No need to set the dirty flag as this value is not used to create the blend state object.
            // It is only used when activating the blend state of the output merger.
        }

        const Vector4& GetConstBlendFactor() const
        {
            return m_ConstBlendFactor;
        }

        void SetSampleMask(const uint32_t sampleMask)
        {
            m_SampleMask = sampleMask;
            // No need to set the dirty flag as this value is not used to create the blend state object.
            // It is only used when activating the blend state of the output merger.
        }

        uint32_t GetSampleMask() const
        {
            return m_SampleMask;
        }

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    protected:

        typedef std::vector<BlendMode> BlendModeList;

        BlendModeList m_BlendModes{};

        bool m_AlphaToCoverageEnabled = false;
        bool m_IndependentBlendEnabled = false;
        uint32_t m_SampleMask = 0xFFFFFFFF;

        Vector4 m_ConstBlendFactor = Vector4::One;

        bool m_Dirty = true;
    };

}
