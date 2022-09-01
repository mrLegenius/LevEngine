#pragma once

#include "Renderer/Buffer.h"

namespace LevEngine
{
    class Mesh
    {
    public:
        static Ref<Mesh> CreatePlane(int resolution);
        static Ref<Mesh> CreateSphere(const uint32_t numberSlices);
        static Ref<Mesh> CreateCube();

        Ref<IndexBuffer> CreateIndexBuffer();

        Ref<VertexBuffer> CreateVertexBuffer(BufferLayout);

        void Clear()
        {
            vertices.clear();
            uvs.clear();
            triangles.clear();
            normals.clear();
        }

        [[nodiscard]] uint32_t GetVerticesCount() { return vertices.size(); }
        [[nodiscard]] glm::vec3 GetVertex(uint32_t index) { return vertices[index]; }

        [[nodiscard]] uint32_t GetTrianglesCount() { return triangles.size(); }
        [[nodiscard]] glm::vec3 GetTriangle(uint32_t index) { return triangles[index]; }

        [[nodiscard]] glm::vec2 GetUV(uint32_t index) { return uvs[index]; }
        [[nodiscard]] glm::vec3 GetNormal(uint32_t index) { return normals[index]; }

    private:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> triangles;
        std::vector<glm::vec3> normals;
    };
}

