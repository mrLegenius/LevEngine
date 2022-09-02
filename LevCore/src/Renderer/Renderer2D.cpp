#include "Renderer2D.h"

#include "Shader.h"
#include "RenderCommand.h"

#include "UniformBuffer.h"

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

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float texTiling;

		// Editor-only
		int entityID;
	};

    struct CircleVertex
    {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Thickness;
        float Fade;

        // Editor-only
        int EntityID;
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor-only
        int EntityID;
    };
	
	struct Renderer2DData
	{
		static const uint32_t verticesPerQuad = 4;
		static const uint32_t indicesPerQuad = 6;
		static const uint32_t maxTextureSlots = 32; //TODO: Render Capabilities;
		
		static const uint32_t maxQuads = 20'000;
		static const uint32_t maxVertices = maxQuads * verticesPerQuad;
		static const uint32_t maxIndices = maxQuads * indicesPerQuad;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> blankTexture;

        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<Shader> CircleShader;

        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;
        Ref<Shader> LineShader;

        Ref<Shader> MeshShader;
        Ref<Shader> SkyboxShader;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

        uint32_t LineVertexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;

        float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // slot 0 - blank texture

        glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;
	
	void Renderer2D::Init()
	{
		LEV_PROFILE_FUNCTION();

        s_Data.quadVertexArray = VertexArray::Create();

        s_Data.quadVertexBuffer = VertexBuffer::Create(Renderer2DData::maxVertices * sizeof(QuadVertex));

        s_Data.quadVertexBufferBase = new QuadVertex[Renderer2DData::maxVertices];
		
		s_Data.quadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float4, "a_Color" },
			{ShaderDataType::Float2, "a_TexCoord" },
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TexTiling"},
			{ShaderDataType::Int, "a_EntityID"},
		});

		const auto quadIndices = new uint32_t[Renderer2DData::maxIndices];
		uint32_t offset = 0;
		for(uint32_t i = 0; i < Renderer2DData::maxIndices; i += Renderer2DData::indicesPerQuad)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += Renderer2DData::verticesPerQuad;
		}
		
		const Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::maxIndices);
		s_Data.quadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

        // Circles
        s_Data.CircleVertexArray = VertexArray::Create();

        s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(CircleVertex));
        s_Data.CircleVertexBuffer->SetLayout({
                                                      { ShaderDataType::Float3, "a_WorldPosition" },
                                                      { ShaderDataType::Float3, "a_LocalPosition" },
                                                      { ShaderDataType::Float4, "a_Color"         },
                                                      { ShaderDataType::Float,  "a_Thickness"     },
                                                      { ShaderDataType::Float,  "a_Fade"          },
                                                      { ShaderDataType::Int,    "a_EntityID"      }
                                              });
        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
        s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.maxVertices];

        // Lines
        s_Data.LineVertexArray = VertexArray::Create();

        s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(LineVertex));
        s_Data.LineVertexBuffer->SetLayout({
                                                   { ShaderDataType::Float3, "a_Position" },
                                                   { ShaderDataType::Float4, "a_Color"    },
                                                   { ShaderDataType::Int,    "a_EntityID" }
                                           });
        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.maxVertices];

        s_Data.blankTexture = Texture2D::Create(1, 1);
		uint32_t blankTextureData = 0xffffffff;
		s_Data.blankTexture->SetData(&blankTextureData, sizeof(uint32_t));
        s_Data.textureSlots[0] = s_Data.blankTexture;
		
		s_Data.quadVertexArray->AddVertexBuffer(s_Data.quadVertexBuffer);

        s_Data.QuadShader = Shader::Create("assets/shaders/Ultimate2D.shader");
        s_Data.CircleShader = Shader::Create("assets/shaders/Circle.shader");
        s_Data.LineShader = Shader::Create("assets/shaders/Line.shader");
        s_Data.MeshShader = Shader::Create("assets/shaders/Mesh_White.shader");
        s_Data.SkyboxShader = Shader::Create("assets/shaders/Skybox.shader");

        s_Data.textureSlots[0] = s_Data.blankTexture;
        
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
        LEV_PROFILE_FUNCTION()

		delete s_Data.quadVertexBufferBase;
		delete s_Data.CircleVertexBufferBase;
		delete s_Data.LineVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		LEV_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        s_Data.SkyboxShader->Bind();
        s_Data.SkyboxShader->SetMatrix4("u_Projection", camera.GetProjection());
        s_Data.SkyboxShader->SetMatrix4("u_View", glm::mat4(glm::mat3(glm::inverse(transform))));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		LEV_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * camera.GetViewMatrix();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        s_Data.SkyboxShader->Bind();
        s_Data.SkyboxShader->SetMatrix4("u_Projection", camera.GetProjection());
        s_Data.SkyboxShader->SetMatrix4("u_View", glm::mat4(glm::mat3(camera.GetViewMatrix())));

		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		LEV_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        s_Data.LineVertexCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.textureSlotIndex = 1;
	}

    void Renderer2D::DrawMesh(const glm::mat4& transform, const MeshRendererComponent& meshRenderer, int entityID)
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
            meshVertexBufferBase[i].Position = transform * glm::vec4(mesh->GetVertex(i), 1.0f);
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
        RenderCommand::DrawIndexed(meshVertexArray);
        s_Data.stats.drawCalls++;
    }

    void Renderer2D::DrawSkybox(const SkyboxRendererComponent& skyboxRenderer, int entityID)
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

        s_Data.stats.drawCalls++;
    }

	void Renderer2D::Flush()
	{
		LEV_PROFILE_FUNCTION();
		
		if(s_Data.quadIndexCount)
        {
            const auto dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
            s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferBase, dataSize);

            for(uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
            {
                s_Data.textureSlots[i]->Bind(i);
            }

            s_Data.QuadShader->Bind();
            RenderCommand::DrawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);
            s_Data.stats.drawCalls++;
        }

        if (s_Data.CircleIndexCount)
        {
            const auto dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

            s_Data.CircleShader->Bind();
            RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
            s_Data.stats.drawCalls++;
        }

        if (s_Data.LineVertexCount)
        {
            const auto dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

            s_Data.LineShader->Bind();
            RenderCommand::SetLineWidth(s_Data.LineWidth);
            RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
            s_Data.stats.drawCalls++;
        }
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

    void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
    {
        s_Data.LineVertexBufferPtr->Position = p0;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexBufferPtr->Position = p1;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexCount += 2;
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
        glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
        glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
        glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        glm::vec3 lineVertices[4];
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

        DrawLine(lineVertices[0], lineVertices[1], color, entityID);
        DrawLine(lineVertices[1], lineVertices[2], color, entityID);
        DrawLine(lineVertices[2], lineVertices[3], color, entityID);
        DrawLine(lineVertices[3], lineVertices[0], color, entityID);
    }

    void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
    {
        LEV_PROFILE_FUNCTION();

        // TODO: implement for circles
        // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        // 	NextBatch();

        for (auto& quadVertexPosition : s_Data.QuadVertexPositions)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * quadVertexPosition;
            s_Data.CircleVertexBufferPtr->LocalPosition = quadVertexPosition * 2.0f;
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += Renderer2DData::indicesPerQuad;

        s_Data.stats.quadCount++;
    }

	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& spriteRenderer, const int entityID)
	{
        if (spriteRenderer.Texture && spriteRenderer.Texture->IsLoaded())
            Draw(transform, spriteRenderer.color, spriteRenderer.Texture, spriteRenderer.TilingFactor, entityID);
        else
            Draw(transform, spriteRenderer.color, s_Data.blankTexture, 1.0f, entityID);
	}

	void Renderer2D::Draw(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, const float tiling, const int entityID)
	{
		LEV_PROFILE_FUNCTION();

		AddQuadToVertexBuffer(transform, color, texture, tiling, entityID);
	}

	void Renderer2D::AddQuadToVertexBuffer(const glm::mat4& model, const glm::vec4& color, const Ref<Texture2D>& texture, const float tiling, const int entityID)
	{
		LEV_PROFILE_FUNCTION();
		
		if (s_Data.quadIndexCount >= Renderer2DData::maxIndices)
			NextBatch();
		
		float textureIndex = -1.0f;

		for(uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
		{
			if(*s_Data.textureSlots[i] == *texture)
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}
		
		if(textureIndex == -1.0f)
		{
			if (s_Data.textureSlotIndex >= Renderer2DData::maxTextureSlots)
				NextBatch();
			
			textureIndex = static_cast<float>(s_Data.textureSlotIndex);
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}
		
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f } };
		
		for (uint32_t i = 0; i < Renderer2DData::verticesPerQuad; i++)
		{
			s_Data.quadVertexBufferPtr->position = model * s_Data.QuadVertexPositions[i];
			s_Data.quadVertexBufferPtr->color = color;
			s_Data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_Data.quadVertexBufferPtr->texIndex = textureIndex;
			s_Data.quadVertexBufferPtr->texTiling = tiling;
			s_Data.quadVertexBufferPtr->entityID = entityID;
			s_Data.quadVertexBufferPtr++;
		}

		s_Data.quadIndexCount += Renderer2DData::indicesPerQuad;

		s_Data.stats.quadCount++;
	}

    float Renderer2D::GetLineWidth()
    {
        return s_Data.LineWidth;
    }

    void Renderer2D::SetLineWidth(float width)
    {
        s_Data.LineWidth = width;
    }

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.stats;
	}
}
