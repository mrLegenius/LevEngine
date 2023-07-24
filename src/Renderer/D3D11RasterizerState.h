#pragma once
#include "d3d11.h"
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

class D3D11RasterizerState
{
public:

    ~D3D11RasterizerState();

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

    void Bind();
    void Unbind();

private:
    ID3D11RasterizerState* m_RasterizerState = nullptr;

    FillMode m_FrontFaceFillMode = FillMode::Solid;
    FillMode m_BackFaceFillMode = FillMode::Solid;

    CullMode m_CullMode = CullMode::Back;

    FrontFace m_FrontFace = FrontFace::CounterClockwise;

    bool m_DepthClipEnabled = true;

    bool m_StateDirty = true;
};
}