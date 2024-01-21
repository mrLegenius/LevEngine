#pragma once
#include <SimpleMath.h>

using Vector2 = DirectX::SimpleMath::Vector2;

template<class Archive>
void serialize(Archive& archive, Vector2 & vector)
{
    archive(vector.x, vector.y);
}