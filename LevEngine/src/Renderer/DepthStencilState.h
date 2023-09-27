#pragma once
namespace LevEngine
{
    enum class DepthWrite
    {
        Enable,
        Disable
    };

    enum class CompareFunction
    {
        Never,          // Never pass the comparison operation.
        Less,           // Pass if the source pixel's depth or stencil reference value is less than the value stored in the depth/stencil buffer ( DSs < DSd ).
        Equal,          // Pass if the source pixel's depth or stencil reference value is equal to the value stored in the depth/stencil buffer ( DSs == DSd ).
        LessOrEqual,    // Pass if the source pixel's depth or stencil reference value is less than or equal to the value stored in the depth/stencil buffer ( DSs <= DSd ).
        Greater,        // Pass if the source pixel's depth or stencil reference value is greater than the value stored in the depth/stencil buffer ( DSs > DSd ).
        NotEqual,       // Pass if the source pixel's depth or stencil reference value is not equal to teh value stored in the depth/stencil buffer ( DSs != DSd ).
        GreaterOrEqual, // Pass if the source pixel's depth or stencil reference value is greater than or equal to the value stored in the depth/stencil buffer ( DSs >= DSd ).
        Always          // Always pass the comparison operation.
    };

    enum class StencilOperation
    {
        Keep,           // Keep the existing value in the stencil buffer unmodified.
        Zero,           // Set the value in the stencil buffer to 0.
        Reference,      // Set the value in the stencil buffer to the reference value. @see DepthStencilState::StencilMode::StencilReference.
        IncrementClamp, // Increment the value in the stencil buffer by 1 and clamp the result if it is out of range.
        DecrementClamp, // Decrement the value in the stencil buffer by 1 and clamp the result if it is out of range.
        Invert,         // Invert the value in the stencil value.
        IncrementWrap,  // Increment the value in the stencil buffer by 1 and wrap the result if it is out of range.
        DecrementWrap,  // Decrement the value in the stencil buffer by 1 and wrap the result if it is out of range.
    };

    struct DepthMode
    {
        /**
         * Set to true to enable depth testing.
         * If enabled, the DepthFunction will be used to check if a pixel should
         * be written to the back buffer or discarded.
         * Default: True
         * @see DepthStencilState::DepthMode::DepthFunction
         */
        bool DepthEnable = true;

        /**
         * Enable or disable writing to the depth buffer.
         * This should be enabled for opaque geometry and disabled
         * for partially transparent objects.
         * Default: Enabled.
         */
        DepthWrite DepthWriteMask = DepthWrite::Enable;

        /**
         * If the depth comparison function evaluates to TRUE then the pixel
         * is rasterized. If the depth comparison function evaluates to FALSE then
         * the pixel is discarded and not written to the back buffer.
         * The default value is CompareFunc::Less which means that source pixels
         * that are closer to the camera will pass the compare function and source
         * pixels that appear further away from the camera are discarded.
         */
        CompareFunction DepthFunction = CompareFunction::Less;

        explicit DepthMode(const bool depthEnable = true,
            const DepthWrite depthWrite = DepthWrite::Enable,
            const CompareFunction depthFunction = CompareFunction::Less)
            : DepthEnable(depthEnable)
            , DepthWriteMask(depthWrite)
            , DepthFunction(depthFunction)
        {}
    };

    struct FaceOperation
    {
        /**
         * The operation to perform on the value in the stencil buffer if the
         * stencil comparision function (specified by FaceOperation::StencilFunction)
         * returns FALSE.
         * Default: StencilOperation::Keep
         */
        StencilOperation StencilFail = StencilOperation::Keep;

        /**
         * The operation to perform on the value in the stencil buffer if the
         * stencil comparision function (specified by FaceOperation::StencilFunction)
         * returns TRUE and depth comparision function (determined by DepthMode::DepthFunction)
         * returns FALSE.
         * Default: StencilOperation::Keep
         */
        StencilOperation StencilPassDepthFail = StencilOperation::Keep;

        /**
         * The operation to perform if both depth comparison function
         * (determined by DepthMode::DepthFunction) and stencil comparision
         * function (specified by FaceOperation::StencilFunction) returns TRUE.
         * Default: StencilOperation::Keep
         */
        StencilOperation StencilDepthPass = StencilOperation::Keep;

        /**
         * The the comparison method to use for stencil operations.
         * Default: CompareFunction::Always (Tests always passes).
         * @see DepthStencilState::CompareFunction
         */
        CompareFunction StencilFunction = CompareFunction::Always;

        explicit FaceOperation(const StencilOperation stencilFail = StencilOperation::Keep,
            const StencilOperation stencilPassDepthFail = StencilOperation::Keep,
            const StencilOperation stencilDepthPass = StencilOperation::Keep,
            const CompareFunction stencilFunction = CompareFunction::Always)
            : StencilFail(stencilFail)
            , StencilPassDepthFail(stencilPassDepthFail)
            , StencilDepthPass(stencilDepthPass)
            , StencilFunction(stencilFunction)
        {}
    };

    struct StencilMode
    {
        /**
         * Set to true to enable stencil testing.
         * Default: false.
         */
        bool StencilEnabled = false;

        /**
         * A mask that is AND'd to the value in the stencil buffer before it is read.
         * Default: 0xff
         */
        uint8_t ReadMask = 0xff;

        /**
         * A mask that is AND'd to the value in the stencil buffer before it is written.
         * Default: 0xff
         */
        uint8_t WriteMask = 0xff;

        /**
         * The value to set the stencil buffer to if any of the StencilOperation
         * members of the FaceOperation struct is set to StencilOperation::Reference.
         * Default: 0
         */
        uint32_t StencilReference = 0;

        /**
         * The compare function and pass/fail operations to perform on the stencil
         * buffer for front-facing polygons.
         */
        FaceOperation FrontFace{};

        /**
         * The compare function and pass/fail operations to perform on the stencil
         * buffer for back-facing polygons.
         */
        FaceOperation BackFace{};

        StencilMode(bool stencilEnabled = false,
            uint8_t readMask = 0xff,
            uint8_t writeMask = 0xff,
            uint32_t stencilReference = 0,
            FaceOperation frontFace = FaceOperation(),
            FaceOperation backFace = FaceOperation())
            : StencilEnabled(stencilEnabled)
            , ReadMask(readMask)
            , WriteMask(writeMask)
            , StencilReference(stencilReference)
            , FrontFace(frontFace)
            , BackFace(backFace)
        {}
    };

class DepthStencilState
{
public:
    virtual ~DepthStencilState() = default;

    static Ref<DepthStencilState> Create();

    void SetDepthMode(const DepthMode& depthMode);
    [[nodiscard]] const DepthMode& GetDepthMode() const { return m_DepthMode; }

    void SetStencilMode(const StencilMode& stencilMode);
    [[nodiscard]] const StencilMode& GetStencilMode() const { return m_StencilMode; }

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

protected:
    DepthMode m_DepthMode;
    StencilMode m_StencilMode;

    bool m_Dirty = true;
};
}
