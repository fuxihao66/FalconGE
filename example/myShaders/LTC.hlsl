
cbuffer RootConstants : register(b0, space0)    
{
    uint LIGHT_NUM;
    uint FRAME_COUNT;
    float3 camPosW;
};



structedBuffer<LightGeometry> LightInfo : register(t5, space0);

Texture2D<float4> gDiffuse : register(t0, space0);
Texture2D<float4> gNormal : register(t1, space0);
Texture2D<float4> gSpecular : register(t2, space0);
Texture2D<float4> gRoughness : register(t3, space0);



Texture2D<float4> gLTCIso0 : register(t4, space0); // 这个不是屏幕空间纹理，uv分别对应入射光线cos\theta和roughness 并且存的是M^{-1}
Texture2D<float4> gLTCIso1 : register(t5, space0); // 这个不是屏幕空间纹理，uv分别对应入射光线cos\theta和roughness 并且存的是M^{-1}
// 分别对应FC*G*D 和 (1-FC)*G*D

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

float Integrate(float3 p0, float3 p1, float3 p2){

}

    // 把F项拆分成两项 分别fitting 
float3 ComputeLTCArea(LightGeometry lg, float3 cameraRay, float3 albedo, float3 specularColor, float roughness, float3 normalW ){

    float theta = dot(cameraRay, normalW)

    float4 matVal0 = gLTCIso0.Sample(g_linearSampler_Border, float2(theta, roughness));
    float4 matVal1 = gLTCIso1.Sample(g_linearSampler_Border, float2(theta, roughness));
    float3x3 matInverse0 = float3x3(matVal0.x, 0, matVal0.y, 0, matVal0.z, 0, matVal0.w, 0, 1);
    float3x3 matInverse1 = float3x3(matVal1.x, 0, matVal1.y, 0, matVal1.z, 0, matVal1.w, 0, 1);

    float tP0 = matInverse0 * lg.p0;
    float tP1 = matInverse0 * lg.p1;
    float tP2 = matInverse0 * lg.p2;

    float mtP0 = matInverse1 * lg.p0;
    float mtP1 = matInverse1 * lg.p1;
    float mtP2 = matInverse1 * lg.p2;

    float integral0 = Integrate(tP0, tP1, tP2);
    float integral1 = Integrate(mtP0, mtP1, mtP2);


    float3 integral = saturate( 50.0 * specularColor.g ) * integral0 + integral1 * specularColor + albedo;
    return lg.Le * integral;
}


// struct PSOutput2
// {
// 	float4 WorldPos     : COLOR0;
//     float4 Normal       : COLOR1;
//     float4 Albedo       : COLOR2;
//     float4 Specular     : COLOR3;
//     float4 Roughness    : COLOR4;
// 	//float Depth : DEPTH;
// };



float4 PSMain(PSInput input) : SV_TARGET
{
    float4 result = float4(0.0, 0.0, 0.0, 1.0);


    const float4 worldPos = gWorldPos[launchIndex];

    const float3 albedo = gDiffuse[launchIndex].xyz;
    const float3 specularColor = gSpecular[launchIndex].xyz;
    const float roughness = gRoughness[launchIndex].x;
    const float3 normalW = gNormal[launchIndex].xyz;



    float3 camRay = camPosW - worldPos.xyz;
    for (int i = 0; i < LIGHT_NUM; i++){
        result.xyz += ComputeLTCArea(LightInfo[i], camRay, albedo, specularColor, roughness, normalW);
    }


    return result;
}