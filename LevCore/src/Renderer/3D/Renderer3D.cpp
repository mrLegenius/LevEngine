#include "Renderer3D.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Camera/EditorCamera.h"
#include "Scene/Components.h"
#include "Renderer/VertexArray.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/StorageBuffer.h"

namespace LevEngine
{
    struct SkyboxVertex
    {
        glm::vec3 Position;

        // Editor-only
        int EntityID;
    };
    struct MeshVertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        float TexIndex;
        float TexTiling;

        // Editor-only
        int EntityID;
    };

    struct Renderer3DData
    {
        Ref<Shader> SkyboxShader;
        Ref<Shader> MeshShader;

        //Renderer3D::Statistics stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
            glm::vec3 Position;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;

        struct DirLightData
        {
            //We need extra offset for vec3 due OpenGL buffer memory specifics
            glm::vec3 Direction; float _0;
            glm::vec3 Ambient; float _1;
            glm::vec3 Diffuse; float _2;
            glm::vec3 Specular; float _3;
        };

        struct LightsData
        {
            DirLightData DirLight;
        };

        LightsData LightsBuffer;
        Ref<UniformBuffer> LightsStorageBuffer;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init()
    {
        s_Data.SkyboxShader = Shader::Create("assets/shaders/Skybox.shader");
        s_Data.MeshShader = Shader::Create("assets/shaders/Mesh_Lit.shader"); //TODO: Move it to material
        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
        s_Data.LightsStorageBuffer = UniformBuffer::Create(sizeof(Renderer3DData::LightsData), 1);
    }

    void Renderer3D::Shutdown()
    {
        LEV_PROFILE_FUNCTION()
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& position)
    {
        LEV_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        s_Data.CameraBuffer.Position = position;
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

        s_Data.SkyboxShader->Bind();
        s_Data.SkyboxShader->SetMatrix4("u_Projection", camera.GetProjection());
        s_Data.SkyboxShader->SetMatrix4("u_View", glm::mat4(glm::mat3(glm::inverse(transform))));
    }

    void Renderer3D::BeginScene(const EditorCamera& camera)
    {
        LEV_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * camera.GetViewMatrix();
        s_Data.CameraBuffer.Position = camera.GetPosition();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

        s_Data.SkyboxShader->Bind();
        s_Data.SkyboxShader->SetMatrix4("u_Projection", camera.GetProjection());
        s_Data.SkyboxShader->SetMatrix4("u_View", glm::mat4(glm::mat3(camera.GetViewMatrix())));
    }

    void Renderer3D::SetLights(const glm::vec3& dirLightDirection, const DirectionalLightComponent& dirLight)
    {
        s_Data.LightsBuffer.DirLight.Direction = dirLightDirection;
        s_Data.LightsBuffer.DirLight.Ambient = dirLight.Ambient;
        s_Data.LightsBuffer.DirLight.Diffuse = dirLight.Diffuse;
        s_Data.LightsBuffer.DirLight.Specular = dirLight.Specular;

        s_Data.LightsStorageBuffer->SetData(&s_Data.LightsBuffer, sizeof(Renderer3DData::LightsData));
    }

    void Renderer3D::EndScene()
    {
        LEV_PROFILE_FUNCTION();
    }

    void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshRendererComponent& meshRenderer, int entityID)
    {
        auto mesh = meshRenderer.Mesh;
        if (mesh == nullptr) return;

        auto shader = s_Data.MeshShader;
        if (shader == nullptr) return;

        BufferLayout bufferLayout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Float, "a_TexIndex" },
                { ShaderDataType::Float, "a_TexTiling" },
                { ShaderDataType::Int, "a_EntityID" },
        };

        Ref<VertexBuffer> meshVertexBuffer = mesh->CreateVertexBuffer(bufferLayout);
        const Ref<IndexBuffer> indexBuffer = mesh->CreateIndexBuffer();

        Ref<VertexArray> meshVertexArray = VertexArray::Create();
        meshVertexArray->SetIndexBuffer(indexBuffer);
        meshVertexArray->AddVertexBuffer(meshVertexBuffer);

        auto verticesCount = mesh->GetVerticesCount();
        auto* meshVertexBufferBase = new MeshVertex[verticesCount];
        for (uint32_t i = 0; i < verticesCount; i++)
        {
            meshVertexBufferBase[i].Position = mesh->GetVertex(i);
            meshVertexBufferBase[i].Normal = mesh->GetNormal(i);
            meshVertexBufferBase[i].TexCoord = mesh->GetUV(i);
            meshVertexBufferBase[i].TexIndex = 0;
            meshVertexBufferBase[i].TexTiling = 1;
            meshVertexBufferBase[i].EntityID = entityID;
        }

        meshVertexBuffer->SetData(meshVertexBufferBase);

        if (meshRenderer.Texture)
            meshRenderer.Texture->Bind();

        shader->Bind();

        auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));
        shader->SetMatrix4("u_Model", transform);
        shader->SetMatrix3("u_NormalMatrix", normalMatrix);
        RenderCommand::DrawIndexed(meshVertexArray);
        //s_Data.stats.drawCalls++;
    }

    void Renderer3D::DrawSkybox(const SkyboxRendererComponent& skyboxRenderer, int entityID)
    {
        auto mesh = skyboxRenderer.Mesh;
        if (mesh == nullptr) return;

        auto shader = s_Data.SkyboxShader;
        if (shader == nullptr) return;

        auto texture = skyboxRenderer.Texture;
        if (texture == nullptr) return;

        BufferLayout bufferLayout = {
                { ShaderDataType::Float3, "a_Position", },
                { ShaderDataType::Int, "a_EntityID", }
        };

        Ref<VertexBuffer> meshVertexBuffer = mesh->CreateVertexBuffer(bufferLayout);
        const Ref<IndexBuffer> indexBuffer = mesh->CreateIndexBuffer();

        Ref<VertexArray> vertexArray = VertexArray::Create();
        vertexArray->SetIndexBuffer(indexBuffer);
        vertexArray->AddVertexBuffer(meshVertexBuffer);

        auto verticesCount = mesh->GetVerticesCount();
        auto* meshVertexBufferBase = new SkyboxVertex[verticesCount];
        for (uint32_t i = 0; i < verticesCount; i++)
        {
            meshVertexBufferBase[i].Position = mesh->GetVertex(i);
            meshVertexBufferBase[i].EntityID = entityID;
        }

        meshVertexBuffer->SetData(meshVertexBufferBase);

        texture->Bind();
        shader->Bind();
        RenderCommand::SetDepthFunc(DepthFunc::LessOrEqual);
        RenderCommand::DrawIndexed(vertexArray);
        RenderCommand::SetDepthFunc(DepthFunc::Less);

        //s_Data.stats.drawCalls++;
    }
}