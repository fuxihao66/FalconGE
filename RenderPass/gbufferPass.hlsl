

// cbuffer {

// };
ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);


PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}


float4 PSMain(PSInput input) : SV_TARGET
{
    // This draws a perceptual gradient rather than a linear gradient.
    return float4(pow(abs(input.color.rgb), 2.2), input.color.a);
}