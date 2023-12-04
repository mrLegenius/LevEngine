#include "levpch.h"
#include "AnimatorComponent.h"

#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"
#include "Renderer/3D/Animator.h"

namespace LevEngine
{
    AnimatorComponent::AnimatorComponent()
    {
        m_Animator = CreateRef<Animator>();
    }

    const Ref<AnimationAsset>& AnimatorComponent::GetAnimationClipConst() const
    {
        return m_Animator->GetAnimationClip();
    }

    Ref<AnimationAsset>& AnimatorComponent::GetAnimationClip()
    {
        return m_Animator->GetAnimationClip();
    }

    void AnimatorComponent::SetAnimationClip(const Ref<AnimationAsset>& animationClip)
    {
        m_Animator->SetAnimationClip(animationClip);
    }

    bool AnimatorComponent::GetPlayOnInit() const
    {
        return m_PlayOnInit;
    }

    void AnimatorComponent::SetPlayOnInit(bool playOnInit)
    {
        m_PlayOnInit = playOnInit;
    }

    void AnimatorComponent::PlayAnimation() const
    {
        m_Animator->PlayAnimation();
    }

    void AnimatorComponent::UpdateAnimation(float deltaTime) const
    {
        m_Animator->UpdateAnimation(deltaTime);
    }

    Array<Matrix, AnimationConstants::MaxBoneCount> AnimatorComponent::GetFinalBoneMatrices() const
    {
        return m_Animator->GetFinalBoneMatrices();
    }

    void AnimatorComponent::DrawDebugPose(const Vector3& meshPosition)
    {
        m_Animator->DrawDebugPose(meshPosition);
    }

    void AnimatorComponent::DrawDebugSkeleton(const Vector3& meshPosition)
    {
        m_Animator->DrawDebugSkeleton(meshPosition);
    }

    void AnimatorComponent::Init()
    {
        if (m_PlayOnInit)
        {
            PlayAnimation();
        }
        
        m_IsInited = true;
    }

    bool AnimatorComponent::IsInitialized() const
    {
        return m_IsInited;
    }

    void AnimatorComponent::ResetInit()
    {
        m_IsInited = false;
    }

    class AnimatorComponentSerializer final
        : public ComponentSerializer<AnimatorComponent, AnimatorComponentSerializer>
    {
    protected:
        const char* GetKey() override { return "Animator"; } 

        void SerializeData(YAML::Emitter& out, const AnimatorComponent& component) override
        {
            SerializeAsset(out, c_AnimationClipNodeKey, component.GetAnimationClipConst());
            out << YAML::Key << c_PlayOnInitNodeKey << YAML::Value << component.GetPlayOnInit();
        }
        
        void DeserializeData(YAML::Node& node, AnimatorComponent& component) override
        {
            component.SetAnimationClip(DeserializeAsset<AnimationAsset>(node[c_AnimationClipNodeKey]));

            if (const auto playOnInitNode = node[c_PlayOnInitNodeKey])
            {
                component.SetPlayOnInit(playOnInitNode.as<bool>());
            }

            component.ResetInit();
        }

    private:
        const char* c_AnimationClipNodeKey = "Animation Clip";
        const char* c_PlayOnInitNodeKey = "PlayOnInit";
    };
}
