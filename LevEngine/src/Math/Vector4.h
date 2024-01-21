#pragma once
#include <SimpleMath.h>

using Vector4 = DirectX::SimpleMath::Vector4;

template<class Archive>
void serialize(Archive& archive, Vector4 & vector)
{
    archive(vector.x, vector.y, vector.z, vector.w);
}