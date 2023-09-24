#include "levpch.h"
#include "DebugRender.h"

#include "DebugCube.h"
#include "DebugLine.h"
#include "DebugSphere.h"
#include "DebugWireCube.h"
#include "DebugWireSphere.h"

namespace LevEngine
{
    void DebugRender::DrawCube(const Vector3 position, const Vector3 size, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(size)
            * Matrix::CreateTranslation(position);

        const auto cube = CreateRef<DebugCube>(model, color);

        m_Shapes.emplace(cube);
    }

    void DebugRender::DrawWireCube(const Vector3 position, const Vector3 size, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(size)
            * Matrix::CreateTranslation(position);

        const auto cube = CreateRef<DebugWireCube>(model, color);

        m_Shapes.emplace(cube);
    }

    void DebugRender::DrawSphere(const Vector3 position, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugSphere>(model, color);

        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawWireSphere(const Vector3 position, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugWireSphere>(model, color);

        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawLine(Vector3 start, Vector3 end, const Color color)
    {
        const auto line = CreateRef<DebugLine>(start, end, color);

        m_Shapes.emplace(line);
    }

    void DebugRender::DrawRay(const Vector3 origin, const Vector3 direction, const Color color)
    {
        DrawLine(origin, origin + direction, color);
    }

    void DebugRender::Update(float deltaTime)
    {
        static auto debugBuffer = ConstantBuffer::Create(sizeof Color);
        
        ShaderAssets::Debug()->Bind();
        while (!m_Shapes.empty())
        {
            const auto& gizmo = m_Shapes.front();
            debugBuffer->SetData(&gizmo->GetColor());
            debugBuffer->Bind(2, ShaderType::Pixel);
            gizmo->Draw();
            m_Shapes.pop();
        }
        ShaderAssets::Debug()->Unbind();
    }
}
