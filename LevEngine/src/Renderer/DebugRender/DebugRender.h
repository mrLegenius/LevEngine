#pragma once
#include "DebugShape.h"
#include "Renderer/Renderer3D.h"
#include "DataTypes/Queue.h"

namespace LevEngine
{
    class DebugRender
    {
    public:
        static void DrawCube(Vector3 position, Vector3 size, Color color);
        static void DrawWireCube(Vector3 position, Vector3 size, Color color);
        static void DrawSphere(Vector3 position, float radius, Color color);
        static void DrawWireSphere(Vector3 position, float radius, Color color);
        static void DrawLine(Vector3 start, Vector3 end, Color color);
        static void DrawRay(Vector3 origin, Vector3 direction, Color color);

        static Queue<Ref<DebugShape>>& GetRequestedShapes() { return m_Shapes; }
    private:
        inline static Queue<Ref<DebugShape>> m_Shapes;
    };
}
