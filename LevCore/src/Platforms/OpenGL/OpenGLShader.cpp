#include "OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>

namespace LevEngine
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;

		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		LEV_CORE_ASSERT(false, "Unknown shader type '{0}'", type)
		return 0;
	}
	
	OpenGLShader::OpenGLShader(const std::string& filepath) : m_FilePath(filepath)
	{
		LEV_PROFILE_FUNCTION();
		
		const std::string source = ReadFile(filepath);
		const auto shaderSources = Preprocess(source);
		Compile(shaderSources);

		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		const auto lastDot = filepath.rfind('.');

		const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::~OpenGLShader()
	{
		LEV_PROFILE_FUNCTION();
		
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		LEV_PROFILE_FUNCTION();
		
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		LEV_PROFILE_FUNCTION();
		
		glUseProgram(0);
	}

	void OpenGLShader::SetBool(const std::string& name, const bool value)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform1i(GetUniformLocation(name), static_cast<int>(value));
	}
	void OpenGLShader::SetInt(const std::string& name, const int value)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* array, uint32_t count)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform1iv(GetUniformLocation(name), count, array);
	}

	void OpenGLShader::SetFloat(const std::string& name, const float value)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& vector)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform2fv(GetUniformLocation(name), 1, &vector[0]);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& vector)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform3fv(GetUniformLocation(name), 1, &vector[0]);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& vector)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniform4fv(GetUniformLocation(name), 1, &vector[0]);
	}
	
	void OpenGLShader::SetMatrix2(const std::string& name, const glm::mat2& matrix)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, value_ptr(matrix));
	}
	void OpenGLShader::SetMatrix3(const std::string& name, const glm::mat3& matrix)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, value_ptr(matrix));
	}
	void OpenGLShader::SetMatrix4(const std::string& name, const glm::mat4& matrix)
	{
		LEV_PROFILE_FUNCTION();
		
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value_ptr(matrix));
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath) const
	{
		LEV_PROFILE_FUNCTION();
		
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if(in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			Log::CoreError("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Preprocess(const std::string& source) const
	{
		LEV_PROFILE_FUNCTION();
		
		std::unordered_map<GLenum, std::string> shaderSources;

		const auto typeToken = "#shader";
		const size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while(pos != std::string::npos)
		{
			const size_t eol = source.find_first_of("\r\n", pos);
			LEV_CORE_ASSERT(eol != std::string::npos, "Syntax error")
			const size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			LEV_CORE_ASSERT(ShaderTypeFromString(type), "Unknown shader type!")
			
			const size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
					pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		LEV_PROFILE_FUNCTION();
		
		const int maxNumberOfShaders = 3;
		LEV_CORE_ASSERT(shaderSources.size() <= maxNumberOfShaders, "Only supports {0} of shader so far!", maxNumberOfShaders)

		const auto program = glCreateProgram();
		
		std::array<GLuint, maxNumberOfShaders> glShaderIDs{};
		int glShaderIDIndex = 0;
		
		for (auto& [type, source] : shaderSources)
		{
			const GLuint shader = glCreateShader(type);
			const char* src = source.c_str();
			
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE)
			{
				GLint length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
				
				const auto message = static_cast<char*>(nullptr);
				glGetShaderInfoLog(shader, length, &length, message);
				
				Log::CoreError("Failed to compile {0}", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
				Log::CoreError(message);
				LEV_CORE_ASSERT(false)
				
				glDeleteShader(shader);
				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}
		
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

		if(isLinked == GL_FALSE)
		{
			GLint length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			const auto message = static_cast<char*>(nullptr);
            glGetProgramInfoLog(program, length, &length, message);

			for (auto shader : glShaderIDs)
			{
				glDeleteShader(shader);
			}
			
			Log::CoreError(message);
			LEV_CORE_ASSERT(false, "Shader link failure!")
			
			return;
		}
		
		glValidateProgram(program);
		for (auto shader : glShaderIDs)
		{
			glDeleteShader(shader);
		}

		m_RendererID = program;
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		LEV_PROFILE_FUNCTION();
		
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());

		if (location == -1)
			Log::CoreWarning("In Shader({0}) uniform {1} doesn't exist!", m_RendererID, name);

		m_UniformLocationCache[name] = location;
		return location;
	}
}