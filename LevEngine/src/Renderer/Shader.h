#pragma once
#include "BufferBinding.h"
#include "ShaderParameter.h"
#include "ShaderType.h"
#include "Kernel/PointerUtils.h"

namespace LevEngine
{
	class Shader
	{
	public:

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& filepath, ShaderType shaderTypes);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		[[nodiscard]] virtual bool HasSemantic(const BufferBinding& binding) = 0;
		[[nodiscard]] virtual uint32_t GetSlotIdBySemantic(const BufferBinding& binding) = 0;
		[[nodiscard]] virtual ShaderParameter& GetShaderParameterByName(const std::string& name) const = 0;

		[[nodiscard]] ShaderType GetType() const { return m_Type; }

	protected:
		explicit Shader(std::string filepath) : m_FilePath(std::move(filepath)) { }
		virtual ~Shader() = default;

		std::string m_FilePath;
		std::string m_Name;

		ShaderType m_Type;
	};
}
