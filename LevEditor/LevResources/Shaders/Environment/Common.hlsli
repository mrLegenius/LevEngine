struct PS_IN
{
    float4 pos : SV_Position;
    uint slice : SV_RenderTargetArrayIndex;
    float3 uv : UV;
};

static const float PI = 3.14159265359f;