#include "Renderer2D.h"

#include <glm/ext/matrix_transform.hpp>

#include "Shader.h"
#include "RenderCommand.h"

namespace LevEngine
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float texTiling;

		// Editor-only
		int entityID = 2;
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
		Ref<Shader> ultimateShader;
		Ref<Texture2D> blankTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // slot 0 - blank texture

		Renderer2D::Statistics stats;
	};

	static Renderer2DData* s_Data;
	
	void Renderer2D::Init()
	{
		LEV_PROFILE_FUNCTION();
		
		s_Data = new Renderer2DData;
		auto& data = *s_Data;
		data.quadVertexArray = VertexArray::Create();

		s_Data->quadVertexBuffer = VertexBuffer::Create(Renderer2DData::maxVertices * sizeof(QuadVertex));

		s_Data->quadVertexBufferBase = new QuadVertex[Renderer2DData::maxVertices];
		
		s_Data->quadVertexBuffer->SetLayout({
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
		data.quadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;
		
		data.blankTexture = Texture2D::Create(1, 1);
		uint32_t blankTextureData = 0xffffffff;
		data.blankTexture->SetData(&blankTextureData, sizeof(uint32_t));
		s_Data->textureSlots[0] = s_Data->blankTexture;

		int32_t samplers[Renderer2DData::maxTextureSlots];
		for(int32_t i = 0; i < Renderer2DData::maxTextureSlots; i++)
		{
			samplers[i] = i;
		}
		
		data.quadVertexArray->AddVertexBuffer(s_Data->quadVertexBuffer);

		data.ultimateShader = Shader::Create("assets/shaders/Ultimate2D.shader");
		data.ultimateShader->Bind();
		data.ultimateShader->SetIntArray("u_Textures", samplers, Renderer2DData::maxTextureSlots);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data->quadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		LEV_PROFILE_FUNCTION();

		s_Data->ultimateShader->Bind();
		s_Data->ultimateShader->SetMatrix4("u_Projection", camera.GetProjection());
		s_Data->ultimateShader->SetMatrix4("u_View", glm::inverse(transform));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		LEV_PROFILE_FUNCTION();

		s_Data->ultimateShader->Bind();
		s_Data->ultimateShader->SetMatrix4("u_Projection", camera.GetProjection());
		s_Data->ultimateShader->SetMatrix4("u_View", camera.GetViewMatrix());

		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		LEV_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->quadIndexCount = 0;
		s_Data->quadVertexBufferPtr = s_Data->quadVertexBufferBase;

		s_Data->textureSlotIndex = 1;
	}
	
	void Renderer2D::Flush()
	{
		LEV_PROFILE_FUNCTION();
		
		if(s_Data->quadIndexCount == 0)
			return;

		const auto dataSize = (uint32_t)((uint8_t*)s_Data->quadVertexBufferPtr - (uint8_t*)s_Data->quadVertexBufferBase);
		s_Data->quadVertexBuffer->SetData(s_Data->quadVertexBufferBase, dataSize);

		for(uint32_t i = 0; i < s_Data->textureSlotIndex; i++)
		{
			s_Data->textureSlots[i]->Bind(i);
		}

		s_Data->ultimateShader->Bind();
		RenderCommand::DrawIndexed(s_Data->quadVertexArray, s_Data->quadIndexCount);
		s_Data->stats.drawCalls++;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& spriteRenderer, const int entityID)
	{
		Draw(
			transform,
			spriteRenderer.color,
			s_Data->blankTexture,
			1.0f,
			entityID);
	}

	void Renderer2D::Draw(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, const float tiling, const int entityID)
	{
		LEV_PROFILE_FUNCTION();

		AddQuadToVertexBuffer(transform, color, texture, tiling, entityID);

		const auto& shader = s_Data->ultimateShader;

		s_Data->quadVertexArray->Bind();
		texture->Bind();
		RenderCommand::DrawIndexed(s_Data->quadVertexArray);
	}

	void Renderer2D::AddQuadToVertexBuffer(const glm::mat4& model, const glm::vec4& color, const Ref<Texture2D>& texture, const float tiling, const int entityID)
	{
		LEV_PROFILE_FUNCTION();
		
		if (s_Data->quadIndexCount >= Renderer2DData::maxIndices)
			NextBatch();
		
		float textureIndex = -1.0f;

		for(uint32_t i = 0; i < s_Data->textureSlotIndex; i++)
		{
			if(*s_Data->textureSlots[i] == *texture)
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}
		
		if(textureIndex == -1.0f)
		{
			if (s_Data->textureSlotIndex >= Renderer2DData::maxTextureSlots)
				NextBatch();
			
			textureIndex = static_cast<float>(s_Data->textureSlotIndex);
			s_Data->textureSlots[s_Data->textureSlotIndex] = texture;
			s_Data->textureSlotIndex++;
		}
		
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f } };
		
		constexpr glm::vec4 quadVertexPositions[] {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		
		for (uint32_t i = 0; i < Renderer2DData::verticesPerQuad; i++)
		{
			s_Data->quadVertexBufferPtr->position = model * quadVertexPositions[i];
			s_Data->quadVertexBufferPtr->color = color;
			s_Data->quadVertexBufferPtr->texCoord = textureCoords[i];
			s_Data->quadVertexBufferPtr->texIndex = textureIndex;
			s_Data->quadVertexBufferPtr->texTiling = tiling;
			s_Data->quadVertexBufferPtr->entityID = entityID;
			s_Data->quadVertexBufferPtr++;
		}

		s_Data->quadIndexCount += Renderer2DData::indicesPerQuad;

		s_Data->stats.quadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->stats;
	}
}
