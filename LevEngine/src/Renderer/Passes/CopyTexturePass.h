#pragma once
#include "RenderPass.h"

namespace LevEngine
{
    class Texture;

    class CopyTexturePass final : public RenderPass
    {
    public:
        CopyTexturePass(const Ref<Texture>& destinationTexture,
                        const Ref<Texture>& sourceTexture,
                        const String& label = "Copy Texture")
            : m_DestinationTexture(destinationTexture)
              , m_SourceTexture(sourceTexture)
              , m_Label(label) { }

        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<Texture> m_DestinationTexture;
        Ref<Texture> m_SourceTexture;
        String m_Label;
    };
}
