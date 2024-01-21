#pragma once
#include <SimpleMath.h>

using Vector3 = DirectX::SimpleMath::Vector3;

template<class Archive>
void serialize(Archive& archive, Vector3 & vector)
{
    archive(vector.x, vector.y, vector.z);
}
