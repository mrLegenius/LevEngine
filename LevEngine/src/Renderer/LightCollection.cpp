#include "levpch.h"
#include "LightCollection.h"
#include "Scene/Components/Lights/Lights.h"

#include "ConstantBuffer.h"
#include "RenderParams.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    LightCollection::LightCollection()
        : m_LightningConstantBuffer(ConstantBuffer::Create(sizeof LightingData, 2))
    {
    }

    void LightCollection::Prepare(entt::registry& registry, const RenderParams& params)
    {
        DirectionalLightSystem(registry);
        PointLightsSystem(registry, params);
        SpotLightsSystem(registry, params);

        UpdateLights();
    }

    void LightCollection::DirectionalLightSystem(entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        const auto view = registry.group<>(entt::get<Transform, DirectionalLightComponent>);
        for (const auto entity : view)
        {
            auto [transform, light] = view.get<Transform, DirectionalLightComponent>(entity);

            SetDirLight(transform.GetForwardDirection(), light);
        }
    }

    void LightCollection::PointLightsSystem(entt::registry& registry, const RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
        for (const auto entity : view)
        {
            auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

            auto worldPosition = transform.GetWorldPosition();
            auto positionViewSpace = Vector4::Transform(
                Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), params.CameraViewMatrix);
            AddPointLight(positionViewSpace, worldPosition, light);
        }
    }

    void LightCollection::SpotLightsSystem(entt::registry& registry, const RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        const auto view = registry.group<>(entt::get<Transform, SpotLightComponent>);
        for (const auto entity : view)
        {
            auto [transform, light] = view.get<Transform, SpotLightComponent>(entity);

            auto worldPosition = transform.GetWorldPosition();
            auto direction = transform.GetForwardDirection();
            auto positionViewSpace = Vector4::Transform(
                Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), params.CameraViewMatrix);
            auto directionViewSpace = Vector4::Transform(
                Vector4(direction.x, direction.y, direction.z, 0.0f), params.CameraViewMatrix);
            AddSpotLight(positionViewSpace, directionViewSpace, worldPosition, direction, light);
        }
    }

    void LightCollection::SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight)
    {
        LEV_PROFILE_FUNCTION();

        m_LightningData.DirLight.Direction = dirLightDirection;
        m_LightningData.DirLight.Color = static_cast<Vector3>(dirLight.color);
    }

    void LightCollection::AddPointLight(const Vector4& positionViewSpace, const Vector3& position,
                                        const PointLightComponent& light)
    {
        LEV_PROFILE_FUNCTION();

        if (m_LightningData.LightsCount >= RenderSettings::MaxLights)
        {
            Log::CoreWarning("Trying to add light beyond maximum");
            return;
        }

        LightData& lightGPUData = m_LightningData.Lights[m_LightningData.LightsCount];

        lightGPUData.PositionViewSpace = positionViewSpace;
        lightGPUData.Position = position;
        lightGPUData.Color = static_cast<Vector3>(light.color);
        lightGPUData.Range = light.Range;
        lightGPUData.Intensity = light.Intensity;
        lightGPUData.Smoothness = 1 - light.Smoothness;
        lightGPUData.Type = LightType::Point;

        m_LightningData.LightsCount++;
    }

    void LightCollection::AddSpotLight(const Vector4& positionViewSpace, const Vector4& directionViewSpace,
                                       const Vector3& position, const Vector3& direction,
                                       const SpotLightComponent& light)
    {
        LEV_PROFILE_FUNCTION();

        if (m_LightningData.LightsCount >= RenderSettings::MaxLights)
        {
            Log::CoreWarning("Trying to add light beyond maximum");
            return;
        }

        LightData& lightGPUData = m_LightningData.Lights[m_LightningData.LightsCount];

        lightGPUData.PositionViewSpace = positionViewSpace;
        lightGPUData.DirectionViewSpace = directionViewSpace;
        lightGPUData.Position = position;
        lightGPUData.Direction = direction;
        lightGPUData.Color = static_cast<Vector3>(light.color);
        lightGPUData.Range = light.Range;
        lightGPUData.Angle = light.Angle;
        lightGPUData.Intensity = light.Intensity;
        lightGPUData.Smoothness = 1 - light.Smoothness;
        lightGPUData.Type = LightType::Spot;

        m_LightningData.LightsCount++;
    }

    void LightCollection::UpdateLights()
    {
        LEV_PROFILE_FUNCTION();

        m_LightningConstantBuffer->SetData(&m_LightningData);
        m_LightningConstantBuffer->Bind(ShaderType::Pixel);
        m_LightningData.LightsCount = 0;
    }
}
