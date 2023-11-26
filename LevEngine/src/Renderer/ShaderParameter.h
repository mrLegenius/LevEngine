#pragma once
#include "ShaderType.h"
#include "DataTypes/Utility.h"

namespace LevEngine
{
	class SamplerState;
	class StructuredBuffer;
	class Texture;
	class ConstantBuffer;

	class ShaderParameter
	{
	public:
		ShaderParameter() = default;

		ShaderParameter(String name, const uint32_t slotId, const ShaderType shaderType)
			: m_Name(Move(name))
			, m_SlotId(slotId)
			, m_ShaderType(shaderType) { }

		void Set(const Ref<ConstantBuffer>& constantBuffer);
		void Set(const Ref<Texture>& texture);
		void Set(const Ref<SamplerState>& sampler);
		void Set(const Ref<StructuredBuffer>& structuredBuffer);

		void Bind() const;
		void Unbind() const;
		bool IsValid() const { return !m_Name.empty(); }

	private:
		Weak<Texture> m_Texture;
		Weak<SamplerState> m_SamplerState;
		Weak<ConstantBuffer> m_ConstantBuffer;
		Weak<StructuredBuffer> m_StructuredBuffer;

		String m_Name;
		uint32_t m_SlotId;
		ShaderType m_ShaderType;
	};
}
