#pragma once
#include <SimpleMath.h>

using Quaternion = DirectX::SimpleMath::Quaternion;

template<class Archive>
void serialize(Archive& archive, Quaternion & quaternion)
{
    archive(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}