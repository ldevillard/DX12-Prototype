#pragma once

#include <DirectXMath.h>

using Vector4 = DirectX::XMVECTOR;
using Matrix4 = DirectX::XMMATRIX;

inline void Vec4ToFloat(Vector4 vec, FLOAT* outArray)
{
    DirectX::XMFLOAT4 float4;
    DirectX::XMStoreFloat4(&float4, vec);

    outArray[0] = float4.x;
    outArray[1] = float4.y;
    outArray[2] = float4.z;
    outArray[3] = float4.w;
}