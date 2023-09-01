#pragma once
#include "ConstantBuffer.h"
#include "StructuredBuffer.h"
#include "Texture.h"

namespace LevEngine
{
	class ShaderParameter
	{
	public:
		ShaderParameter() = default;

		ShaderParameter(std::string name, const uint32_t slotId, const ShaderType shaderType)
			: m_Name(std::move(name))
			, m_SlotId(slotId)
			, m_ShaderType(shaderType)
		{
			
		}

		void Set(const Ref<ConstantBuffer>& constantBuffer);
		void Set(const Ref<Texture>& texture);
		void Set(const Ref<SamplerState>& sampler);
		void Set(const Ref<StructuredBuffer>& structuredBuffer);

		void Bind() const;
		void Unbind() const;
		bool IsValid() const { return !m_Name.empty(); }

	private:
		std::weak_ptr<Texture> m_Texture;
		std::weak_ptr<SamplerState> m_SamplerState;
		std::weak_ptr<ConstantBuffer> m_ConstantBuffer;
		std::weak_ptr<StructuredBuffer> m_StructuredBuffer;

		std::string m_Name;
		uint32_t m_SlotId;
		ShaderType m_ShaderType;
	};
}
