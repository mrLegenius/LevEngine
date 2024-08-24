#pragma once
#include "Renderer/Pipeline/Viewport.h"
#include "Renderer/Pipeline/Rect.h"

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
        RasterizerState();

        virtual ~RasterizerState() = default;

        static Ref<RasterizerState> Create();

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        void SetFrontFaceFillMode(const FillMode fillMode = FillMode::Solid) { m_FrontFaceFillMode = fillMode; }
        [[nodiscard]] FillMode GetFrontFaceFillMode() const;

        void SetBackFaceFillMode(FillMode fillMode = FillMode::Solid);
        [[nodiscard]] FillMode GetBackFaceFillMode() const;

        void SetCullMode(CullMode cullMode = CullMode::Back);
        [[nodiscard]] CullMode GetCullMode() const;

        void SetFrontFacing(FrontFace frontFace = FrontFace::CounterClockwise);
        [[nodiscard]] FrontFace GetFrontFacing() const { return m_FrontFace; }
        
        void SetDepthClipEnabled(bool depthClipEnabled = true);
        [[nodiscard]] bool GetDepthClipEnabled() const { return m_DepthClipEnabled; }

        void SetViewport(const Viewport& viewport);
        void SetViewports(const Vector<Viewport>& viewports);
        [[nodiscard]] const Vector<Viewport>& GetViewports() const;
        
        void SetScissorEnabled(bool scissorEnable = false);
        [[nodiscard]] bool GetScissorEnabled() const;

        void SetScissorRect(const Rect& rect);
        void SetScissorRects(const Vector<Rect>& rects);
        [[nodiscard]] const Vector<Rect>& GetScissorRects() const;

        void SetMultisampleEnabled(bool multisampleEnabled);
        [[nodiscard]] bool GetMultisampleEnabled() const;

        void SetAntialiasedLineEnable(bool antialiasedLineEnabled);
        [[nodiscard]] bool GetAntialiasedLineEnable() const;

        void SetForcedSampleCount(uint8_t sampleCount);
        [[nodiscard]] uint8_t GetForcedSampleCount() const;

        //Only for DirectX 11.3 and 12
        void SetConservativeRasterizationEnabled(bool conservativeRasterizationEnabled);
        [[nodiscard]] bool GetConservativeRasterizationEnabled() const;

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
