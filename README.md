
问题
1.  D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE和 Non Pixel shader resource

后续调试
1. scene   为每个mesh创建vertex buffer和index buffer；之后对每个vertex buffer创建BLAS（bottom level），最终创建一个完整的TLAS（参考Falcor scene.cpp）
2. ltc
   1. 推导
   2. 如何处理UE brdf（官方？） fresnel  主要是要求D是Normalized Distribution
   3. Textured？
3. 其他部分代码整理完成





CPU和GPU descriptor handle    资源的View：
1. 创建时候需要CPU Handle
2. 创建之后可以获取GPU handle，用于绑定






资源绑定与Root Signature创建
bindingNameToRootSignatureSlot是RS的vector
每一个RS负责一个renderPass
bindingNameToRootSignatureSlot[index]对应某个renderPass的root signature slot mapping，把键值映射到slot
对于SRV和UAV table，用其shader名+srv/uav来索引
对于const，用shader名+变量名来索引
对于AS，用gScene+shader名来索引


1. 对于Acceleration Struction

2. 对于constant
    * 通过shader reflection来获取变量名和register 创建shader object
    * 通过shader object创建shader resource binding，创建root signature（root signature为每一个变量创建parameter）
    * 
3. 对于uav/srv table 
   * 通过shader reflection来获取变量名和register 创建shader object
   * 通过shader object创建shader resource binding，创建root signature（root signature创建两个parameter，对应srv和uav table）
   * shader resource binding为resource创建descriptor
   * 运行时通过shader object的srv和uav table名称获取slot，绑定srv/uav descriptor 到slot上


