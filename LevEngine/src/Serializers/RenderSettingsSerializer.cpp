#include "levpch.h"
#include "RenderSettingsSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    void RenderSettingsSerializer::Serialize(YAML::Emitter& out)
    {
        out << YAML::Key << "RenderSettings" << YAML::Value << YAML::BeginMap;
        {
            out << YAML::Key << "PostProcessing" << YAML::Value << YAML::BeginMap;
            {
                out << YAML::Key << "IsBloomEnabled" << YAML::Value << RenderSettings::IsBloomEnabled;
                out << YAML::Key << "BloomThreshold" << YAML::Value << RenderSettings::BloomThreshold;
                out << YAML::Key << "BloomMagnitude" << YAML::Value << RenderSettings::BloomMagnitude;
                out << YAML::Key << "BloomBlurSigma" << YAML::Value << RenderSettings::BloomBlurSigma;

                out << YAML::Key << "AdaptationRate" << YAML::Value << RenderSettings::AdaptationRate;
                out << YAML::Key << "KeyValue" << YAML::Value << RenderSettings::KeyValue;
                out << YAML::Key << "MinExposure" << YAML::Value << RenderSettings::MinExposure;
                out << YAML::Key << "MaxExposure" << YAML::Value << RenderSettings::MaxExposure;
                
                out << YAML::Key << "IsVignetteEnabled" << YAML::Value << RenderSettings::IsVignetteEnabled;
                out << YAML::Key << "VignetteRadius" << YAML::Value << RenderSettings::VignetteRadius;
                out << YAML::Key << "VignetteSoftness" << YAML::Value << RenderSettings::VignetteSoftness;
                out << YAML::Key << "VignetteColor" << YAML::Value << RenderSettings::VignetteColor;
                out << YAML::Key << "VignetteIntensity" << YAML::Value << RenderSettings::VignetteIntensity;
            }
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
    }

    void RenderSettingsSerializer::Deserialize(const YAML::Node& node)
    {
        if (const auto& renderSettings = node["RenderSettings"])
        {
            if (const auto pp = renderSettings["PostProcessing"])
            {
                TryParse(pp["IsBloomEnabled"], RenderSettings::IsBloomEnabled);
                TryParse(pp["BloomThreshold"], RenderSettings::BloomThreshold);
                TryParse(pp["BloomMagnitude"], RenderSettings::BloomMagnitude);
                TryParse(pp["BloomBlurSigma"], RenderSettings::BloomBlurSigma);
                
                TryParse(pp["AdaptationRate"], RenderSettings::AdaptationRate);
                TryParse(pp["KeyValue"], RenderSettings::KeyValue);
                TryParse(pp["MinExposure"], RenderSettings::MinExposure);
                TryParse(pp["MaxExposure"], RenderSettings::MaxExposure);
                
                TryParse(pp["IsVignetteEnabled"], RenderSettings::IsVignetteEnabled);
                TryParse(pp["VignetteRadius"], RenderSettings::VignetteRadius);
                TryParse(pp["VignetteSoftness"], RenderSettings::VignetteSoftness);
                TryParse(pp["VignetteColor"], RenderSettings::VignetteColor);
                TryParse(pp["VignetteIntensity"], RenderSettings::VignetteIntensity);
            }
        }
    }
}