#include "levpch.h"
#include "AnimatorComponent.h"

#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
    const Ref<AnimationAsset>& AnimatorComponent::GetAnimationClipConst() const
    {
        return m_AnimationClip;
    }

    Ref<AnimationAsset>& AnimatorComponent::GetAnimationClip()
    {
        return m_AnimationClip;
    }

    void AnimatorComponent::SetAnimationClip(const Ref<AnimationAsset>& animationClip)
    {
        m_AnimationClip = animationClip;
    }
    
    class AnimatorComponentSerializer final
        : public ComponentSerializer<AnimatorComponent, AnimatorComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "Animator"; } 

        void SerializeData(YAML::Emitter& out, const AnimatorComponent& component) override
        {
            SerializeAsset(out, "Animation Clip", component.GetAnimationClipConst());
        }
        
        void DeserializeData(YAML::Node& node, AnimatorComponent& component) override
        {
            component.SetAnimationClip(DeserializeAsset<AnimationAsset>(node));
        }
    };
}
