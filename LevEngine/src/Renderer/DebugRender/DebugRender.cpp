#include "levpch.h"
#include "DebugRender.h"

#include "DebugCircle.h"
#include "DebugCube.h"
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

    void DebugRender::DrawCube(const Vector3 position, const Vector3 size, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawCube(position, size, color); }, duration);
    }

    void DebugRender::DrawWireCube(const Vector3 position, const Vector3 size, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(size)
            * Matrix::CreateTranslation(position);

        const auto cube = CreateRef<DebugWireCube>(model, color);

        m_Shapes.emplace(cube);
    }

    void DebugRender::DrawWireCube(const Vector3 position, const Vector3 size, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireCube(position, size, color); }, duration);
    }

    void DebugRender::DrawWireCube(const Vector3 position, const Quaternion rotation, const Vector3 size, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(size)
            * Matrix::CreateFromQuaternion(rotation)
            * Matrix::CreateTranslation(position);

        DrawWireCube(model, color);
    }
    
    void DebugRender::DrawWireCube(const Vector3 position, const Quaternion rotation, const Vector3 size, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireCube(position, rotation, size, color); }, duration);
    }

    void DebugRender::DrawWireCube(const Matrix& model, const Color color)
    {
        const auto cube = CreateRef<DebugWireCube>(model, color);
        
        m_Shapes.emplace(cube);
    }

    void DebugRender::DrawWireCube(const Matrix& model, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireCube(model, color); }, duration);
    }

    void DebugRender::DrawSphere(const Vector3 position, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugSphere>(model, color);

        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawSphere(const Vector3 position, const float radius, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawSphere(position, radius, color); }, duration);
    }

    void DebugRender::DrawPoint(const Vector3 position, const Color color)
    {
        DrawSphere(position, 0.1f, color);
    }

    void DebugRender::DrawPoint(const Vector3 position, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawPoint(position, color); }, duration);
    }

    void DebugRender::DrawPointStar(const Vector3 position, const Color color)
    {
        constexpr float size = 0.1f;
        DrawLine(position + Vector3{-size, 0, 0}, position + Vector3{size, 0, 0}, color);
        DrawLine(position + Vector3{0, -size, 0}, position + Vector3{0, size, 0}, color);
        DrawLine(position + Vector3{0, 0, -size}, position + Vector3{0, 0, size}, color);
    }

    void DebugRender::DrawPointStar(const Vector3 position, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawPointStar(position, color); }, duration);
    }

    void DebugRender::DrawWireSphere(const Vector3 position, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugWireSphere>(model, color);

        m_Shapes.emplace(shape);
    }
    void DebugRender::DrawWireSphere(const Vector3 position, const float radius, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireSphere(position, radius, color); }, duration);
    }

    void DebugRender::DrawWireSphere(const Matrix& model, const Color color)
    {
        const auto shape = CreateRef<DebugWireSphere>(model, color);

        m_Shapes.emplace(shape);
    }
    void DebugRender::DrawWireSphere(const Matrix& model, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireSphere(model, color); }, duration);
    }

    void DebugRender::DrawWireCapsule(const Vector3 position, const Quaternion rotation, const float halfHeight, const float radius, const Color color)
    {
        const Matrix model =
            Matrix::CreateFromQuaternion(rotation)
            * Matrix::CreateTranslation(position);

        DrawWireCapsule(model, halfHeight, radius, color);
    }

    void DebugRender::DrawWireCapsule(const Vector3 position, const Quaternion rotation, const float halfHeight, const float radius, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireCapsule(position, rotation, halfHeight, radius, color); }, duration);
    }

    void DebugRender::DrawWireCapsule(const Matrix& model, const float halfHeight, const float radius, const Color color)
    {
        DrawLine(Vector3::Transform(Vector3(-halfHeight, radius, 0.0f), model), Vector3::Transform(Vector3(halfHeight, radius, 0.0f), model), color);
        DrawLine(Vector3::Transform(Vector3(-halfHeight, 0.0f, radius), model), Vector3::Transform(Vector3(halfHeight, 0.0f, radius), model), color);
        DrawLine(Vector3::Transform(Vector3(-halfHeight, -radius, 0.0f), model), Vector3::Transform(Vector3(halfHeight, -radius, 0.0f), model), color);
        DrawLine(Vector3::Transform(Vector3(-halfHeight, 0.0f, -radius), model), Vector3::Transform(Vector3(halfHeight, 0.0f, -radius), model), color);
        
        const auto sphere0 = CreateRef<DebugWireSphere>(Matrix::CreateScale(radius) * Matrix::CreateTranslation(Vector3(halfHeight, 0.0f, 0.0f)) * model, color);
        const auto sphere1 = CreateRef<DebugWireSphere>(Matrix::CreateScale(radius) * Matrix::CreateTranslation(Vector3(-halfHeight, 0.0f, 0.0f)) * model, color);
        
        m_Shapes.emplace(sphere0);
        m_Shapes.emplace(sphere1);
    }

    void DebugRender::DrawWireCapsule(const Matrix& model, const float halfHeight, const float radius, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawWireCapsule(model, halfHeight, radius, color); }, duration);
    }

    void DebugRender::DrawLine(const Vector3 start, const Vector3 end, const Color color)
    {
        const auto baseIndex = m_Vertices.size();
        m_Indices.push_back(baseIndex);
        m_Indices.push_back(baseIndex + 1);

        m_Vertices.push_back(start);
        m_Vertices.push_back(end);

        m_Colors.push_back(color);
        m_Colors.push_back(color);
    }

    void DebugRender::DrawLine(const Vector3 start, const Vector3 end, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawLine(start, end, color); }, duration);
    }

    void DebugRender::DrawRay(const Vector3 origin, const Vector3 direction, const Color color)
    {
        DrawLine(origin, origin + direction, color);
    }

    void DebugRender::DrawRay(const Vector3 origin, const Vector3 direction, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawRay(origin, direction, color); }, duration);
    }

    void DebugRender::DrawCircle(const Vector3 position, const float radius, const Quaternion rotation, const Color color)
    {
        const Matrix model =
            Matrix::CreateFromQuaternion(rotation)
            * Matrix::CreateScale(radius)
            * Matrix::CreateTranslation(position);

        const auto shape = CreateRef<DebugCircle>(model, color);
        
        m_Shapes.emplace(shape);
    }

    void DebugRender::DrawCircle(const Vector3 position, const float radius, const Quaternion rotation, const Color color, const float duration)
    {
        m_TemporaryShapes.emplace_back([=]{ DrawCircle(position, radius, rotation, color); }, duration);
    }

    void DebugRender::DrawGrid(const Vector3 position,
                               const Vector3 xAxis, const Vector3 yAxis,
                               const uint32_t xCells, const uint32_t yCells,
                               const float cellSize,
                               const Color color)
    {
        const Vector3 scale = static_cast<float>(xCells) * cellSize * xAxis + static_cast<float>(yCells) * cellSize * yAxis;
        
        const Matrix model =
            Matrix::CreateScale(scale)
            * Matrix::CreateTranslation(position);
        
        const auto grid = CreateRef<DebugGrid>(model, xAxis, yAxis, xCells, yCells, color);

        m_Shapes.emplace(grid);
    }
}