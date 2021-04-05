

cbuffer RootConstants : register(b0, space0)    // 要求用户保证register与变量名一一对应
{
    float standardNits;
    float4x4 ModelMatrix;
    float4x4 ViewProjMatrix;
};


Texture2D<float4> gDiffuse : register(t0, space0);
Texture2D<float4> gNormal : register(t1, space0);
Texture2D<float4> gSpecular : register(t2, space0);
Texture2D<float4> gRoughness : register(t3, space0);


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




struct PSOutput2
{
	float4 WorldPos     : COLOR0;
    float4 Normal       : COLOR1;
    float4 Albedo       : COLOR2;
    float4 Specular     : COLOR3;
    float4 Roughness    : COLOR4;
	//float Depth : DEPTH;
};

PSInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = position * ModelMatrix * ViewProjMatrix;
    result.worldPos = position * ModelMatrix;
    result.uv = uv;

    return result;
}



PSOutput2 PSMain(PSInput input) : SV_TARGET
{
    PSOutput2 pso;

    pso.WorldPos = float4(input.worldPos, 1.0f);
    pso.Normal = gNormal.Sample(g_linearSampler_Border, input.uv);
    pso.Albedo = gAlbedo.Sample(g_linearSampler_Border, input.uv);
    pso.Specular = gSpecular.Sample(g_linearSampler_Border, input.uv);
    pso.Roughness = gRoughness.Sample(g_linearSampler_Border, input.uv);


    return pso;
}