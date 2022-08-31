#include "Mesh.h"

namespace LevEngine
{
    Ref<Mesh> Mesh::CreatePlane(int resolution)
    {
        auto mesh = CreateRef<Mesh>();

        float size = 1.f / (resolution - 1);

        for (int i = 0; i < resolution; i++)
        {
            for (int j = 0; j < resolution; j++)
            {
                auto pos = glm::vec3(i * size - 0.5f, j * size - 0.5f, 0);
                mesh->vertices.emplace_back(pos);

                auto normal = glm::vec3(0, 0, -1);
                mesh->normals.emplace_back(normal);

                auto uv = glm::vec2(i * size, j * size);
                mesh->uvs.emplace_back(uv);

                if (i == resolution - 1 || j == resolution - 1) continue;

                auto triangle1 = glm::vec3(resolution * i + j,
                                           resolution * i + j + resolution,
                                           resolution * i + j + resolution + 1);

                auto triangle2 = glm::vec3(resolution * i + j,
                                           resolution * i + j + resolution + 1,
                                           resolution * i + j + 1);

                mesh->triangles.emplace_back(triangle1);
                mesh->triangles.emplace_back(triangle2);
            }
        }

        return mesh;
    }

    Ref<Mesh> Mesh::CreateCube()
    {
        auto mesh = CreateRef<Mesh>();

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f, -1.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f, -1.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f, -1.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f, -1.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f,  1.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f,  1.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f,  1.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  0.0f,  1.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(-1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(-1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(-1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(-1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(1.0f,  0.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f, -1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f, -1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f, -1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f, -0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f, -1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f, -0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  1.0f));

        mesh->vertices.emplace_back(glm::vec3(0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(1.0f,  0.0f));

        mesh->vertices.emplace_back(glm::vec3(-0.5f,  0.5f,  0.5f));
        mesh->normals.emplace_back(glm::vec3(0.0f,  1.0f,  0.0f));
        mesh->uvs.emplace_back(glm::vec2(0.0f,  0.0f));

        mesh->triangles.emplace_back(glm::vec3(0, 2, 1));
        mesh->triangles.emplace_back(glm::vec3(0, 3, 2));
        mesh->triangles.emplace_back(glm::vec3(4, 5, 6));
        mesh->triangles.emplace_back(glm::vec3(4, 6, 7));
        mesh->triangles.emplace_back(glm::vec3(8, 9, 10));
        mesh->triangles.emplace_back(glm::vec3(8, 10, 11));
        mesh->triangles.emplace_back(glm::vec3(12, 15, 14));
        mesh->triangles.emplace_back(glm::vec3(12, 14, 13));
        mesh->triangles.emplace_back(glm::vec3(16, 17, 18));
        mesh->triangles.emplace_back(glm::vec3(16, 18, 19));
        mesh->triangles.emplace_back(glm::vec3(20, 23, 22));
        mesh->triangles.emplace_back(glm::vec3(20, 22, 21));

        return mesh;
}

    Ref<Mesh> Mesh::CreateSphere(const uint32_t numberSlices)
    {
        uint32_t numberParallels = numberSlices / 2;

        LEV_ASSERT(numberSlices >= 3u);

        float angleStep = 2 * glm::pi<float>() / numberSlices;

        auto mesh = CreateRef<Mesh>();

        for (int i = 0u; i < numberParallels + 1u; i++)
        {
            for (int j = 0u; j < numberSlices + 1u; j++)
            {
                float x = glm::sin(angleStep * (float)i) * glm::sin(angleStep * (float)j);
                float y = glm::cos(angleStep * (float)i);
                float z = glm::sin(angleStep * (float)i) * glm::cos(angleStep * (float)j);

                auto pos = glm::vec3(x, y, z);
                mesh->vertices.emplace_back(pos);

                auto normal = glm::vec3(x, y, z);
                mesh->normals.emplace_back(normal);

                auto uv = glm::vec2((float)j / numberSlices, (float)i / numberParallels);
                mesh->uvs.emplace_back(uv);

                if (i == numberParallels || j == numberSlices) continue;

                auto triangle1 = glm::vec3(i * (numberSlices + 1u) + j,
                                          (i + 1u) * (numberSlices + 1u) + j,
                                          (i + 1u) * (numberSlices + 1u) + (j + 1u));

                auto triangle2 = glm::vec3(i * (numberSlices + 1u) + j,
                                           (i + 1u) * (numberSlices + 1u) + (j + 1u),
                                           i * (numberSlices + 1u) + (j + 1u));

                mesh->triangles.emplace_back(triangle1);
                mesh->triangles.emplace_back(triangle2);
            }
        }

        return mesh;
    }
}