#pragma once
#include <string>
#include <unordered_map>

#include "Renderer/Shader.h"

#include <glm/glm.hpp>

//TODO: Remove!
typedef unsigned int GLenum;

namespace LevEngine
{
	class OpenGLShader : public Shader

	{
	public:
		explicit OpenGLShader(const std::string& filepath);
		~OpenGLShader() override;

		void Bind() const override;
		void Unbind() const override;

		[[nodiscard]] const std::string& GetName() const override { return m_Name; }

		void SetBool(const std::string& name, bool value) override;
		void SetInt(const std::string& name, int value) override;
		void SetIntArray(const std::string& name, int* array, uint32_t count) override;

		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const glm::vec2& vector) override;
		void SetFloat3(const std::string& name, const glm::vec3& vector) override;
		void SetFloat4(const std::string& name, const glm::vec4& vector) override;

		void SetMatrix2(const std::string& name, const glm::mat2& matrix) override;
		void SetMatrix3(const std::string& name, const glm::mat3& matrix) override;
		void SetMatrix4(const std::string& name, const glm::mat4& matrix) override;
	private:
		[[nodiscard]] std::string ReadFile(const std::string& filepath) const;
		[[nodiscard]] std::unordered_map<GLenum, std::string> Preprocess(const std::string& source) const;

		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		
		[[nodiscard]] int GetUniformLocation(const std::string& name);

	
	private:
		std::string m_FilePath;
		std::string m_Name;
		
		uint32_t m_RendererID = 0;
		
		std::unordered_map<std::string, int> m_UniformLocationCache;
	};
}
