1. 管线  和传统的graphics和compute管线不同,并且设置管线需要不同api:SetPipelineState1;但是RS就是普通的compute RS
2. shader table
3. AS创建
4. attribute 

### local RS
本质是进行资源可见性控制

### Ray Tracing和传统管线步骤
创建管线的方式不同（同时需要创建SBT和AS）
AS需要绑定
SBT需要绑定（Dispatch Ray的参数）

Root Signature和compute管线相同（但是支持local rootsignature）



### 如何定义hit中的参数BuiltInTriangleIntersectionAttributes
不能定义;
其是RTX库中已经定义好的了:
```
struct BuiltInTriangleIntersectionAttributes
{
    float2 barycentrics;
};
```
只需要用这个来直接插值就可以了
```
[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    float3 hitPosition = HitWorldPosition();

    // Get the base index of the triangle's first 16 bit index.
    uint indexSizeInBytes = 2;
    uint indicesPerTriangle = 3;
    uint triangleIndexStride = indicesPerTriangle * indexSizeInBytes;
    uint baseIndex = PrimitiveIndex() * triangleIndexStride;

    // Load up 3 16 bit indices for the triangle.
    const uint3 indices = Load3x16BitIndices(baseIndex);

    // Retrieve corresponding vertex normals for the triangle vertices.
    float3 vertexNormals[3] = { 
        Vertices[indices[0]].normal, 
        Vertices[indices[1]].normal, 
        Vertices[indices[2]].normal 
    };

    // Compute the triangle's normal.
    // This is redundant and done for illustration purposes 
    // as all the per-vertex normals are the same and match triangle's normal in this sample. 
    float3 triangleNormal = vertexNormals[0] +
        attr.barycentrics.x * (vertexNormals[1] - vertexNormals[0]) +
        attr.barycentrics.y * (vertexNormals[2] - vertexNormals[0]);

    float4 diffuseColor = CalculateDiffuseLighting(hitPosition, triangleNormal);
    float4 color = g_sceneCB.lightAmbientColor + diffuseColor;

    payload.color = color;
}
```

### Shader Binding Table
https://www.willusher.io/graphics/2019/11/20/the-sbt-three-ways
一个shader binding table由一个ray gen和多个miss hit 的record构成
一个ray gen可以对应很多个miss和很多个hit group(叫做group是因为有any closest ..)
每次需要把一个ray gen,一个miss shader record和一个hit group records绑定到管线上

通过trace ray的参数来决定调用哪个miss和哪个hit group

### TraceRay参数
在shader中,通过traceRay来发出光线
```
void TraceRay(RaytracingAccelerationStructure AccelerationStructure,
              uint RayFlags,
              uint InstanceInclusionMask,
              uint RayContributionToHitGroupIndex,
              uint MultiplierForGeometryContributionToHitGroupIndex,
              uint MissShaderIndex,
              RayDesc Ray,
              inout payload_t Payload);
```
InstanceInclusionMask说明哪些instance需要被mask?
RayContributionToHitGroupIndex是hit group的index,MissShaderIndex是miss的index;

