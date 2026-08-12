#pragma once

#include "Kernel/Core.h"
#include "BufferBinding.h"
#include "ShaderMacros.h"
#include "ShaderMaterialLayout.h"
#include "ShaderParameter.h"
#include "ShaderType.h"

namespace LevEngine
{
	class LEV_API Shader
	{
	public:

		static Ref<Shader> Create(const String& filepath);
		static Ref<Shader> Create(const String& filepath, const ShaderMacros& macros);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		[[nodiscard]] virtual bool HasSemantic(const BufferBinding& binding) = 0;
		[[nodiscard]] virtual uint32_t GetSlotIdBySemantic(const BufferBinding& binding) = 0;
		[[nodiscard]] virtual ShaderParameter& GetShaderParameterByName(const String& name) const = 0;

		//<--- What a material can set on this shader. Empty for shaders no material binds ---<<
		[[nodiscard]] virtual const ShaderMaterialLayout& GetMaterialLayout() const = 0;

		[[nodiscard]] ShaderType GetType() const { return m_Type; }

		[[nodiscard]] bool HasStage(const ShaderType type) const { return m_Type & type; }

		//<--- A shader is only good for drawing geometry once both of its stages compiled ---<<
		[[nodiscard]] bool CanDrawGeometry() const
		{
			return HasStage(ShaderType::Vertex) && HasStage(ShaderType::Pixel);
		}

		[[nodiscard]] const String& GetFilePath() const { return m_FilePath; }
		virtual void Reload() { }

	protected:
		explicit Shader(String filepath, ShaderMacros macros)
		: m_FilePath(Move(filepath)), m_Macros(Move(macros)){ }
		virtual ~Shader() = default;

		String m_FilePath;
		String m_Name;

		ShaderType m_Type{};
		ShaderMacros m_Macros{};
	};
}
