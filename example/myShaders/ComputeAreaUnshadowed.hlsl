
#include "utils.hlsli"


cbuffer RootConstants : register(b0, space1)    
{
    uint LIGHT_NUM;
    uint SPP;
    uint FRAME_COUNT;
    uint2 texDim;
};


struct LightGeometry{
    float3 Le;
    float3 p0;
    float3 p1;
    float3 p2;
    float  area;
};




Texture2D<float4> gDiffuse : register(t0, space1);
Texture2D<float4> gNormal : register(t1, space1);
Texture2D<float4> gSpecular : register(t2, space1);
Texture2D<float4> gRoughness : register(t3, space1);
Texture2D<float4> gWorldPos : register(t4, space1);
structedBuffer<LightGeometry> LightInfo : register(t5, space1);

RWTexture2D<float4> gUnshadowedResults : register(u0, space1);



// sampler 所有shader中都一样  不需要管???
SamplerState g_linearSampler_Warp : register(s0);
SamplerState g_linearSampler_Mirror : register(s1);
SamplerState g_linearSampler_Clamp : register(s2);
SamplerState g_linearSampler_Border : register(s3);
SamplerState g_pointSampler_Warp : register(s4);
SamplerState g_pointSampler_Mirror : register(s5);
SamplerState g_pointSampler_Clamp : register(s6);
SamplerState g_pointSampler_Border : register(s7);



#define TILE_SIZE 16

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void MainCS(
	uint2 GroupId : SV_GroupID,
	uint GroupThreadIndex : SV_GroupIndex)
{
    uint2 DispatchThreadId = (
		ZOrder2D(GroupThreadIndex, log2(TILE_SIZE)) +
		GroupId * uint2(TILE_SIZE, TILE_SIZE));


    // uint2 launchIndex = DispatchRaysIndex().xy;
    // uint2 launchDim = DispatchRaysDimensions().xy;
    uint2 launchIndex = DispatchThreadId;
    uint2 launchDim = texDim;

    const uint WIDTH = launchDim.x; 
    const uint HEIGHT = launchDim.y; 

    const float4 worldPos = gWorldPos[launchIndex];
    float2 uv = (float2(launchIndex) + float2(0.5, 0.5) ) / float2(launchDim);

    
    uint rndSeed = RND::CreateRandomSeed(texDim, launchIndex, FRAME_COUNT);

    const float3 albedo = gDiffuse[launchIndex].xyz;
    const float3 specularColor = gSpecular[launchIndex].xyz;
    const float roughness = gRoughness[launchIndex].x;
    const float3 normalW = gNormal[launchIndex].xyz;


    const float3 cameraRay = normalize(camPosW - worldPos.xyz);

    
    if (worldPos.w != 0.f)  // Using w to indicate valid geometry 
    {

        float4 result = float4(0.0, 0.0, 0.0, 1.0);
        // SampleUniform01();

        for (int lIndex = 0; lIndex < LIGHT_NUM; lIndex++){
            for (int i = 0; i < SPP; i++){

                float r0 = RND::SampleUniform01(rndSeed);
                float r1 = RND::SampleUniform01(rndSeed);
                float3 v1 = (LightInfo[lIndex].p1 - LightInfo[lIndex].p0);
                float3 v2 = (LightInfo[lIndex].p2 - LightInfo[lIndex].p0);
                float3 p1 = LightInfo[lIndex].p0 + r0 * v1 + r1 * v2;
                float3 p2 = LightInfo[lIndex].p0 + (1-r0)*v1 + (1-r1)*v2;
                float3 sp = length(LightInfo[lIndex].p0 - p1) > length(LightInfo[lIndex].p0 - p2) ? p2 : p1;

                float3 dir = sp - worldPos.xyz;
                float dist = length(dir);
                dir = dir / dist;
                
                

                float3 lightNorm = normalize(cross(v1, v2));

                result += LightInfo[lIndex].Le * brdf(cameraRay, dir, normalW, albedo, specularColor, roughness) * G(dot(normalW, dir), dot(lightNorm, dir), dist) * LightInfo[lIndex].area;
            }
        }
        result /= float(SPP);
        result.w = 1.0;

        gUnshadowedResults[DispatchThreadId] = result;
        
    }  
    else{
        gUnshadowedResults[launchIndex] = float4(0.0, 0.0, 0.0, 1.0);
    }  
}