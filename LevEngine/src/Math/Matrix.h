#pragma once
#include <SimpleMath.h>

using Matrix = DirectX::SimpleMath::Matrix;

template<class Archive>
void serialize(Archive& archive, Matrix & matrix)
{
    archive(
        matrix._11, matrix._12, matrix._13, matrix._14,
        matrix._21, matrix._22, matrix._23, matrix._24,
        matrix._31, matrix._32, matrix._33, matrix._34,
        matrix._41, matrix._42, matrix._43, matrix._44);
}