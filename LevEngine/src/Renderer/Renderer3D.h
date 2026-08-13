#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "3D/AnimationConstants.h"
#include "DataTypes/Array.h"

namespace LevEngine
{
    class Shader;
    class ConstantBuffer;
    struct MeshRendererComponent;
    class SceneCamera;
    class Mesh;
    class Material;

    class StructuredBuffer;

    struct LEV_API MeshModelBufferData
    {
        Matrix Model;
        Matrix TransposedInvertedModel;
    	Array<Matrix, AnimationConstants::MaxBoneCount> FinalBoneMatrices;
    };

    // One entry per instance of an instanced draw. Mirrors the InstanceData struct that
    // ShaderCommon.hlsl reads out of the structured buffer at RenderSettings::InstanceDataSlot.
    struct LEV_API MeshInstanceData
    {
        Matrix Model;
        Matrix TransposedInvertedModel;
    };

    class LEV_API Renderer3D
    {
    public:
        static void Init();

        //Releases the buffers, the primitive meshes and the fallback material. Has to be called
        //while the render device is still alive, see TextureLibrary::Shutdown
        static void Shutdown();

        static void SetCameraBuffer(const SceneCamera* camera, const Matrix& viewMatrix, const Vector3& position);
        static void DrawMesh(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader);
    	static void DrawMesh(const Matrix& model, const Array<Matrix, AnimationConstants::MaxBoneCount>& finalBoneMatrices,
			const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawCube(const Ref<Shader>& vertexShader);

        // Draws the same mesh once per entry in instances with a single draw call. The shader
        // must have been compiled with WITH_INSTANCING -- it reads its model matrices out of the
        // instance buffer instead of the per-draw model constant buffer.
        static void DrawMeshInstanced(const Vector<MeshInstanceData>& instances, const Ref<Mesh>& mesh,
            const Ref<Shader>& shader);

        static void DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        static void DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
        
        static void RenderSphere(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCone(const Matrix& model, const Ref<Shader>& shader);
        static void RenderCube(const Matrix& model, const Ref<Shader>& shader);

        static Ref<Material> MissingMaterial;

    private:
        static void EnsureInstanceBufferCapacity(uint32_t count);

        static Ref<StructuredBuffer> m_InstanceBuffer;
        static uint32_t m_InstanceBufferCapacity;

        static Ref<ConstantBuffer> m_ModelConstantBuffer;
        static Ref<ConstantBuffer> m_CameraConstantBuffer;
        static Ref<ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

        //<--- Lazily created on the first draw and kept as members instead of function local
        //statics, so Shutdown can release them ---<<
        static Ref<Mesh> m_CubeMesh;
        static Ref<Mesh> m_SphereMesh;
        static Ref<Mesh> m_ConeMesh;

        static Matrix m_ViewProjection;
    };
}
