

#include "utils.hlsl"




cbuffer RootConstants : register(b0, space2)    
{
    uint LIGHT_NUM;
    uint SPP;
    uint FRAME_COUNT;
    float3 camPosW;
};




Texture2D<float4> gDiffuse : register(t0, space2);
Texture2D<float4> gNormal : register(t1, space2);
Texture2D<float4> gSpecular : register(t2, space2);
Texture2D<float4> gRoughness : register(t3, space2);
Texture2D<float4> gWorldPos : register(t4, space2);
structedBuffer<LightGeometry> LightInfo : register(t5, space2);


RaytracingAccelerationStructure gScene : register(t6, space2);


RWTexture2D<float4> gShadowedResults : register(u0, space2);



// sampler 所有shader中都一样  不需要管???
SamplerState g_linearSampler_Warp : register(s0);
SamplerState g_linearSampler_Mirror : register(s1);
SamplerState g_linearSampler_Clamp : register(s2);
SamplerState g_linearSampler_Border : register(s3);
SamplerState g_pointSampler_Warp : register(s4);
SamplerState g_pointSampler_Mirror : register(s5);
SamplerState g_pointSampler_Clamp : register(s6);
SamplerState g_pointSampler_Border : register(s7);

/** Payload for shadow ray.
*/
struct RayData
{
    bool intersected;
    float dist;
    float3 rayOrigin;
};



[shader("miss")]
void ShadowMiss(inout RayData rayData)
{
    rayData.intersected = false;

}



[shader("closesthit")]
void ShadowClosestHit(
    uniform HitShaderParams hitParams,
    inout RayData rayData, in BuiltInTriangleIntersectionAttributes attribs)
{
    rayData.intersected = true;

    // Evaluate Falcor's material parameters at the hit point.
    // TODO: Implement texLOD to enable texture filtering in prepareShadingData().
    VertexData v = getVertexData(hitParams, PrimitiveIndex(), attribs);


    rayData.dist = length(v.posW - rayData.rayOrigin);
}




float3 brdf(in float3 inVec, in float3 outVec, in float3 normal, in float3 diffuseColor, in float3 spec, in float roughness){

    float3 diffusePart = diffuseColor / PI;

    float NoH = dot(normal, normalize(inVec+outVec));
    float roughnessSqr = roughness*roughness;
    float d = ( NoH * roughnessSqr - NoH ) * NoH + 1;
    float D = roughnessSqr / ( PI * d * d);


    float VoH = dot(inVec, normalize(inVec+outVec));
    float Fc = pow( 1 - VoH, 5 );					
	float3 F = saturate( 50.0 * spec.g ) * Fc + (1 - Fc) * spec;

    float NoV = dot(normal, inVec);
    float NoL = dot(normal, outVec);

    float temp = ( roughness + 1 ) / 2;
    float k = temp * temp * 0.5;
	float Vis_SchlickV = NoV * (1 - k) + k;
	float Vis_SchlickL = NoL * (1 - k) + k;
	float GT = ( Vis_SchlickV * Vis_SchlickL );

    float3 specularPart = D*F*GT / (4 * NoV * NoL);
    // return 1.0;

    return diffusePart + specularPart;
}


float G(in float cosTheta, in float cosGamma, in float dist){
    return abs(cosTheta * cosGamma) / (dist * dist);
}

[shader("raygeneration")]
void rayGen()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 launchDim = DispatchRaysDimensions().xy;


    const uint WIDTH = launchDim.x; 
    const uint HEIGHT = launchDim.y; 

    const float4 worldPos = gWorldPos[launchIndex];
    float2 uv = (float2(launchIndex) + float2(0.5, 0.5) ) / float2(launchDim);

    
    const float3 albedo = gDiffuse[launchIndex].xyz;
    const float3 specularColor = gSpecular[launchIndex].xyz;
    const float roughness = gRoughness[launchIndex].x;
    const float3 normalW = gNormal[launchIndex].xyz;
    // TODO: 随机种子必须要和Shadowed不一样
    uint rndSeed = RND::CreateRandomSeed(texDim, launchIndex, FRAME_COUNT*(FRAME_COUNT%5+2));

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

                // float l0 = length(lightPos[1] - lightPos[2] );
                // float l1 = length(v1);
                // float l2 = length(v2);
                // float halfC = (l0+l1+l2)/2.0;
                // float area = sqrt(halfC*(halfC-l0)*(halfC-l1)*(halfC-l2));


                float3 dir = sp - worldPos.xyz;
                float dist = length(dir);
                dir = dir / dist;
                float3 origin = worldPos.xyz + 1e-4*dir; // 光栅化导致worldPos在实际三角形的下方，导致shadow ray打到自己

                RayDesc ray;
                ray.Origin = origin;
                ray.Direction = dir;
                ray.TMin = 0.f;
                ray.TMax = FLT_MAX;

                uint rayFlags = 0;     

                RayData rayData;
                rayData.rayOrigin = origin;

                TraceRay(gScene,
                    rayFlags,
                    RTAOTraceRayParameters::InstanceMask,
                    RTAOTraceRayParameters::HitGroup::Offset[RTAORayType::AO],
                    RTAOTraceRayParameters::HitGroup::GeometryStride,
                    RTAOTraceRayParameters::MissShader::Offset[RTAORayType::AO],
                    ray, shadowPayload);

                // TraceRay(gRtScene, rayFlags, 0xff , 0 , hitProgramCount, 0 , ray, rayData);
                // // 用第一组hit miss group （只有一组）
                if (rayData.intersected && rayData.dist <= dist - 1e-3 ){
                    continue;
                }


                
                
                

                float3 lightNorm = normalize(cross(v1, v2));

                result += LightInfo[lIndex].Le * brdf(cameraRay, dir, normalW, albedo, specularColor, roughness) * G(dot(normalW, dir), dot(lightNorm, dir), dist) * LightInfo[lIndex].area;
            }
        }
        result /= float(SPP);
        result.w = 1.0;

        gShadowedResults[DispatchThreadId] = result;
        
    }  
    else{
        gShadowedResults[launchIndex] = float4(0.0, 0.0, 0.0, 1.0);
    }  

}
