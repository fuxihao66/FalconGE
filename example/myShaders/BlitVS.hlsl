
struct PSInput
{
    float4 position : SV_POSITION;
    float4 worldPos : COLOR0;
    float2 uv : TEXCOORD;
};



PSInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = position * ModelMatrix * ViewProjMatrix;
    result.worldPos = position * ModelMatrix;
    result.uv = uv;

    return result;
}
