#pragma once
#include "DataTypes/Vector.h"
#include "Renderer/Viewport.h"
#include "Renderer/Rect.h"

namespace LevEngine
{
    enum class FillMode
    {
		Wireframe,
        Solid
    };

    enum class CullMode
    {
        None,
        Front,
        Back,
        FrontAndBack    // OpenGL only
    };

    enum class FrontFace
    {
        Clockwise,
        CounterClockwise
    };

    class RasterizerState
    {
    public:
        RasterizerState()
        {
            m_Viewports.resize(8, Viewport());
            m_ScissorRects.resize(8, Rect());
        }

        virtual ~RasterizerState() = default;

        static Ref<RasterizerState> Create();

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        void SetFrontFaceFillMode(const FillMode fillMode = FillMode::Solid)
        {
            m_FrontFaceFillMode = fillMode;
        }

        [[nodiscard]] FillMode GetFrontFaceFillMode() const
        {
            return m_FrontFaceFillMode;
        }

        void SetBackFaceFillMode(const FillMode fillMode = FillMode::Solid)
        {
            m_BackFaceFillMode = fillMode;
        }

        [[nodiscard]] FillMode GetBackFaceFillMode() const
        {
            return m_BackFaceFillMode;
        }

        void SetCullMode(const CullMode cullMode = CullMode::Back)
        {
            m_CullMode = cullMode;
            m_StateDirty = true;
        }

        [[nodiscard]] CullMode GetCullMode() const { return m_CullMode; }

        void SetFrontFacing(const FrontFace frontFace = FrontFace::CounterClockwise)
        {
            m_FrontFace = frontFace;
            m_StateDirty = true;
        }

        [[nodiscard]] FrontFace GetFrontFacing() const { return m_FrontFace; }

        void SetDepthClipEnabled(bool depthClipEnabled = true)
        {
            m_DepthClipEnabled = depthClipEnabled;
            m_StateDirty = true;
        }

        [[nodiscard]] bool GetDepthClipEnabled() const { return m_DepthClipEnabled; }

        void SetViewport(const Viewport& viewport)
        {
            m_Viewports[0] = viewport;
            m_ViewportsDirty = true;
        }
        void SetViewports(const Vector<Viewport>& viewports)
        {
            m_Viewports = viewports;
            m_ViewportsDirty = true;
        }
        [[nodiscard]] const Vector<Viewport>& GetViewports() const
        {
            return m_Viewports;
        }

        void SetScissorEnabled(const bool scissorEnable = false)
        {
            m_ScissorEnabled = scissorEnable;
            m_StateDirty = true;
        }
        [[nodiscard]] bool GetScissorEnabled() const
        {
            return m_ScissorEnabled;
        }

        void SetScissorRect(const Rect& rect)
        {
            m_ScissorRects[0] = rect;
            m_ScissorRectsDirty = true;
        }
        void SetScissorRects(const Vector<Rect>& rects)
        {
            m_ScissorRects = rects;
            m_ScissorRectsDirty = true;
        }
        [[nodiscard]] const Vector<Rect>& GetScissorRects() const
        {
            return m_ScissorRects;
        }

        void SetMultisampleEnabled(bool multisampleEnabled)
        {
            m_MultisampleEnabled = multisampleEnabled;
            m_StateDirty = true;
        }

        bool GetMultisampleEnabled() const
        {
            return m_MultisampleEnabled;
        }

        void SetAntialiasedLineEnable(bool antialiasedLineEnabled)
        {
            m_AntialiasedLineEnabled = antialiasedLineEnabled;
            m_StateDirty = true;
        }

        bool GetAntialiasedLineEnable() const
        {
            return m_AntialiasedLineEnabled;
        }

        void SetForcedSampleCount(uint8_t sampleCount)
        {
            m_ForcedSampleCount = sampleCount;
            m_StateDirty = true;
        }

        uint8_t GetForcedSampleCount() const
        {
            return m_ForcedSampleCount;
        }

        void SetConservativeRasterizationEnabled(bool conservativeRasterizationEnabled)
        {
            m_ConservativeRasterization = conservativeRasterizationEnabled;
        }

        bool GetConservativeRasterizationEnabled() const
        {
            // Currently, this implementation always returns false
            // because conservative rasterization is supported since DirectX 11.3 and 12.
            return false;
        }


    protected:
        FillMode m_FrontFaceFillMode = FillMode::Solid;
        FillMode m_BackFaceFillMode = FillMode::Solid;

        CullMode m_CullMode = CullMode::Back;

        FrontFace m_FrontFace = FrontFace::CounterClockwise;

        float m_DepthBias{};
        float m_SlopeBias{};
        float m_BiasClamp{};

        bool m_DepthClipEnabled = true;
        bool m_ScissorEnabled = false;

        bool m_MultisampleEnabled = false;
        bool m_AntialiasedLineEnabled = false;

        bool m_ConservativeRasterization = false;

        uint8_t m_ForcedSampleCount = 0;

        typedef Vector<Rect> RectList;
        RectList m_ScissorRects;

        typedef Vector<Viewport> ViewportList;
        ViewportList m_Viewports;

        bool m_StateDirty = true;
        bool m_ViewportsDirty = true;
        bool m_ScissorRectsDirty = true;
	};
}
