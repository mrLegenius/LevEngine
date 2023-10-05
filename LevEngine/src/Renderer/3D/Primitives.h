#pragma once
#include "Math/Vector3.h"

namespace LevEngine
{
    class Mesh;
    class Primitives
    {
    public:
        static Ref<Mesh> CreatePlane(uint32_t resolution);
        static Ref<Mesh> CreateWireCube();
        static Ref<Mesh> CreateCube();
        static Ref<Mesh> CreateLine(DirectX::SimpleMath::Vector3 start, Vector3 end);
        static Ref<Mesh> CreateSphere(uint32_t sliceCount);
        static Ref<Mesh> CreateRing(Vector3 majorAxis, Vector3 minorAxis);
        static Ref<Mesh> CreateGrid(Vector3 xAxis, Vector3 yAxis, uint32_t xDivisions, uint32_t yDivisions);
    };
}
