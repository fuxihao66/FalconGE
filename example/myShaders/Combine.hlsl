

Texture2D<float4> gLTC : register(t0, space0);
Texture2D<float4> gUnshadowedFiltered : register(t1, space0);
Texture2D<float4> gShadowedFiltered : register(t2, space0);


// sampler 所有shader中都一样  不需要管???
SamplerState g_linearSampler_Warp : register(s0);
SamplerState g_linearSampler_Mirror : register(s1);
SamplerState g_linearSampler_Clamp : register(s2);
SamplerState g_linearSampler_Border : register(s3);
SamplerState g_pointSampler_Warp : register(s4);
SamplerState g_pointSampler_Mirror : register(s5);
SamplerState g_pointSampler_Clamp : register(s6);
SamplerState g_pointSampler_Border : register(s7);



struct PSInput
{
    float4 position : SV_POSITION;
    float4 worldPos : COLOR0;
    float2 uv : TEXCOORD;
};


float4 PSMain(PSInput input) : SV_TARGET
{
    float4 result = float4(0.0, 0.0, 0.0, 1.0);

    float4 ltc = gLTC.Sample(g_linearSampler_Border, input.uv);
    float4 uf = gUnshadowedFiltered.Sample(g_linearSampler_Border, input.uv);
    float4 sf = gShadowedFiltered.Sample(g_linearSampler_Border, input.uv);

    result = ltc*sf/uf;
    result.w = 1.0;
    return result;
}