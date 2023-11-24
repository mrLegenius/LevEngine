#include "levpch.h"
#include "DebugRender.h"

#include "DebugCircle.h"
#include "DebugCube.h"
#include "DebugLine.h"
#include "DebugSphere.h"
#include "DebugWireCube.h"
#include "DebugWireSphere.h"
#include "DebugGrid.h"

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
    
    void DebugRender::DrawWireCube(const Matrix& model, const Color color)
    {
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

    void DebugRender::DrawPoint(const Vector3 position, const Color color)
    {
        DrawSphere(position, 0.1f, color);
    }

    void DebugRender::DrawPointStar(const Vector3 position, const Color color)
    {
        constexpr float size = 0.1f;
        DrawLine(position + Vector3{-size, 0, 0}, position + Vector3{size, 0, 0}, color);
        DrawLine(position + Vector3{0, -size, 0}, position + Vector3{0, size, 0}, color);
        DrawLine(position + Vector3{0, 0, -size}, position + Vector3{0, 0, size}, color);
    }

    void DebugRender::DrawWireSphere(const Vector3 position, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugWireSphere>(model, color);

        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawWireSphere(const Matrix& model, const Color color)
    {
        const auto shape = CreateRef<DebugWireSphere>(model, color);

        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawWireCapsule(const Matrix& model, float halfHeight, float radius, Color color)
    {
        const auto line0 = CreateRef<DebugLine>(Vector3::Transform(Vector3(-halfHeight, radius, 0.0f), model), Vector3::Transform(Vector3(halfHeight, radius, 0.0f), model), color);
        const auto line1 = CreateRef<DebugLine>(Vector3::Transform(Vector3(-halfHeight, 0.0f, radius), model), Vector3::Transform(Vector3(halfHeight, 0.0f, radius), model), color);
        const auto line2 = CreateRef<DebugLine>(Vector3::Transform(Vector3(-halfHeight, -radius, 0.0f), model), Vector3::Transform(Vector3(halfHeight, -radius, 0.0f), model), color);
        const auto line3 = CreateRef<DebugLine>(Vector3::Transform(Vector3(-halfHeight, 0.0f, -radius), model), Vector3::Transform(Vector3(halfHeight, 0.0f, -radius), model), color);
        
        m_Shapes.emplace(line0);
        m_Shapes.emplace(line1);
        m_Shapes.emplace(line2);
        m_Shapes.emplace(line3);
        
        const auto sphere0 = CreateRef<DebugWireSphere>(Matrix::CreateScale(radius) * Matrix::CreateTranslation(Vector3(halfHeight, 0.0f, 0.0f)) * model, color);
        const auto sphere1 = CreateRef<DebugWireSphere>(Matrix::CreateScale(radius) * Matrix::CreateTranslation(Vector3(-halfHeight, 0.0f, 0.0f)) * model, color);
        
        m_Shapes.emplace(sphere0);
        m_Shapes.emplace(sphere1);
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

    void DebugRender::DrawCircle(const Vector3 position, const float radius, const Quaternion rotation, Color color)
    {
        const Matrix model =
            Matrix::CreateFromQuaternion(rotation)
            * Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugCircle>(model, color);
        
        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawGrid(const Vector3 position,
                               Vector3 xAxis, Vector3 yAxis,
                               uint32_t xCells, uint32_t yCells,
                               const float cellSize,
                               Color color)
    {
        const Vector3 scale = static_cast<float>(xCells) * cellSize * xAxis + static_cast<float>(yCells) * cellSize * yAxis;
        
        const Matrix model =
            Matrix::CreateScale(scale)
            * Matrix::CreateTranslation(position);
        
        const auto grid = CreateRef<DebugGrid>(model, xAxis, yAxis, xCells, yCells, color);

        m_Shapes.emplace(grid);
    }
}
