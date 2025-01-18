#include "levpch.h"
#include "Scene/Serializers/SerializerUtils.h"
#include "TransformSerializer.h"

#include "Transform.h"

namespace LevEngine
{
    void TransformSerializer::SerializeData(YAML::Emitter& out, const Transform& component)
    {
        out << YAML::Key << "Transform";
        out << YAML::BeginMap;
        
        out << YAML::Key << "ChildIndex" << YAML::Value << component.GetChildIndex();
        out << YAML::Key << "Position" << YAML::Value << component.GetLocalPosition();
        out << YAML::Key << "Rotation" << YAML::Value << component.GetLocalRotation().ToEuler();
        out << YAML::Key << "Scale" << YAML::Value << component.GetLocalScale();

        out << YAML::EndMap;
    }

    void TransformSerializer::DeserializeData(const YAML::Node& entityNode, Transform& component)
    {
        auto transformNode = entityNode["Transform"];
        
        component.SetLocalPosition(transformNode["Position"].as<Vector3>());
        component.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(transformNode["Rotation"].as<Vector3>()));
        component.SetLocalScale(transformNode["Scale"].as<Vector3>());

        uint32_t childIndex = 0;
        if (TryParse(transformNode["ChildIndex"], childIndex))
            component.childIndex = childIndex;

        //TODO: Find a better way to keep order of children
        component.parent.GetComponent<Transform>().SortChildren();
    }
}
