struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

PS_IN VSMain(uint vI : SV_VERTEXID)
{
    PS_IN output;

    float2 texcoord = float2(vI & 1, vI >> 1);
    output.uv = texcoord;
    output.pos = float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
    return output;
}