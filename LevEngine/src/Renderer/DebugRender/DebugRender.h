#pragma once

namespace LevEngine
{
    class DebugShape;

    struct TemporaryShape
    {
        Action<> DrawCallback;
        float TimeLeft;
    };

    class DebugRender
    {
    public:
        static void DrawCube(Vector3 position, Vector3 size, Color color);
        static void DrawCube(Vector3 position, Vector3 size, Color color, float duration);
        
        static void DrawWireCube(Vector3 position, Vector3 size, Color color);
        static void DrawWireCube(Vector3 position, Vector3 size, Color color, float duration);
        static void DrawWireCube(Vector3 position, Quaternion rotation, Vector3 size, Color color);
        static void DrawWireCube(Vector3 position, Quaternion rotation, Vector3 size, Color color, float duration);
        static void DrawWireCube(const Matrix& model, Color color);
        static void DrawWireCube(const Matrix& model, Color color, float duration);
        
        static void DrawSphere(Vector3 position, float radius, Color color);
        static void DrawSphere(Vector3 position, float radius, Color color, float duration);
        
        static void DrawWireSphere(Vector3 position, float radius, Color color);
        static void DrawWireSphere(Vector3 position, float radius, Color color, float duration);
        static void DrawWireSphere(const Matrix& model, Color color);
        static void DrawWireSphere(const Matrix& model, Color color, float duration);

        static void DrawWireCapsule(Vector3 position, Quaternion rotation, float halfHeight, float radius, Color color);
        static void DrawWireCapsule(Vector3 position, Quaternion rotation, float halfHeight, float radius, Color color, float duration);
        static void DrawWireCapsule(const Matrix& model, float halfHeight, float radius, Color color);
        static void DrawWireCapsule(const Matrix& model, float halfHeight, float radius, Color color, float duration);
        
        static void DrawPoint(Vector3 position, Color color);
        static void DrawPoint(Vector3 position, Color color, float duration);
        
        static void DrawPointStar(Vector3 position, Color color);
        static void DrawPointStar(Vector3 position, Color color, float duration);
        
        static void DrawLine(Vector3 start, Vector3 end, Color color);
        static void DrawLine(Vector3 start, Vector3 end, Color color, float duration);
        
        static void DrawRay(Vector3 origin, Vector3 direction, Color color);
        static void DrawRay(Vector3 origin, Vector3 direction, Color color, float duration);
        
        static void DrawCircle(Vector3 position, float radius, Quaternion rotation, Color color);
        static void DrawCircle(Vector3 position, float radius, Quaternion rotation, Color color, float duration);
        
        static void DrawGrid(Vector3 position, Vector3 xAxis, Vector3 yAxis, uint32_t xCells, uint32_t yCells, float cellSize, Color color);

        friend class DebugRenderPass;
        
    private:
        static Queue<Ref<DebugShape>>& GetRequestedShapes() { return m_Shapes; }
        
        static Vector<Vector3>& GetVertices() { return m_Vertices; }
        static Vector<uint32_t>& GetIndices() { return m_Indices; }
        static Vector<Color>& GetColors() { return m_Colors; }
        static void Reset()
        {
            m_Vertices.clear();
            m_Indices.clear();
            m_Colors.clear();
        }
        
        inline static Queue<Ref<DebugShape>> m_Shapes;
        inline static Vector<Vector3> m_Vertices;
        inline static Vector<Color> m_Colors;
        inline static Vector<uint32_t> m_Indices;
        inline static Vector<TemporaryShape> m_TemporaryShapes;
    };
}