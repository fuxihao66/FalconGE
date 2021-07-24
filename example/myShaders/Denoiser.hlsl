// TODO: 暂时只实现了一个简单的cross bilateral




#define KERNEL_SIZE 5
#define TILE_SIZE 16

cbuffer RootConstants : register(b0, space1)    
{
    uint2 texDim;
};

Texture2D gBeforeFiltered : register(t0, space0);
Texture2D gNormal : register(t1, space0);

RWTexture2D<float4> gFilterResults : register(u0, space0);




// sampler 所有shader中都一样  不需要管???
SamplerState g_linearSampler_Warp : register(s0);
SamplerState g_linearSampler_Mirror : register(s1);
SamplerState g_linearSampler_Clamp : register(s2);
SamplerState g_linearSampler_Border : register(s3);
SamplerState g_pointSampler_Warp : register(s4);
SamplerState g_pointSampler_Mirror : register(s5);
SamplerState g_pointSampler_Clamp : register(s6);
SamplerState g_pointSampler_Border : register(s7);



bool InBound(int2 pixelPos){
    if (pixelPos.x < 0 || pixelPos.y < 0)
        return false;
    if (pixelPos.x >= texDim.x || pixelPos.y >= texDim.y)
        return false;

    return true;
}

float CrossBilateralWeight(float3 value1, float3 value2, uint2 pixelPos1, uint2 pixelPos2){
    float factor = -1.0*(pixelPos1.x - pixelPos2.x)*(pixelPos1.x - pixelPos2.x)-1*(pixelPos1.y - pixelPos2.y)*(pixelPos1.y - pixelPos2.y)
    float bandSqr = 4.0;
    float weight1 = exp(factor/bandSqr)

    return max(0.0, dot(value1, value2)) * weight1;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void MainCS(
	uint2 GroupId : SV_GroupID,
	uint GroupThreadIndex : SV_GroupIndex)
{
    int2 DispatchThreadId = (
		ZOrder2D(GroupThreadIndex, log2(TILE_SIZE)) +
		GroupId * uint2(TILE_SIZE, TILE_SIZE));


    float2 uv = (DispatchThreadId + float2(0.5, 0.5)) / texDim;
	float4 result = float4(0.0, 0.0, 0.0, 1.0);
    float weight = 0.0;
    for (int i = -KERNEL_SIZE/2; i <= KERNEL_SIZE/2; i++ ){
        for (int j = -KERNEL_SIZE/2; j <= KERNEL_SIZE/2; j++){
            int2 newPixelPos = DispatchThreadId + int2(i, j);
            float2 uv2 = (newPixelPos + float2(0.5, 0.5)) / texDim;

            if (InBound(newPixelPos)){
                float3 v1 = gNormal.Sample(g_linearSampler_Border, uv);
                float3 v2 = gNormal.Sample(g_linearSampler_Border, uv2);
                
                weight += CrossBilateralWeight(v1, v2, DispatchThreadId, newPixelPos);
                result.xyz += weight*gBeforeFiltered[newPixelPos].xyz;
            } 
        }
    }

    result /= weight;

    result.w = 1.0;
    gFilterResults[DispatchThreadId] = result;

}