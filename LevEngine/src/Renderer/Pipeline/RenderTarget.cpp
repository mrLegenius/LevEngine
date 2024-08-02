#include "levpch.h"
#include "RenderTarget.h"

#include "Texture.h"
#include "Kernel/Application.h"
#include "Math/Math.h"
#include "Renderer/RenderDevice.h"

namespace LevEngine
{
	RenderTarget::RenderTarget()
	{
		m_Textures.resize(static_cast<size_t>(AttachmentPoint::NumAttachmentPoints));
		m_StructuredBuffers.resize(StructuredBuffersCount);
	}

	Ref<RenderTarget> RenderTarget::Create()
	{
		return App::RenderDevice().CreateRenderTarget();
	}

	void RenderTarget::AttachTexture(AttachmentPoint attachment, const Ref<Texture>& texture)
	{
		m_Textures[static_cast<uint8_t>(attachment)] = texture;

		// Next time the render target is "bound", check that it is valid
		m_CheckValidity = true;
	}

	Ref<Texture> RenderTarget::GetTexture(AttachmentPoint attachment)
	{ return m_Textures[static_cast<uint8_t>(attachment)]; }

	void RenderTarget::Clear(AttachmentPoint attachment, const ClearFlags clearFlags, const Vector4& color,
		const float depth, const uint8_t stencil) const
	{
		if (const auto& texture = m_Textures[static_cast<uint8_t>(attachment)])
			texture->Clear(clearFlags, color, depth, stencil);
	}

	void RenderTarget::Clear(const ClearFlags clearFlags, const Vector4& color, const float depth,
		const uint8_t stencil) const
	{
		for (uint8_t i = 0; i < static_cast<uint8_t>(AttachmentPoint::NumAttachmentPoints); ++i)
		{
			Clear(static_cast<AttachmentPoint>(i), clearFlags, color, depth, stencil);
		}
	}

	void RenderTarget::Resize(const uint16_t width, const uint16_t height)
	{
		if (m_Width != width || m_Height != height)
		{
			m_Width = Math::Max<uint16_t>(width, 1u);
			m_Height = Math::Max<uint16_t>(height, 1u);
			// Resize the attached textures
			for (auto texture : m_Textures)
			{
				if (texture)
					texture->Resize(m_Width, m_Height);
			}
		}
	}
}

