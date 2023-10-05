#include "levpch.h"
#include "BlendState.h"

#include "RenderSettings.h"
#include "Platform/D3D11/D3D11BlendState.h"

namespace LevEngine
{
	BlendMode BlendMode::AlphaBlending = BlendMode{ true, false, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOperation::Add, BlendFactor::OneMinusDstAlpha, BlendFactor::One };
	BlendMode BlendMode::Additive = BlendMode{ true, false, BlendFactor::One, BlendFactor::One };

	BlendMode::BlendMode(
		const bool enabled,
		const bool logicOpEnabled,
		const BlendFactor srcFactor,
		const BlendFactor dstFactor,
		const BlendOperation blendOp,
		const BlendFactor srcAlphaFactor,
		const BlendFactor dstAlphaFactor,
		const BlendOperation alphaOp,
		const LogicOperator logicOp,
		const BlendWrite blendWrite)
		  : BlendEnabled(enabled)
		  , LogicOpEnabled(logicOpEnabled)
		  , SrcFactor(srcFactor)
		  , DstFactor(dstFactor)
		  , BlendOp(blendOp)
		  , SrcAlphaFactor(srcAlphaFactor)
		  , DstAlphaFactor(dstAlphaFactor)
		  , AlphaOp(alphaOp)
		  , LogicOp(logicOp)
		  , Write(blendWrite)
	{ }
	
	Ref<BlendState> BlendState::Create()
	{
		switch (RenderSettings::RendererAPI)
		{
		case RendererAPI::None:
			LEV_CORE_ASSERT(false, "None for API was chosen");
		case RendererAPI::OpenGL:
			LEV_NOT_IMPLEMENTED
		case RendererAPI::D3D11:
			return CreateRef<D3D11BlendState>();
		default:
			LEV_THROW("Unknown Renderer API");
		}
	}

	BlendState::BlendState()
	{
		m_BlendModes.resize(8, BlendMode());
	}
	
	void BlendState::SetBlendMode(const BlendMode& blendMode)
	{
		m_BlendModes[0] = blendMode;
		m_Dirty = true;
	}

	void BlendState::SetBlendModes(const Vector<BlendMode>& blendModes)
	{
		m_BlendModes = blendModes;
		m_Dirty = true;
	}

	void BlendState::SetAlphaCoverage(const bool enabled)
	{
		if (m_AlphaToCoverageEnabled != enabled)
		{
			m_AlphaToCoverageEnabled = enabled;
			m_Dirty = true;
		}
	}

	void BlendState::SetIndependentBlend(const bool enabled)
	{
		if (m_AlphaToCoverageEnabled != enabled)
		{
			m_AlphaToCoverageEnabled = enabled;
			m_Dirty = true;
		}
	}

	void BlendState::SetConstBlendFactor(const Vector4& constantBlendFactor)
	{
		m_ConstBlendFactor = constantBlendFactor;
		// No need to set the dirty flag as this value is not used to create the blend state object.
		// It is only used when activating the blend state of the output merger.
	}

	void BlendState::SetSampleMask(const uint32_t sampleMask)
	{
		m_SampleMask = sampleMask;
		// No need to set the dirty flag as this value is not used to create the blend state object.
		// It is only used when activating the blend state of the output merger.
	}
}
