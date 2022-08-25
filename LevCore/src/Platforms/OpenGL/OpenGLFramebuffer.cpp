#include "OpenGLFramebuffer.h"

#include <glad/gl.h>

namespace LevEngine
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils
    {
		static GLenum TextureTarget(const bool multiSample)
		{
			return multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(const bool multiSample, uint32_t* outIDs, const uint32_t count)
		{
			glCreateTextures(TextureTarget(multiSample), count, outIDs);
		}

		static void BindTexture(const bool multiSample, const uint32_t id)
		{
			glBindTexture(TextureTarget(multiSample), id);
		}

		static void AttachColorTexture(
			const uint32_t id, 
			const int samples, 
			const GLenum internalFormat, 
			const GLenum format,
			const uint32_t width,
			const uint32_t height,
			const int index)
		{
			const bool multiSample = samples > 1;
			if (multiSample)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multiSample), id, 0);
		}

		static void AttachDepthTexture(
			const uint32_t id, 
			const int samples, 
			const GLenum format, 
			const GLenum attachmentType, 
			const uint32_t width, 
			const uint32_t height)
		{
			const bool multiSample = samples > 1;
			if (multiSample)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multiSample), id, 0);
		}

		static bool IsDepthFormat(const FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Depth24Stencil8:	return true;
			default: return false;
			}
		}

		static GLenum FramebufferTextureFormatToOpenGLFormat(const FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:      return GL_RGBA8;
			case FramebufferTextureFormat::RedInteger: return GL_RED_INTEGER;
			}

			LEV_CORE_ASSERT(false);
			return 0;
		}

	}
	
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto attachment : m_Specification.attachmentSpecification.attachments)
		{
			if (!Utils::IsDepthFormat(attachment.textureFormat))
				m_ColorAttachmentSpecifications.emplace_back(attachment);
			else
				m_DepthAttachmentSpecification = attachment;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		const bool multiSample = m_Specification.samples > 1;

		// Attachments
		if (!m_ColorAttachmentSpecifications.empty())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multiSample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multiSample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].textureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_RGBA8, GL_RGBA, m_Specification.width, m_Specification.height, i);
					break;
				case FramebufferTextureFormat::RedInteger:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.samples, GL_R32I, GL_RED_INTEGER, m_Specification.width, m_Specification.height, i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.textureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multiSample, &m_DepthAttachment, 1);
			Utils::BindTexture(multiSample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.textureFormat)
			{
			case FramebufferTextureFormat::Depth24Stencil8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.width, m_Specification.height);
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			LEV_CORE_ASSERT(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		LEV_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.width, m_Specification.height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(const uint32_t width, const uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			Log::CoreWarning("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.width = width;
		m_Specification.height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(const uint32_t attachmentIndex, const int x, const int y)
	{
		LEV_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(const uint32_t attachmentIndex, const int value)
	{
		LEV_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		const auto format = Utils::FramebufferTextureFormatToOpenGLFormat(spec.textureFormat);
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, format, GL_INT, &value);
	}
}
