#pragma once

#include "Kernel/Core.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    struct RenderParams;
    struct SpotLightComponent;
    struct PointLightComponent;
    struct DirectionalLightComponent;
    class ConstantBuffer;
    
    struct LEV_API DirLightData
    {
        alignas(16) Vector3 Direction{};
        alignas(16) Vector3 Color{};
    };

    enum class LightType : uint32_t
    {
        Point = 0,
        Spot = 1,
    };

    struct LEV_API LightData
    {
        Vector4 PositionViewSpace{};
        Vector4 DirectionViewSpace{};

        alignas(16) Vector3 Position{};
        alignas(16) Vector3 Direction{};
        alignas(16) Vector3 Color{};

        float Range = 1.0f;
        float Angle = 1.0f;
        float Smoothness = 0.75f;
        float Intensity = 1.0f;

        LightType Type;
    };

    struct LEV_API LightingData
    {
        DirLightData DirLight;
        LightData Lights[RenderSettings::MaxLights];
        uint32_t LightsCount = 0;
    };
    
    class LEV_API LightCollection
    {
    public:
        LightCollection();

        void Prepare(entt::registry& registry, const RenderParams& params);
        void DirectionalLightSystem(entt::registry& registry);
        void PointLightsSystem(entt::registry& registry, const RenderParams& params);
        void SpotLightsSystem(entt::registry& registry, const RenderParams& params);
    
    private:
        void SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight);
        void AddPointLight(const Vector4& positionViewSpace, const Vector3& position, const PointLightComponent& light);
        void AddSpotLight(const Vector4& positionViewSpace, const Vector4& directionViewSpace, const Vector3& position, const Vector3& direction, const
                          SpotLightComponent& light);

        void UpdateLights();
        
        LightingData m_LightningData{};
        
        Ref<ConstantBuffer> m_LightningConstantBuffer;
    };
}
