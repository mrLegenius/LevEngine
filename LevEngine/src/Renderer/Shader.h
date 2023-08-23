#pragma once
#include "Buffer.h"
#include "Kernel/PointerUtils.h"

namespace LevEngine
{
	class Shader
	{
	public:

		enum class Type
		{
			None = 0,
			Vertex = 1,
			Geometry = 2,
			Pixel = 4,
			Compute = 8,
		};

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& filepath, Type shaderTypes);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

		[[nodiscard]] Type GetType() const { return m_Type; }

	protected:
		explicit Shader(std::string filepath) : m_FilePath(std::move(filepath)) { }
		virtual ~Shader() = default;

		std::string m_FilePath;
		std::string m_Name;

		Type m_Type;

		BufferLayout m_Layout;
	};

	inline Shader::Type operator|(const Shader::Type a, const Shader::Type b)
	{
		return static_cast<Shader::Type>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool operator&(const Shader::Type a, const Shader::Type b)
	{
		return static_cast<int>(a) & static_cast<int>(b);
	}
}
